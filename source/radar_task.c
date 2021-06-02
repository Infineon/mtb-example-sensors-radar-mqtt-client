/*****************************************************************************
 * File name: radar_task.c
 *
 * Description: This file uses RadarSensing library APIs to demonstrate
 * presence detection use case of radar.
 *
 * Related Document: See README.md
 *
 * ===========================================================================
 * Copyright (C) 2021 Infineon Technologies AG. All rights reserved.
 * ===========================================================================
 *
 * ===========================================================================
 * Infineon Technologies AG (INFINEON) is supplying this file for use
 * exclusively with Infineon's sensor products. This file can be freely
 * distributed within development tools and software supporting such
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON
 * WHATSOEVER.
 * ===========================================================================
 */

/* Header file from system */
#include <inttypes.h>

/* Header file includes */
#include "cy_retarget_io.h"
#include "cycfg.h"
#include "cyhal.h"

/* Header file for local task */
#include "publisher_task.h"
#include "radar_config_task.h"
#include "radar_led_task.h"
#include "radar_task.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
/* Pin number designated for LED RED */
#define LED_RGB_RED (CYBSP_GPIOA0)
/* Pin number designated for LED GREEN */
#define LED_RGB_GREEN (CYBSP_GPIOA1)
/* Pin number designated for LED BLUE */
#define LED_RGB_BLUE (CYBSP_GPIOA2)
/* LED off */
#define LED_STATE_OFF (0U)
/* LED on */
#define LED_STATE_ON (1U)
/* RADAR sensor SPI frequency */
#define SPI_FREQUENCY (25000000UL)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
TaskHandle_t radar_task_handle = NULL;

/* Semaphore to protect radar sensing context */
SemaphoreHandle_t sem_radar_sensing_context = NULL;
/* Radar sensing context */
mtb_radar_sensing_context_t radar_sensing_context = {0};

/* Entrance counter In & Out number. Can be reset from remote server. */
int32_t entrance_count_in = 0;
int32_t entrance_count_out = 0;

/*******************************************************************************
 * Local Variables
 ******************************************************************************/
static char local_pub_msg[MQTT_PUB_MSG_MAX_SIZE] = {0};
static int32_t occupy_status = 0;

/*******************************************************************************
 * Function Name: radar_sensing_callback
 *******************************************************************************
 * Summary:
 *   Callback function that handles presence detection or entrance counter events
 *
 * Parameters:
 *   context: context object of RadarSensing
 *   event: types of events that are detected
 *   event_info: description of the event
 *   data:
 *
 * Return:
 *   none
 ******************************************************************************/
static void radar_sensing_callback(mtb_radar_sensing_context_t *context,
                                   mtb_radar_sensing_event_t event,
                                   mtb_radar_sensing_event_info_t *event_info,
                                   void *data)
{
    (void)context;
    (void)data;

    radar_led_set_pattern(event);

    switch (event)
    {
#ifdef RADAR_ENTRANCE_COUNTER_MODE
        /* The following cases happen when radar working in EntranceCounter mode */
        // people walking in detected
        case MTB_RADAR_SENSING_EVENT_COUNTER_IN:
            ++entrance_count_in;
            break;
        // people walking out detected
        case MTB_RADAR_SENSING_EVENT_COUNTER_OUT:
            ++entrance_count_out;
            break;
        // object detected in traffic zone, reminder for social distancing
        case MTB_RADAR_SENSING_EVENT_COUNTER_OCCUPIED:
            occupy_status = 1;
            break;
        // no more object detected in traffic zone
        case MTB_RADAR_SENSING_EVENT_COUNTER_FREE:
            occupy_status = 0;
            break;
#else
        /* The following cases happen when radar working in Presence mode */
        case MTB_RADAR_SENSING_EVENT_PRESENCE_IN:
            occupy_status = 1;
            break;
        case MTB_RADAR_SENSING_EVENT_PRESENCE_OUT:
            occupy_status = 0;
            break;

#endif
        default:
            printf("Unknown event. Error!\n");
            snprintf(local_pub_msg, sizeof(local_pub_msg), "{\"Radar Module\": \"Unknown event. Error!\"}");
            return;
    }

#ifdef RADAR_ENTRANCE_COUNTER_MODE
    printf("%.2f: Counter free detected, IN: %ld, OUT: %ld, occupy_status: %ld\r\n",
           (float)event_info->timestamp / 1000,
           entrance_count_in,
           entrance_count_out,
           occupy_status);

    snprintf(local_pub_msg,
             sizeof(local_pub_msg),
             "{\"IN_Count\":%ld, \"OUT_Count\":%ld, \"Status\":%ld}",
             entrance_count_in,
             entrance_count_out,
             occupy_status);
#else
    if (occupy_status)
    {
        printf("%.3f: Presence IN %.2f-%.2f\n",
               (float)event_info->timestamp / 1000,
               ((mtb_radar_sensing_presence_event_info_t *)event_info)->distance -
                   ((mtb_radar_sensing_presence_event_info_t *)event_info)->accuracy,
               ((mtb_radar_sensing_presence_event_info_t *)event_info)->distance +
                   ((mtb_radar_sensing_presence_event_info_t *)event_info)->accuracy);

        snprintf(local_pub_msg, sizeof(local_pub_msg), "{\"PRESENCE\": \" IN\"}");
    }
    else
    {
        printf("%.3f: Presence OUT\n", (float)event_info->timestamp / 1000);

        snprintf(local_pub_msg, sizeof(local_pub_msg), "{\"PRESENCE\": \"OUT\"}");
    }
#endif

    /* Send message back to publish queue. If queue is full, 'local_pub_msg' will be dropped. So no result checking. */
    xQueueSendToBack(mqtt_pub_q, local_pub_msg, 0);
}

/*******************************************************************************
 * Function Name: ifx_currenttime
 *******************************************************************************
 * Summary:
 *   Obtains system time in ms
 *
 * Parameters:
 *   none
 *
 * Return:
 *   system time in ms
 ******************************************************************************/
static uint64_t ifx_currenttime()
{
    return (uint64_t)xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/*******************************************************************************
 * Function Name: radar_task
 *******************************************************************************
 * Summary:
 *   Initializes GPIO ports, context object of RadarSensing for presence
 *   detection or entrance counter, then initializes radar device configuration,
 *   sets parameters for presence detection or entrance counter, registers
 *   callback to handle presence detection or entrance counter events and
 *   continuously processes data acquired from radar.
 *
 * Parameters:
 *   pvParameters: thread
 *
 * Return:
 *   none
 ******************************************************************************/
void radar_task(void *pvParameters)
{
    (void)pvParameters;

    cyhal_spi_t mSPI;

    mtb_radar_sensing_hw_cfg_t hw_cfg = {.spi_cs = CYBSP_SPI_CS,
                                         .reset = CYBSP_GPIO11,
                                         .ldo_en = CYBSP_GPIO5,
                                         .irq = CYBSP_GPIO10,
                                         .spi = &mSPI};

    /* Activate radar reset pin */
    cyhal_gpio_init(hw_cfg.reset, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);

    /* Enable LDO */
    cyhal_gpio_init(hw_cfg.ldo_en, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);

    /* Enable IRQ pin */
    cyhal_gpio_init(hw_cfg.irq, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLDOWN, false);

    /* CS handled manually */
    cyhal_gpio_init(hw_cfg.spi_cs, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);

    /* Configure SPI interface */
    if (cyhal_spi_init(hw_cfg.spi,
                       CYBSP_SPI_MOSI,
                       CYBSP_SPI_MISO,
                       CYBSP_SPI_CLK,
                       NC,
                       NULL,
                       8,
                       CYHAL_SPI_MODE_00_MSB,
                       false) != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Set the data rate to 25 Mbps */
    if (cyhal_spi_set_frequency(hw_cfg.spi, SPI_FREQUENCY) != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

#ifdef RADAR_ENTRANCE_COUNTER_MODE
    /* Initialize RadarSensing context object for presence detection, */
    /* also initialize radar device configuration */
    if (mtb_radar_sensing_init(&radar_sensing_context, &hw_cfg, MTB_RADAR_SENSING_MASK_COUNTER_EVENTS) !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        printf("**** ifx_radar_sensing_init error - Radar Wingboard not connected? ****\n\n\n");
        vTaskSuspend(NULL);
    }

    /* Register callback to handle counter events */
    if (mtb_radar_sensing_register_callback(&radar_sensing_context, radar_sensing_callback, NULL) !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Set parameters for entrance counter  Here list all parameters with default value as example. */
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_installation", "side") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_orientation", "portrait") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_ceiling_height", "2.5") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_entrance_width", "1.0") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_sensitivity", "0.5") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_traffic_light_zone", "1.0") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_reverse", "false") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_counter_min_person_height", "1.0") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
#else
    /* Initialize RadarSensing context object for presence detection, */
    /* also initialize radar device configuration */
    if (mtb_radar_sensing_init(&radar_sensing_context, &hw_cfg, MTB_RADAR_SENSING_MASK_PRESENCE_EVENTS) !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        printf("**** ifx_radar_sensing_init error - Radar Wingboard not connected? ****\n\n\n");
        vTaskSuspend(NULL);
    }

    /* Register callback to handle presence detection events  Here list all parameters with default value as example. */
    if (mtb_radar_sensing_register_callback(&radar_sensing_context, radar_sensing_callback, NULL) !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Set parameter for presence detection */
    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_presence_range_max", "2.0") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }

    if (mtb_radar_sensing_set_parameter(&radar_sensing_context, "radar_presence_sensitivity", "medium") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
#endif

    /* Enable context object */
    if (mtb_radar_sensing_enable(&radar_sensing_context) != MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initiate semaphore mutex to protect 'radar_sensing_context' */
    sem_radar_sensing_context = xSemaphoreCreateMutex();
    if (sem_radar_sensing_context == NULL)
    {
        printf(" 'sem_radar_sensing_context' semaphore creation failed... Task suspend\n\n");
        vTaskSuspend(NULL);
    }

    /**
     * Create task for radar configuration. Configuration parameters come from
     * Subscriber task. Subscribed topics are configured inside 'mqtt_client_config.c'.
     */
    if (pdPASS != xTaskCreate(radar_config_task,
                              RADAR_CONFIG_TASK_NAME,
                              RADAR_CONFIG_TASK_STACK_SIZE,
                              NULL,
                              RADAR_CONFIG_TASK_PRIORITY,
                              &radar_config_task_handle))
    {
        printf("Failed to create Radar config task!\n");
        CY_ASSERT(0);
    }

    /**
     * Create task for led control. Based on different radar sensing event, led will display
     * in different mode. Refer to 'radar_led_task.c/.h' for more info.
     */
    if (pdPASS != xTaskCreate(radar_led_task,
                              RADAR_LED_TASK_NAME,
                              RADAR_LED_TASK_STACK_SIZE,
                              NULL,
                              RADAR_LED_TASK_PRIORITY,
                              &radar_led_task_handle))
    {
        printf("Failed to create Radar led task!\n");
        CY_ASSERT(0);
    }

    for (;;)
    {
        if (xSemaphoreTake(sem_radar_sensing_context, portMAX_DELAY) == pdTRUE)
        {
            /* Process data acquired from radar every 2ms */
            if (mtb_radar_sensing_process(&radar_sensing_context, ifx_currenttime()) != MTB_RADAR_SENSING_SUCCESS)
            {
                printf("ifx_radar_sensing_process error\n");
                CY_ASSERT(0);
            }
            xSemaphoreGive(sem_radar_sensing_context);
            vTaskDelay(MTB_RADAR_SENSING_PROCESS_DELAY);
        }
    }
}

/*******************************************************************************
 * Function Name: radar_task_cleanup
 *******************************************************************************
 * Summary:
 *   Cleanup all resources radar_task has used/created.
 *
 * Parameters:
 *   void
 *
 * Return:
 *   void
 ******************************************************************************/
void radar_task_cleanup(void)
{
    if (radar_config_task_handle != NULL)
    {
        vTaskDelete(radar_config_task_handle);
    }
    if (radar_led_task_handle != NULL)
    {
        vTaskDelete(radar_led_task_handle);
    }
}

/* [] END OF FILE */
