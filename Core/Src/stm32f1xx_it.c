/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32f1xx_it.h"

#include "FreeRTOS.h"
#include "main.h"
#include "semphr.h"
#include "task.h"
#include "croutine.h"

extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart2;
extern xQueueHandle xQueue1;
extern xQueueHandle xQueue2;


void NMI_Handler(void) {
    while (1) {
    }
}


void HardFault_Handler(void) {
    while (1) {
    }
}


void MemManage_Handler(void) {

    while (1) {
    }
}

void BusFault_Handler(void) {
    while (1) {
    }
}

void UsageFault_Handler(void) {
    while (1) {
    }
}


void DebugMon_Handler(void) {
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }
}

void TIM1_UP_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim1);
}

void USART2_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart2);
}

void EXTI15_10_IRQHandler(void) {
    static portBASE_TYPE pxCoRoutineWoken ;
    if (__HAL_GPIO_EXTI_GET_IT(btn1_Pin) != 0x00u) {
        static uint16_t val1 = 0;
        crQUEUE_SEND_FROM_ISR(xQueue1,&val1,pxCoRoutineWoken);
        ++val1;
        __HAL_GPIO_EXTI_CLEAR_IT(btn1_Pin);
    } else if (__HAL_GPIO_EXTI_GET_IT(btn2_Pin) != 0x00u) {
        static uint16_t val2 = 0;
        crQUEUE_SEND_FROM_ISR(xQueue2,&val2,pxCoRoutineWoken);
        ++val2;
        __HAL_GPIO_EXTI_CLEAR_IT(btn2_Pin);
    }
}