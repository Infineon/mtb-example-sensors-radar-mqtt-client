/******************************************************************************
 * File Name:   radar_task.h
 *
 * Description: This file contains the function prototypes and constants used
 *   in radar_task.c.
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

#pragma once

/* Header file includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Header file for library */
#include "mtb_radar_sensing.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
#define RADAR_TASK_NAME       "RADAR PRESENCE TASK"
#define RADAR_TASK_STACK_SIZE (1024 * 4)
#define RADAR_TASK_PRIORITY   (3)

/**
 * Compile time switch to determine which function mode the radar module is
 * working on. By default undefine the following macro, radar module works in
 * 'PresenceDetection' mode. Define the following macro, it works in
 * 'EntranceCounter' mode.
 */
#undef RADAR_ENTRANCE_COUNTER_MODE

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
extern TaskHandle_t radar_task_handle;

extern SemaphoreHandle_t sem_radar_sensing_context;
extern mtb_radar_sensing_context_t radar_sensing_context;

extern int32_t entrance_count_in;
extern int32_t entrance_count_out;
extern cyhal_timer_t led_blink_timer;

/*******************************************************************************
 * Functions
 ******************************************************************************/
void radar_task(void *pvParameters);
void radar_task_cleanup(void);

/* [] END OF FILE */
