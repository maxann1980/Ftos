#include "main.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"
#include "task.h"
#include "semphr.h"

#define BUF_SIZE 200
#define LOOP_DELAY 1000000
#define QUEUE_SIZE 10

uint32_t idle_count;
UART_HandleTypeDef huart2;

uint8_t task1_text[] ="\
*******************************************************************************\r\n\
********************************* TASK 1 RUNNING ******************************\r\n\
*******************************************************************************\r\n\r\n";


uint8_t task2_text[] ="\
###############################################################################\r\n\
################################# TASK 2 RUNNING ##############################\r\n\
###############################################################################\r\n\r\n";

xSemaphoreHandle BinSem1 = NULL;
xSemaphoreHandle BinSem2 = NULL;

xSemaphoreHandle mutex = NULL;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

xTaskHandle TaskHandle1;
xTaskHandle TaskHandle2;

void Task1(void *pvParameters) {
    while (1) {
        xSemaphoreTake( mutex, portMAX_DELAY );
        for (uint32_t i = 0; i < sizeof(task1_text); ++i) {
            while( (huart2.Instance->SR & UART_FLAG_TXE) != UART_FLAG_TXE) { }
            huart2.Instance->DR = task1_text[i] & 0xFFU;
        }
        xSemaphoreGive( mutex); 
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

void Task2(void *pvParameters) {
    while (1) {
        xSemaphoreTake( mutex, portMAX_DELAY );
        for (uint32_t i = 0; i < sizeof(task2_text); ++i) {
            while( (huart2.Instance->SR & UART_FLAG_TXE) != UART_FLAG_TXE) { }
            huart2.Instance->DR = task2_text[i] & 0xFFU;
        }
        xSemaphoreGive( mutex);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL)
        Error_Handler();


    auto res = xTaskCreate(Task1, "Task1", 256, NULL, MID_PRIORITY, NULL);
    if (res != pdTRUE)
        Error_Handler();

    res = xTaskCreate(Task2, "Task2", 256, NULL, MID_PRIORITY, NULL);
    if (res != pdTRUE)
        Error_Handler();

    vTaskStartScheduler();

    while (1) {
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }

    __HAL_RCC_PLLI2S_ENABLE();
}

static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

    /*Configure GPIO pins : PC0 PC1 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : btn2_Pin btn1_Pin */
    GPIO_InitStruct.Pin = btn2_Pin | btn1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
