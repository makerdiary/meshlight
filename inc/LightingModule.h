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

#pragma once

#include <Module.h>

class LightingModule: public Module
{
	private:

		//Module configuration that is saved persistently (size must be multiple of 4)
		struct LightingModuleConfiguration : ModuleConfiguration{
			//Insert more persistent config values here
		};

		LightingModuleConfiguration configuration;

		enum LightingModuleTriggerActionMessages{
			//MESSAGE_0 = 0
		};

		enum LightingModuleActionResponseMessages{
			//MESSAGE_0 = 0
		};

		/*
		//####### Module messages (these need to be packed)
		#pragma pack(push)
		#pragma pack(1)

			#define SIZEOF_TEMPLATE_MODULE_***_MESSAGE 10
			typedef struct
			{
				//Insert values here

			}LightingModule***Message;

		#pragma pack(pop)
		//####### Module messages end
		*/


	public:
		LightingModule(u16 moduleId, Node* node, ConnectionManager* cm, const char* name, u16 storageSlot);

		void ConfigurationLoadedHandler();

		void ResetToDefaultConfiguration();

		void TimerEventHandler(u16 passedTime, u32 appTimer);

		//void BleEventHandler(ble_evt_t* bleEvent);

		//void ConnectionPacketReceivedEventHandler(connectionPacket* inPacket, Connection* connection, connPacketHeader* packetHeader, u16 dataLength);

		//void NodeStateChangedHandler(discoveryState newState);

		bool TerminalCommandHandler(string commandName, vector<string> commandArgs);
};
