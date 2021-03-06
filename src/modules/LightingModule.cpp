/**

Copyright (c) 2014-2015 "M-Way Solutions GmbH"
FruityMesh - Bluetooth Low Energy mesh protocol [http://mwaysolutions.com/]

This file is part of FruityMesh

FruityMesh is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <LightingModule.h>
#include <Logger.h>
#include <Utility.h>
#include <Storage.h>
#include <Node.h>

extern "C"{
	#include <stdlib.h>
}

APP_PWM_INSTANCE(PWM1,1);
APP_PWM_INSTANCE(PWM2,2);

static volatile bool pwm1_ready_flag, pwm2_ready_flag;            // A flag indicating PWM status.

volatile int32_t light_sensor_adc_val;
volatile app_pwm_duty_t wrgb_buffer[4] = {0, 0, 0, 0};

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
	if(pwm_id == 1) {
		pwm1_ready_flag = true;
	}
	if (pwm_id == 2)
	{
		pwm2_ready_flag = true;
	}
}

/**
 * @brief Initialize meshlight hardware.
 */
void meshlight_init(void)
{
    /* 2-channel PWM, 200Hz, output on PWM pins. */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, PWM_WLED_PIN, PWM_RLED_PIN);
    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, PWM_GLED_PIN, PWM_BLED_PIN);

    /* Switch the polarity of the second channel. */
    pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm2_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
    ret_code_t err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);

    err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);

	while(app_pwm_channel_duty_set(&PWM1, 0, 80) == NRF_ERROR_BUSY);
	while(app_pwm_channel_duty_set(&PWM1, 1, 20) == NRF_ERROR_BUSY);
	while(app_pwm_channel_duty_set(&PWM2, 0, 20) == NRF_ERROR_BUSY);
	while(app_pwm_channel_duty_set(&PWM2, 1, 20) == NRF_ERROR_BUSY);

    /* Initialize PIR sensor PIN as input */
    nrf_gpio_cfg_input(PIR_SENSOR_PIN, NRF_GPIO_PIN_PULLDOWN);


    const nrf_adc_config_t nrf_adc_config = NRF_ADC_CONFIG_DEFAULT;

    // Initialize and configure ADC
    nrf_adc_configure( (nrf_adc_config_t *)&nrf_adc_config);
    //nrf_adc_input_select(LIGHT_SENSOR_INPUT);
    //nrf_adc_int_enable(ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos);
    //NVIC_SetPriority(ADC_IRQn, NRF_APP_PRIORITY_HIGH);
    //NVIC_EnableIRQ(ADC_IRQn);

    //nrf_adc_start();

}

LightingModule::LightingModule(u16 moduleId, Node* node, ConnectionManager* cm, const char* name, u16 storageSlot)
	: Module(moduleId, node, cm, name, storageSlot)
{
	//Register callbacks n' stuff
	Logger::getInstance().enableTag("LIGHTINGMOD");

	//Save configuration to base class variables
	//sizeof configuration must be a multiple of 4 bytes
	configurationPointer = &configuration;
	configurationLength = sizeof(LightingModuleConfiguration);

	//Start module configuration loading
	LoadModuleConfiguration();
}

void LightingModule::ConfigurationLoadedHandler()
{
	//Does basic testing on the loaded configuration
	Module::ConfigurationLoadedHandler();

	//Version migration can be added here
	if(configuration.moduleVersion == 1){/* ... */};

	//Do additional initialization upon loading the config
    meshlight_init();
	//Start the Module...

}

void LightingModule::TimerEventHandler(u16 passedTime, u32 appTimer)
{
	//Do stuff on timer...

}

void LightingModule::ResetToDefaultConfiguration()
{
	//Set default configuration values
	configuration.moduleId = moduleId;
	configuration.moduleActive = false;
	configuration.moduleVersion = 1;

	//Set additional config values...

}

bool LightingModule::TerminalCommandHandler(string commandName, vector<string> commandArgs)
{
	//React on commands, return true if handled, false otherwise
	if(commandArgs.size() >= 2 && commandArgs[1] == moduleName)
	{
		if(commandName == "action")
		{
			if(commandArgs[1] != moduleName) return false;

			nodeID destinationNode = (commandArgs[0] == "this") ? node->persistentConfig.nodeId : atoi(commandArgs[0].c_str());


			if(commandArgs.size() >= 7 && commandArgs[2] == "set_wrgb")
			{
				u8 buffer[4];
				buffer[0] = atoi(commandArgs[3].c_str());
				buffer[1] = atoi(commandArgs[4].c_str());
				buffer[2] = atoi(commandArgs[5].c_str());
				buffer[3] = atoi(commandArgs[6].c_str());

				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::SET_WRGB_CONFIG,
					0,
					buffer,
					4,
					false
				);

				return true;
			}

			else if(commandArgs.size() == 3 && commandArgs[2] == "open")
			{
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::OPEN_LIGHT,
					0,
					NULL,
					0,
					false
				);

				return true;
			}

			else if(commandArgs.size() == 3 && commandArgs[2] == "close")
			{
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::CLOSE_LIGHT,
					0,
					NULL,
					0,
					false
				);

				return true;
			}

			else if(commandArgs.size() == 3 && commandArgs[2] == "get_wrgb")
			{
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::GET_WRGB_CONFIG,
					0,
					NULL,
					0,
					false
				);

				return true;
			}

			else if(commandArgs.size() == 3 && commandArgs[2] == "get_pir")
			{
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::GET_PIR_VALUE,
					0,
					NULL,
					0,
					false
				);

				return true;
			}

			else if(commandArgs.size() == 3 && commandArgs[2] == "get_light")
			{
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::GET_LIGHT_VALUE,
					0,
					NULL,
					0,
					false
				);

				return true;
			}

			else if(commandArgs.size() >= 3 && commandArgs[2] == "set_config")
			{
				u8 config_buffer[32];

				//handle the configuration


				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::SET_AUTO_CONFIG,
					0,
					config_buffer,
					32,
					true
				);

				return true;
			}

			else if(commandArgs.size() == 3 && commandArgs[2] == "get_status")
			{
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_TRIGGER_ACTION,
					destinationNode,
					LightingModuleTriggerActionMessages::GET_STATUS,
					0,
					NULL,
					0,
					false
				);

				return true;
			}

			return false;

		}
	}

	//Must be called to allow the module to get and set the config
	return Module::TerminalCommandHandler(commandName, commandArgs);
}


void LightingModule::ConnectionPacketReceivedEventHandler(connectionPacket* inPacket, Connection* connection, connPacketHeader* packetHeader, u16 dataLength)
{
	//Must call superclass for handling
	Module::ConnectionPacketReceivedEventHandler(inPacket, connection, packetHeader, dataLength);

	if(packetHeader->messageType == MESSAGE_TYPE_MODULE_TRIGGER_ACTION){
		connPacketModule* packet = (connPacketModule*)packetHeader;
		u16 dataFieldLength = dataLength - SIZEOF_CONN_PACKET_MODULE;

		//Check if our module is meant and we should trigger an action
		if(packet->moduleId == moduleId){
			if(packet->actionType == LightingModuleTriggerActionMessages::SET_WRGB_CONFIG){

				/* Set the duty cycle - keep trying until PWM is ready... */
				while(app_pwm_channel_duty_set(&PWM1, 0, (app_pwm_duty_t)(packet->data[0])) == NRF_ERROR_BUSY);
				while(app_pwm_channel_duty_set(&PWM1, 1, (app_pwm_duty_t)(packet->data[1]*100/255)) == NRF_ERROR_BUSY);
				while(app_pwm_channel_duty_set(&PWM2, 0, (app_pwm_duty_t)(packet->data[2]*100/255)) == NRF_ERROR_BUSY);
				while(app_pwm_channel_duty_set(&PWM2, 1, (app_pwm_duty_t)(packet->data[3]*100/255)) == NRF_ERROR_BUSY);

				wrgb_buffer[0] = packet->data[0];
				wrgb_buffer[1] = packet->data[1];
				wrgb_buffer[2] = packet->data[2];
				wrgb_buffer[3] = packet->data[3];


				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::SET_WRGB_CONFIG_RESPONSE,
					packet->requestHandle,
					NULL,
					0,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::OPEN_LIGHT) {

				//app_pwm_enable(&PWM1);
                //app_pwm_enable(&PWM2);

				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::OPEN_LIGHT_RESPONSE,
					packet->requestHandle,
					NULL,
					0,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::CLOSE_LIGHT) {
			    /* Set the duty cycle - keep trying until PWM is ready... */
				while(app_pwm_channel_duty_set(&PWM1, 0, 0) == NRF_ERROR_BUSY);
				while(app_pwm_channel_duty_set(&PWM1, 1, 0) == NRF_ERROR_BUSY);
				while(app_pwm_channel_duty_set(&PWM2, 0, 0) == NRF_ERROR_BUSY);
				while(app_pwm_channel_duty_set(&PWM2, 1, 0) == NRF_ERROR_BUSY);

				wrgb_buffer[0] = 0;
				wrgb_buffer[1] = 0;
				wrgb_buffer[2] = 0;
				wrgb_buffer[3] = 0;

                //app_pwm_disable(&PWM1);
                //app_pwm_disable(&PWM2);
                
				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::CLOSE_LIGHT_RESPONSE,
					packet->requestHandle,
					NULL,
					0,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::GET_WRGB_CONFIG) {
				u8 buffer[4];
				buffer[0] = wrgb_buffer[0];
				buffer[1] = wrgb_buffer[1];
				buffer[2] = wrgb_buffer[2];
				buffer[3] = wrgb_buffer[3];

				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::GET_WRGB_CONFIG_RESPONSE,
					packet->requestHandle,
					buffer,
					4,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::GET_PIR_VALUE) {
				u8 buffer[1];
				buffer[0] = nrf_gpio_pin_read(PIR_SENSOR_PIN);
				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::PIR_VALUE_RESPONSE,
					packet->requestHandle,
					buffer,
					1,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::GET_LIGHT_VALUE) {
				u8 buffer[4];
				light_sensor_adc_val = nrf_adc_convert_single(LIGHT_SENSOR_INPUT);
				buffer[0] = light_sensor_adc_val & 0x000000FF;
				buffer[1] = (light_sensor_adc_val & 0x0000FF00) >> 8;
				buffer[2] = (light_sensor_adc_val & 0x00FF0000) >> 16;
				buffer[3] = (light_sensor_adc_val & 0xFF000000) >> 24;
 				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::LIGHT_VALUE_RESPONSE,
					packet->requestHandle,
					buffer,
					4,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::SET_AUTO_CONFIG) {

				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::AUTO_CONFIG_RESPONSE,
					packet->requestHandle,
					NULL,
					0,
					false
				);
			}

			else if(packet->actionType == LightingModuleTriggerActionMessages::GET_STATUS) {
				u8 buffer[7];
				light_sensor_adc_val = nrf_adc_convert_single(LIGHT_SENSOR_INPUT);
				buffer[0] = wrgb_buffer[0];
				buffer[1] = wrgb_buffer[1];
				buffer[2] = wrgb_buffer[2];
				buffer[3] = wrgb_buffer[3];
				buffer[4] = light_sensor_adc_val & 0x000000FF;
				buffer[5] = (light_sensor_adc_val & 0x0000FF00) >> 8;
				buffer[6] = nrf_gpio_pin_read(PIR_SENSOR_PIN);
 				//Confirmation
				SendModuleActionMessage(
					MESSAGE_TYPE_MODULE_ACTION_RESPONSE,
					packet->header.sender,
					LightingModuleActionResponseMessages::GET_STATUS_RESPONSE,
					packet->requestHandle,
					buffer,
					7,
					false
				);
			}

		}
	}

	//Parse Module responses
	if(packetHeader->messageType == MESSAGE_TYPE_MODULE_ACTION_RESPONSE){
		connPacketModule* packet = (connPacketModule*)packetHeader;

		//Check if our module is meant and we should trigger an action
		if(packet->moduleId == moduleId)
		{
			if(packet->actionType == LightingModuleActionResponseMessages::SET_WRGB_CONFIG_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"set_wrgb_result\",\"module\":%u,", packet->header.sender, packet->moduleId);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::OPEN_LIGHT_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"open_light_result\",\"module\":%u,", packet->header.sender, packet->moduleId);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::CLOSE_LIGHT_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"close_light_result\",\"module\":%u,", packet->header.sender, packet->moduleId);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::GET_WRGB_CONFIG_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"get_wrgb_result\",\"module\":%u,\"wrgb_value\":[%u,%u,%u,%u],", packet->header.sender, packet->moduleId, packet->data[0], packet->data[1], packet->data[2], packet->data[3]);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::PIR_VALUE_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"get_pir_result\",\"module\":%u,\"pir_value\":%u,", packet->header.sender, packet->moduleId, packet->data[0]);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::LIGHT_VALUE_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"get_light_result\",\"module\":%u,\"light_value\":%u,", packet->header.sender, packet->moduleId, packet->data[1]*256 + packet->data[0]);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::AUTO_CONFIG_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"set_config_result\",\"module\":%u,", packet->header.sender, packet->moduleId);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

			else if(packet->actionType == LightingModuleActionResponseMessages::GET_STATUS_RESPONSE)
			{
				uart("MODULE", "{\"nodeId\":%u,\"type\":\"get_status_result\",\"module\":%u,\"data\":[%u,%u,%u,%u,%u,%u,%u],", packet->header.sender, packet->moduleId, packet->data[0], packet->data[1], packet->data[2], packet->data[3], packet->data[4], packet->data[5], packet->data[6]);
				uart("MODULE",  "\"requestHandle\":%u,\"code\":%u}" SEP, packet->requestHandle, 0);
			}

		}
	}
}

