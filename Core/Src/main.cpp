#include "main.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "string.h"
#include "task.h"

#define IDLE_BUF_SIZE   30

typedef struct TaskData_t {
    uint8_t *str;
    uint32_t period;

} TaskData;

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

uint8_t task1_text[] = "Hello from First Task \r\n";
uint8_t task2_text[] = "*******Hello from Second Task*******\r\n";
volatile uint32_t idle_count = 0;


void MyTask(void *pvParameters) {
    uint16_t size = strlen((const char *)pvParameters);
    uint8_t buf[IDLE_BUF_SIZE] = {0};
    while (1) {
        for (int i = 0; i < size; ++i) {
            HAL_UART_Transmit(&huart2, &((uint8_t *)pvParameters)[i], 1, 100);
        }
        snprintf((char*)buf,IDLE_BUF_SIZE,"idle count : %ld \r\n",idle_count);
        int cnt_size = strlen((char*)buf);
        for (int i = 0; i < cnt_size; ++i) {
            HAL_UART_Transmit(&huart2, &buf[i], 1, 100);
        }
        vTaskDelay(pdMS_TO_TICKS(1200));
    }
    vTaskDelete(NULL);
}

void CommonTask(void *pvParameters) {
    volatile TaskData* params = (TaskData*)pvParameters;
    uint16_t size = strlen((const char*)params->str);
    while (1) {
        for (int i=0; i < size; ++i) {
            HAL_UART_Transmit(&huart2,(uint8_t*)&params->str[i], 1, 100);
        }
        vTaskDelay(pdMS_TO_TICKS(params->period));
    }
    vTaskDelete(NULL);
}

void LedTask(void *pvParameters) {
    portTickType LastWakeTime;
    LastWakeTime = xTaskGetTickCount();
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
        vTaskDelayUntil( &LastWakeTime, pdMS_TO_TICKS(1000) );
    }
    vTaskDelete(NULL);
}

void LedTask1(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
    }
    vTaskDelete(NULL);
}

TaskData data1 ,data2;

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    data1 = {
        .str = task1_text,
        .period = 1000
    };


    auto res = xTaskCreate(CommonTask,"Common Task1",256, (void*)&data1,configMAX_PRIORITIES,NULL);
    if (res != pdTRUE) {
        while (1) {
            // HAL_Delay(500);
            // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
        }
    }

    res = xTaskCreate(MyTask, "Second Task", 256, task2_text,configMAX_PRIORITIES, NULL);
    if (res != pdTRUE) {
        while (1) {
            // HAL_Delay(500);
            // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
        }
    }

    res = xTaskCreate(LedTask, "LED Task", 128,NULL,configMAX_PRIORITIES, NULL);
    if (res != pdTRUE) {
        while (1) {

        }
    }

    res = xTaskCreate(LedTask1, "LED Task1", 128,NULL,configMAX_PRIORITIES, NULL);
    if (res != pdTRUE) {
        while (1) {

        }
    }

    vTaskStartScheduler();

    while (1) {
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
    }
}

void SystemClock_Config(void)
{
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
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

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 , GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

    /*Configure GPIO pins : PC0 PC1 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
