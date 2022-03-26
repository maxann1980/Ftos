/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"

#include "main.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

extern uint32_t idle_count;
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}

// this function will be called in case task stack overflow
// configCHECK_FOR_STACK_OVERFLOW has to be 1
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // TODO process error state
}

// this function will be called in case idle task has running state
// configUSE_IDLE_HOOK has to be 1
void vApplicationIdleHook(void) {
    // TODO some actions in idle state
    ++idle_count;
    vCoRoutineSchedule();
}

// this function will be called every time quant of OS
// configUSE_TICK_HOOK has to be 1
void vApplicationTickHook(void) {

}
