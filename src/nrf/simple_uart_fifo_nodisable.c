/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdint.h>

#include "nrf.h"
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_uart.h"
#include "app_error.h"
#include "app_util.h"

#define UART_TX_BUF_SIZE 1024                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1024                           /**< UART RX buffer size. */


//I changes sth. to disable UART if no receiving device has been
//detected for a few seconds.

uint8_t simple_uart_get(void)
{
    uint8_t readbyte;
    while(app_uart_get((uint8_t*)&readbyte) != NRF_SUCCESS);

    return readbyte;
}


bool simple_uart_get_with_timeout(int32_t timeout_ms, uint8_t * rx_data)
{
	//if(disableUART) return false;

    bool ret = true;

    while (app_uart_get(rx_data) != NRF_SUCCESS)
    {
        if (timeout_ms-- > 0)
        {
            // wait in 1ms chunk before checking for status.
            nrf_delay_us(1000);
        }
        else
        {
            ret = false;
            break;
        }
    } // Wait for RXD data to be received.

    return ret;
}

void simple_uart_put(uint8_t cr)
{
    while(app_uart_put(cr) != NRF_SUCCESS);
}


void simple_uart_putstring(const uint8_t * str)
{
    uint_fast8_t i  = 0;
    uint8_t      ch = str[i++];

    while (ch != '\0')
    {
        simple_uart_put(ch);
        ch = str[i++];
    }
}


void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


void simple_uart_config(uint8_t rts_pin_number,
                        uint8_t txd_pin_number,
                        uint8_t cts_pin_number,
                        uint8_t rxd_pin_number,
                        bool    hwfc)
{

// uart pareams configuration
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
      {
          rxd_pin_number,
          txd_pin_number,
          rts_pin_number,
          cts_pin_number,
          hwfc?APP_UART_FLOW_CONTROL_ENABLED:APP_UART_FLOW_CONTROL_DISABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud38400
      };

    /** @snippet [Configure UART RX and TX pin] */
    /*
    nrf_gpio_cfg_output(txd_pin_number);
    nrf_gpio_cfg_input(rxd_pin_number, NRF_GPIO_PIN_NOPULL);
    */

    /** @snippet [Configure UART RX and TX pin] */
    /*
    if (hwfc)
    {
        nrf_gpio_cfg_output(rts_pin_number);
        nrf_gpio_cfg_input(cts_pin_number, NRF_GPIO_PIN_NOPULL);
    }
    */

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_HIGH,
                         err_code);

    APP_ERROR_CHECK(err_code);

}



