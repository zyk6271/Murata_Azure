/*
 * Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Defines board support package for MurataType1LD board
 */
#include "platform.h"
#include "platform_config.h"
#include "platform_init.h"
#include "platform_isr.h"
#include "platform_peripheral.h"
#include "platform_bluetooth.h"
#include "wwd_platform_common.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "wiced_platform.h"
#include "platform_mfi.h"
#include "platform_button.h"
#include "gpio_button.h"
#include "wiced_low_power.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

platform_result_t platform_bluetooth_powersave_wakeup_handler( void );
platform_result_t platform_bluetooth_powersave_sleep_handler( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* GPIO pin table. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_gpio_t platform_gpio_pins[] =
{
    [WICED_GPIO_1]  = { GPIOA,  4 }, /* PA4/SPI1_NSS   */
    [WICED_GPIO_2]  = { GPIOA,  5 }, /* PA5/SPI1_SCK   */
    [WICED_GPIO_3]  = { GPIOA,  6 }, /* PA6/SPI1_MISO  */
    [WICED_GPIO_4]  = { GPIOA,  7 }, /* PA7/SPI1_MOSI  */
    [WICED_GPIO_5]  = { GPIOA,  8 }, /* PA8/MCO_1      */
    [WICED_GPIO_6]  = { GPIOA,  9 }, /* USART1_TX      */
    [WICED_GPIO_7]  = { GPIOA, 10 }, /* USART1_RX      */
    [WICED_GPIO_8]  = { GPIOA, 11 }, /* USART1_CTS     */
    [WICED_GPIO_9]  = { GPIOA, 12 }, /* USART1_RTS     */
    [WICED_GPIO_10] = { GPIOA, 13 }, /* PA13/JTMS      */
    [WICED_GPIO_11] = { GPIOA, 14 }, /* PA14/JTCK      */
    [WICED_GPIO_12] = { GPIOA, 15 }, /* PA15/JTDI      */
    [WICED_GPIO_13] = { GPIOB,  0 }, /* PB0/I2S5_CK    */
    [WICED_GPIO_14] = { GPIOB,  1 }, /* PB1/I2S5_WS    */
    [WICED_GPIO_15] = { GPIOB,  2 }, /* PB2/BOOT1      */
    [WICED_GPIO_16] = { GPIOB,  3 }, /* PB3/JTDO       */
    [WICED_GPIO_17] = { GPIOB,  4 }, /* PB4/JTRST      */
    [WICED_GPIO_18] = { GPIOB,  5 }, /* PB5/I2C1_SMBA  */
    [WICED_GPIO_19] = { GPIOB,  6 }, /* PB6/I2C1_SCL   */
    [WICED_GPIO_20] = { GPIOB,  7 }, /* PB7/I2C1_SDA   */
    [WICED_GPIO_21] = { GPIOB,  8 }, /* PB8/I2S5_SD    */
    [WICED_GPIO_22] = { GPIOB,  9 }, /* PB9            */
    [WICED_GPIO_23] = { GPIOB, 10 }, /* PB10           */
    [WICED_GPIO_24] = { GPIOB, 12 }, /* PB12/SPI2_NSS  */
    [WICED_GPIO_25] = { GPIOB, 13 }, /* PB13/SPI2_SCK  */
    [WICED_GPIO_26] = { GPIOB, 14 }, /* PB14/SPI2_MISO */
    [WICED_GPIO_27] = { GPIOB, 15 }, /* PB15/SPI2_MOSI */
    [WICED_GPIO_28] = { GPIOC,  1 }, /* PC1/WAKE       */
    [WICED_GPIO_29] = { GPIOC,  4 }, /* PC4            */
    [WICED_GPIO_30] = { GPIOC,  5 }, /* PC5            */
    [WICED_GPIO_31] = { GPIOC,  6 }, /* USART6_TX      */
    [WICED_GPIO_32] = { GPIOC,  7 }, /* USART6_RX      */
    [WICED_GPIO_33] = { GPIOC, 14 }, /* PC14/OSC32_IN  */
    [WICED_GPIO_34] = { GPIOC, 15 }, /* PC14/OSC32_OUT */

};

/* ADC peripherals. Used WICED/platform/MCU/wiced_platform_common.c */
const platform_adc_t platform_adc_peripherals[] =
{
    [WICED_ADC_1] = {ADC1, ADC_Channel_15, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_22]}, /* SW2 */
};

/* PWM peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_pwm_t platform_pwm_peripherals[] =
{
    [WICED_PWM_1]  = {TIM3,  3, RCC_APB1Periph_TIM3,  GPIO_AF_TIM3,  &platform_gpio_pins[WICED_GPIO_11]}, //! LED1
    [WICED_PWM_2]  = {TIM3,  4, RCC_APB1Periph_TIM3,  GPIO_AF_TIM3,  &platform_gpio_pins[WICED_GPIO_12]}, //! LED2
    [WICED_PWM_3]  = {TIM4,  2, RCC_APB1Periph_TIM4,  GPIO_AF_TIM4,  &platform_gpio_pins[WICED_GPIO_14]}, //! LED3
};

/* SPI peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_spi_t platform_spi_peripherals[] =
{
    [WICED_SPI_1]  =
    {
        .port                  = SPI1,
        .gpio_af               = GPIO_AF_SPI1,
        .peripheral_clock_reg  = RCC_APB2Periph_SPI1,
        .peripheral_clock_func = RCC_APB2PeriphClockCmd,
        .pin_mosi              = &platform_gpio_pins[WICED_GPIO_4],
        .pin_miso              = &platform_gpio_pins[WICED_GPIO_3],
        .pin_clock             = &platform_gpio_pins[WICED_GPIO_2],
        .tx_dma =
        {
            .controller        = DMA2,
            .stream            = DMA2_Stream5,
            .channel           = DMA_Channel_3,
            .irq_vector        = DMA2_Stream5_IRQn,
            .complete_flags    = DMA_HISR_TCIF5,
            .error_flags       = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
        },
        .rx_dma =
        {
            .controller        = DMA2,
            .stream            = DMA2_Stream0,
            .channel           = DMA_Channel_3,
            .irq_vector        = DMA2_Stream0_IRQn,
            .complete_flags    = DMA_LISR_TCIF0,
            .error_flags       = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
        },
    },

    [WICED_SPI_2]  =
    {
        .port                  = SPI2,
        .gpio_af               = GPIO_AF_SPI2,
        .peripheral_clock_reg  = RCC_APB2Periph_SPI4,
        .peripheral_clock_func = RCC_APB2PeriphClockCmd,
        .pin_mosi              = &platform_gpio_pins[WICED_GPIO_27],
        .pin_miso              = &platform_gpio_pins[WICED_GPIO_26],
        .pin_clock             = &platform_gpio_pins[WICED_GPIO_25],
        .tx_dma =
        {
            .controller        = DMA1,
            .stream            = DMA1_Stream4,
            .channel           = DMA_Channel_0,
            .irq_vector        = DMA1_Stream4_IRQn,
            .complete_flags    = DMA_HISR_TCIF4,
            .error_flags       = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 | DMA_HISR_DMEIF4 ),
        },
        .rx_dma =
        {
            .controller        = DMA1,
            .stream            = DMA1_Stream3,
            .channel           = DMA_Channel_0,
            .irq_vector        = DMA1_Stream3_IRQn,
            .complete_flags    = DMA_LISR_TCIF3,
            .error_flags       = ( DMA_LISR_TEIF3 | DMA_LISR_FEIF3 | DMA_LISR_DMEIF3 ),
        },
    }
};

const platform_i2c_t platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .port                    = I2C1,
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_19],
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_20],
        .peripheral_clock_reg    = RCC_APB1Periph_I2C1,
        .tx_dma                  = DMA1,
        .tx_dma_peripheral_clock = RCC_AHB1Periph_DMA1,
        .tx_dma_stream           = DMA1_Stream7,
        .rx_dma_stream           = DMA1_Stream0,
        .tx_dma_stream_id        = 7,
        .rx_dma_stream_id        = 0,
        .tx_dma_channel          = DMA_Channel_1,
        .rx_dma_channel          = DMA_Channel_1,
#if defined (SPLIT_I2C_CONFIG)
        .gpio_af                 = ((GPIO_AF9_I2C2 << 8) | GPIO_AF_I2C2),
#else
        .gpio_af                 = GPIO_AF_I2C1
#endif
    },
};

const wiced_i2c_device_t auth_chip_i2c_device =
{
    .port          = WICED_I2C_1,
    .address       = 0x11,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_STANDARD_SPEED_MODE,
};

const platform_mfi_auth_chip_t platform_auth_chip =
{
    .i2c_device = &auth_chip_i2c_device,
    .reset_pin  = WICED_GPIO_18
};

/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
    [WICED_UART_1] =
    {
        .port               = USART1,
        .tx_pin             = &platform_gpio_pins[WICED_GPIO_6],
        .rx_pin             = &platform_gpio_pins[WICED_GPIO_7],
        .cts_pin            = &platform_gpio_pins[WICED_GPIO_8],
        .rts_pin            = &platform_gpio_pins[WICED_GPIO_9],
        .tx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream7,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA2_Stream7_IRQn,
            .complete_flags = DMA_HISR_TCIF7,
            .error_flags    = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 ),
        },
        .rx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream2,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA2_Stream2_IRQn,
            .complete_flags = DMA_LISR_TCIF2,
            .error_flags    = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
        },
    },
    [WICED_UART_2] =
    {
        .port               = USART6,
        .tx_pin             = &platform_gpio_pins[WICED_GPIO_31],
        .rx_pin             = &platform_gpio_pins[WICED_GPIO_32],
        .cts_pin            = NULL,
        .rts_pin            = NULL,
        .tx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream6,
            .channel        = DMA_Channel_5,
            .irq_vector     = DMA2_Stream6_IRQn,
            .complete_flags = DMA_HISR_TCIF6,
            .error_flags    = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
        },
        .rx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream1,
            .channel        = DMA_Channel_5,
            .irq_vector     = DMA2_Stream1_IRQn,
            .complete_flags = DMA_LISR_TCIF1,
            .error_flags    = ( DMA_LISR_TEIF1 | DMA_LISR_FEIF1 | DMA_LISR_DMEIF1 ),
        },
    },
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

/* SPI flash. Exposed to the applications through include/wiced_platform.h */
#if defined ( WICED_PLATFORM_INCLUDES_SPI_FLASH )
const wiced_spi_device_t wiced_spi_flash =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_SPI_FLASH_CS,
    .speed       = 25000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
    .bits        = 8
};
#endif

platform_spi_slave_driver_t platform_spi_slave_drivers[WICED_SPI_MAX];

/* UART standard I/O configuration */
#ifndef WICED_DISABLE_STDIO
static const platform_uart_config_t stdio_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};
#endif

/* Wi-Fi control pins. Used by WICED/platform/MCU/wwd_platform_common.c
 * SDIO: WWD_PIN_BOOTSTRAP[1:0] = b'00
 */
const platform_gpio_t wifi_control_pins[] =
{
    [WWD_PIN_POWER      ] = { GPIOC, 13 },
    [WWD_PIN_32K_CLK    ] = { GPIOA,  8 },
	/*  bootstrap pins are unused for Type1LD */
//    [WWD_PIN_BOOTSTRAP_0] = { GPIOA,  9 },
//    [WWD_PIN_BOOTSTRAP_1] = { GPIOA,  9 },
};

/* Wi-Fi SDIO bus pins. Used by WICED/platform/STM32F4xx/WWD/wwd_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
    [WWD_PIN_SDIO_OOB_IRQ] = { GPIOC,  1 },
    [WWD_PIN_SDIO_CLK    ] = { GPIOC, 12 },
    [WWD_PIN_SDIO_CMD    ] = { GPIOD,  2 },
    [WWD_PIN_SDIO_D0     ] = { GPIOC,  8 },
    [WWD_PIN_SDIO_D1     ] = { GPIOC,  9 },
    [WWD_PIN_SDIO_D2     ] = { GPIOC, 10 },
    [WWD_PIN_SDIO_D3     ] = { GPIOC, 11 },
};

/* Bluetooth control pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_gpio_t internal_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [WICED_BT_PIN_POWER      ] = { GPIOC,  2 },
    [WICED_BT_PIN_HOST_WAKE  ] = { GPIOC,  0 },
    [WICED_BT_PIN_DEVICE_WAKE] = { GPIOC,  3 },
};
const platform_gpio_t* wiced_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [WICED_BT_PIN_POWER      ] = &internal_bt_control_pins[WICED_BT_PIN_POWER      ],
    [WICED_BT_PIN_HOST_WAKE  ] = &internal_bt_control_pins[WICED_BT_PIN_HOST_WAKE  ],
    [WICED_BT_PIN_DEVICE_WAKE] = &internal_bt_control_pins[WICED_BT_PIN_DEVICE_WAKE],
    [WICED_BT_PIN_RESET      ] = NULL,
};

/* Bluetooth UART pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_gpio_t internal_bt_uart_pins[] =
{
    [WICED_BT_PIN_UART_TX ] = { GPIOA,  2 },
    [WICED_BT_PIN_UART_RX ] = { GPIOA,  3 },
    [WICED_BT_PIN_UART_CTS] = { GPIOA,  0 },
    [WICED_BT_PIN_UART_RTS] = { GPIOA,  1 },
};
const platform_gpio_t* wiced_bt_uart_pins[] =
{
    [WICED_BT_PIN_UART_TX ] = &internal_bt_uart_pins[WICED_BT_PIN_UART_TX ],
    [WICED_BT_PIN_UART_RX ] = &internal_bt_uart_pins[WICED_BT_PIN_UART_RX ],
    [WICED_BT_PIN_UART_CTS] = &internal_bt_uart_pins[WICED_BT_PIN_UART_CTS],
    [WICED_BT_PIN_UART_RTS] = &internal_bt_uart_pins[WICED_BT_PIN_UART_RTS],
};

/* Bluetooth UART peripheral and runtime driver. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_uart_t internal_bt_uart_peripheral =
{
    .port               = USART2,
    .tx_pin             = &internal_bt_uart_pins[WICED_BT_PIN_UART_TX ],
    .rx_pin             = &internal_bt_uart_pins[WICED_BT_PIN_UART_RX ],
    .cts_pin            = &internal_bt_uart_pins[WICED_BT_PIN_UART_CTS],
    .rts_pin            = &internal_bt_uart_pins[WICED_BT_PIN_UART_RTS],
    .tx_dma_config =
    {
        .controller     = DMA1,
        .stream         = DMA1_Stream6,
        .channel        = DMA_Channel_4,
        .irq_vector     = DMA1_Stream6_IRQn,
        .complete_flags = DMA_HISR_TCIF6,
        .error_flags    = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma_config =
    {
        .controller     = DMA1,
        .stream         = DMA1_Stream5,
        .channel        = DMA_Channel_4,
        .irq_vector     = DMA1_Stream5_IRQn,
        .complete_flags = DMA_HISR_TCIF5,
        .error_flags    = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
    },
};
static platform_uart_driver_t internal_bt_uart_driver;
const platform_uart_t*        wiced_bt_uart_peripheral = &internal_bt_uart_peripheral;
platform_uart_driver_t*       wiced_bt_uart_driver     = &internal_bt_uart_driver;

/* Bluetooth UART configuration. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_config_t wiced_bt_uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_CTS_RTS,
};

/*BT chip specific configuration information*/
const platform_bluetooth_config_t wiced_bt_config =
{
    .patchram_download_mode      = PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
    .patchram_download_baud_rate = 115200,
    .featured_baud_rate          = 115200
};

const gpio_button_t platform_gpio_buttons[PLATFORM_BUTTON_MAX] =
{
    [PLATFORM_BUTTON_1] =
    {
        .gpio       = WICED_BUTTON1,
        .config     = INPUT_PULL_UP,        /* no external pull ups on Cypress BCM9WCD9EVAL1 evaluation board */
        .polarity   = WICED_ACTIVE_LOW,
        .trigger    = IRQ_TRIGGER_BOTH_EDGES,
    },

    [PLATFORM_BUTTON_2] =
    {
        .gpio       = WICED_BUTTON2,
        .config     = INPUT_PULL_UP,        /* no external pull ups on Cypress BCM9WCD9EVAL1 evaluation board */
        .polarity   = WICED_ACTIVE_LOW,
        .trigger    = IRQ_TRIGGER_BOTH_EDGES,
    },
};

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
    /* Interrupt priority setup. Called by WICED/platform/MCU/STM32F4xx/platform_init.c */
    NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
    NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
    NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
    NVIC_SetPriority( USART1_IRQn      ,  6 ); /* WICED_UART_1        */
    NVIC_SetPriority( USART6_IRQn      ,  6 ); /* WICED_UART_2        */
    NVIC_SetPriority( USART2_IRQn      ,  6 ); /* BT_UART             */
    NVIC_SetPriority( DMA1_Stream6_IRQn,  7 ); /* BT_UART TX DMA      */
    NVIC_SetPriority( DMA1_Stream5_IRQn,  7 ); /* BT_UART RX DMA      */
    NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* WICED_UART_1 TX DMA */
    NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* WICED_UART_1 RX DMA */
    NVIC_SetPriority( DMA2_Stream6_IRQn,  7 ); /* WICED_UART_2 TX DMA */
    NVIC_SetPriority( DMA2_Stream1_IRQn,  7 ); /* WICED_UART_2 RX DMA */
    NVIC_SetPriority( EXTI0_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI1_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI2_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI3_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI4_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI9_5_IRQn     , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI15_10_IRQn   , 14 ); /* GPIO                */
}

platform_result_t platform_powersave_sleep_handler( void )
{
    return PLATFORM_SUCCESS;
}

void platform_init_external_devices( void )
{
    /* Initialize LEDs and turn off by default */
    platform_gpio_init( &platform_gpio_pins[WICED_LED1], OUTPUT_PUSH_PULL );
    platform_gpio_init( &platform_gpio_pins[WICED_LED2], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED1] );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED2] );

    /* Initialize buttons */
    platform_gpio_init( &platform_gpio_pins[ platform_gpio_buttons[PLATFORM_BUTTON_1].gpio ], platform_gpio_buttons[PLATFORM_BUTTON_1].config );
    platform_gpio_init( &platform_gpio_pins[ platform_gpio_buttons[PLATFORM_BUTTON_2].gpio ], platform_gpio_buttons[PLATFORM_BUTTON_2].config );

#ifndef WICED_DISABLE_STDIO
    /* Initialize UART standard I/O */
    platform_stdio_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_config );
#endif
}

uint32_t  platform_get_factory_reset_button_time ( uint32_t max_time )
{
    uint32_t button_press_timer = 0;
    int led_state = 0;

    /* Initialize input */
     platform_gpio_init( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_BUTTON_GPIO ], platform_gpio_buttons[ PLATFORM_FACTORY_RESET_BUTTON_GPIO ].config );

     while ( (PLATFORM_FACTORY_RESET_PRESSED_STATE == platform_gpio_input_get(&platform_gpio_pins[ PLATFORM_FACTORY_RESET_BUTTON_GPIO ])) )
    {
         /* How long is the "Factory Reset" button being pressed. */
         host_rtos_delay_milliseconds( PLATFORM_FACTORY_RESET_CHECK_PERIOD );

         /* Toggle LED every PLATFORM_FACTORY_RESET_CHECK_PERIOD  */
        if ( led_state == 0 )
        {
            platform_gpio_output_high( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
            led_state = 1;
        }
        else
        {
            platform_gpio_output_low( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
            led_state = 0;
        }

        button_press_timer += PLATFORM_FACTORY_RESET_CHECK_PERIOD;
        if ((max_time > 0) && (button_press_timer >= max_time))
        {
            break;
        }
    }

     /* turn off the LED */
     if (PLATFORM_FACTORY_RESET_LED_ON_STATE == WICED_ACTIVE_HIGH)
     {
         platform_gpio_output_low( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
     }
     else
     {
         platform_gpio_output_high( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
     }

    /* deinit the button */
    platform_gpio_deinit( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_BUTTON_GPIO ] );

    return button_press_timer;
}

platform_result_t platform_bluetooth_powersave_wakeup_handler( void )
{
    uint8_t state = 0;
    state = platform_gpio_input_get(wiced_bt_control_pins[WICED_BT_PIN_HOST_WAKE]);
    if ( state == 0 )
    {
        platform_gpio_irq_disable( wiced_bt_control_pins[WICED_BT_PIN_HOST_WAKE] );

        platform_mcu_powersave_disable();

        /* Toggle RTS to convey to the BT chip that the MCU is awake and ready to receive data */
        platform_gpio_output_low( wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS] );

        /* Set to Alternate function for the UART peripheral */
        platform_gpio_set_alternate_function(wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS]->port,
                                     wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS]->pin_number,
                                     GPIO_OType_PP, GPIO_PuPd_NOPULL,
                                     (wiced_bt_uart_peripheral->port == USART1) ? GPIO_AF_USART1:GPIO_AF_USART2);

        /* NOTE: See below note where we turn these LEDs off when MCU goes to sleep;
         * We may choose to turn the LEDs on again but that will just shoot average current consumption
         */
        //platform_gpio_output_high(&platform_gpio_pins[WICED_LED1]);
        //platform_gpio_output_high(&platform_gpio_pins[WICED_LED2]);

    }
    return PLATFORM_SUCCESS;
}

static void host_wake_irq_handler( void* arg )
{
}

platform_result_t platform_bluetooth_powersave_sleep_handler( void )
{
    platform_pin_config_t config = OUTPUT_PUSH_PULL;

    platform_gpio_set_alternate_function(
                wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS]->port,
                wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS]->pin_number,
                GPIO_OType_PP,
                GPIO_PuPd_NOPULL,
                0);

    platform_gpio_init(wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS], config);

    platform_gpio_output_high(wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS]);

    platform_gpio_irq_enable( wiced_bt_control_pins[ WICED_BT_PIN_HOST_WAKE ], IRQ_TRIGGER_FALLING_EDGE, host_wake_irq_handler, NULL );

    /* NOTE: these two LEDs consume around ~5-6 mA current. Keep them low for accurate current measurements */
    platform_gpio_output_low(&platform_gpio_pins[WICED_LED1]);
    platform_gpio_output_low(&platform_gpio_pins[WICED_LED2]);

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

static void platform_sleep_event_handler( wiced_sleep_event_type_t event, int handler )
{
    switch ( event )
    {
        case WICED_SLEEP_EVENT_ENTER:
            platform_bluetooth_powersave_sleep_handler();
            break;

        case WICED_SLEEP_EVENT_CANCEL:
            platform_bluetooth_powersave_wakeup_handler();
            break;

        case WICED_SLEEP_EVENT_LEAVE:
            platform_bluetooth_powersave_wakeup_handler();
            break;

        default:
            break;
    }
}

/* Call back , which is called before entering or after leaving sleep state */
WICED_SLEEP_EVENT_HANDLER( platform_sleep_event_handler_1 )
{
    platform_sleep_event_handler( event, 0 );
}

/******************************************************
 *           Interrupt Handler Definitions
 ******************************************************/

WWD_RTOS_DEFINE_ISR( usart1_irq )
{
    platform_uart_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart2_irq )
{
    platform_uart_irq( wiced_bt_uart_driver );
}

WWD_RTOS_DEFINE_ISR( usart6_irq )
{
    platform_uart_irq( &platform_uart_drivers[WICED_UART_2] );
}

WWD_RTOS_DEFINE_ISR( usart1_tx_dma_irq )
{
    platform_uart_tx_dma_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart2_tx_dma_irq )
{
    platform_uart_tx_dma_irq( wiced_bt_uart_driver );
}

WWD_RTOS_DEFINE_ISR( usart6_tx_dma_irq )
{
	platform_uart_tx_dma_irq( &platform_uart_drivers[WICED_UART_2] );
}

WWD_RTOS_DEFINE_ISR( usart1_rx_dma_irq )
{
    platform_uart_rx_dma_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart2_rx_dma_irq )
{
    platform_uart_rx_dma_irq( wiced_bt_uart_driver );
}

WWD_RTOS_DEFINE_ISR( usart6_rx_dma_irq )
{
    platform_uart_rx_dma_irq( &platform_uart_drivers[WICED_UART_2] );
}

/******************************************************
 *            Interrupt Handlers Mapping
 ******************************************************/

/* These DMA assignments can be found STM32F4xx datasheet DMA section */
WWD_RTOS_MAP_ISR( usart1_irq       , USART1_irq       )
WWD_RTOS_MAP_ISR( usart2_irq       , USART2_irq       )
WWD_RTOS_MAP_ISR( usart6_irq       , USART6_irq       )
WWD_RTOS_MAP_ISR( usart1_tx_dma_irq, DMA2_Stream7_irq )
WWD_RTOS_MAP_ISR( usart1_rx_dma_irq, DMA2_Stream2_irq )
WWD_RTOS_MAP_ISR( usart2_tx_dma_irq, DMA1_Stream6_irq )
WWD_RTOS_MAP_ISR( usart2_rx_dma_irq, DMA1_Stream5_irq )
WWD_RTOS_MAP_ISR( usart6_tx_dma_irq, DMA2_Stream6_irq )
WWD_RTOS_MAP_ISR( usart6_rx_dma_irq, DMA2_Stream1_irq )
