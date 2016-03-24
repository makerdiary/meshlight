/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
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
#ifndef MESHLIGHT_H
#define MESHLIGHT_H

// LEDs definitions for MeshLight
#define LEDS_NUMBER    3

#define LED_START      3
#define LED_RGB_RED    3
#define LED_RGB_GREEN  4
#define LED_RGB_BLUE   5
#define LED_STOP       5

#define LED_RGB_RED_MASK    (1<<LED_RGB_RED)
#define LED_RGB_GREEN_MASK  (1<<LED_RGB_GREEN)
#define LED_RGB_BLUE_MASK   (1<<LED_RGB_BLUE)

#define LEDS_LIST { LED_RGB_RED, LED_RGB_GREEN, LED_RGB_BLUE}
// defining RGB led as 3 single LEDs
#define BSP_LED_0 LED_RGB_RED
#define BSP_LED_1 LED_RGB_GREEN
#define BSP_LED_2 LED_RGB_BLUE

#define BSP_LED_0_MASK    (1<<BSP_LED_0)
#define BSP_LED_1_MASK    (1<<BSP_LED_1)
#define BSP_LED_2_MASK    (1<<BSP_LED_2)

#define INVERT_LEDS true

#define LEDS_MASK      (BSP_LED_0_MASK | BSP_LED_1_MASK | BSP_LED_2_MASK)
//defines which LEDs are lit when signal is low
#define LEDS_INV_MASK  LEDS_MASK

// there are no user buttons
#define BUTTONS_NUMBER 0
#define BUTTONS_LIST {}
#define BUTTONS_MASK   0x00000000

// UART connection with J-Link
#define RX_PIN_NUMBER  10
#define TX_PIN_NUMBER  11
#define CTS_PIN_NUMBER 7
#define RTS_PIN_NUMBER 17
#define HWFC           false

 // PWM definitions for LED lighting
 #define PWM_WLED_PIN  6
 #define PWM_RLED_PIN  13
 #define PWM_GLED_PIN  14
 #define PWM_BLED_PIN  15

#endif