#Имя системы под которую осуществляется сборка и ее версия
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

#Минимальная вресия CMake необходимая для компиляции проекта
cmake_minimum_required(VERSION 3.16)

#Ниже указаны имена компиляторов и утилит тулчейна
#CXX - компиялтор C++
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

#Из-за того, что мы собираем проект под микроконтроллер, а не под хост
#нужно сообщить об этом CMAKE
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#Имя проекта и используемые языки
#project(test C CXX ASM)
project(application C CXX ASM)

#Имя ядра микроконтроллера
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
#Расширение скомпилированного файла
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

#Стандарты C++ и C максимально поддерживаемые в этом проекте
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Имя файла линкера под ваш МК
set(LINKER_SCRIPT_NAME STM32F105RCTx_FLASH)
#А так же путь до него. В моем случе скрипт лежит в папке startup которая находится в корне проекта
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT_NAME}.ld)

# Далее идут флаги компиляции для каждого компилятора (Си, Си++ и Ассемблер)
# Так как для своих нужд я переносил проект с STM32CubeIDE, все флаги перешли оттуда же
# Опции компиляции можно посмотреть на офф сайте https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
# Рассмотрим некоторые флаги:
# -mcpu - ядро МК
# -g - степень оптимизации
# -std - стандарт языка, gcc для C, g++ (или c++) для C++
# -O - степень (или тип) оптимизации. -O0 - без оптимзации,
# -O1, -O2, -O3 оптимизации от наименьшей к наибольшей, чем выше цифра, тем сильнее оптимизация
# стоит учитывать что чем выше степень оптимизации, тем сложнее работать в режиме дебага
# -Os - оптимизация по размеру, пытается скомпилировать минимальный размер
# -Ofast - оптимизация по скорости испольнения кода
# -Og - оптимизация для дебага
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=${CMAKE_SYSTEM_PROCESSOR} -g3 -std=gnu++${CMAKE_CXX_STANDARD} -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-exceptions -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -std=c++17 -fstack-usage -mthumb -fno-exceptions -fno-rtti ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${CMAKE_SYSTEM_PROCESSOR} -std=gnu${CMAKE_C_STANDARD} -g3 -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -Wall -fstack-usage ")
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR} -g3 -T ${LINKER_SCRIPT} -Wl,-Map=${PROJECT_NAME}.map  -Wl,--gc-sections -static -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group -Wl,--print-memory-usage ")
set(CMAKE_ASM_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR} -g3 -c -x assembler-with-cpp -mthumb")

#Говорим применить такой то компилятор с таким-то линковщиком и т.д.
#set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <LINK_FLAGS> -o <TARGET> <OBJECTS>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <LINK_FLAGS> -o <TARGET> <OBJECTS>")

#Имя и путь к стартап файлу
set(STARTUP_FILE_NAME startup_stm32f105xc)
set(STARTUP_LOCATION "${CMAKE_SOURCE_DIR}/${STARTUP_FILE_NAME}.s")

#Пути по которым лежат инклуды, '.' означает корень проекта
include_directories(.)
include_directories(cmsis)

#Глобальный дефайн, нужен для CMSIS
add_definitions(-DSTM32F105xC)

#имена .cpp, .c и .s файлов
set(SOURCE_FILES 
    Core/Src/main.cpp
    Core/Src/freertos.c
    Core/Src/syscalls.c
    Core/Src/stm32f1xx_it.c
    Core/Src/stm32f1xx_hal_msp.c
    Core/Src/stm32f1xx_hal_timebase_tim.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    Core/Src/system_stm32f1xx.c  

#   FREE RTOS
    FreeRTOS/src/croutine.c
    FreeRTOS/src/event_groups.c
    FreeRTOS/src/list.c
    FreeRTOS/src/queue.c
    FreeRTOS/src/stream_buffer.c
    FreeRTOS/src/tasks.c
    FreeRTOS/src/timers.c
    FreeRTOS/src/heap_4.c
    FreeRTOS/src/port.c
)
#имена хедеров
include_directories(
    Core/Inc
    Drivers/STM32F1xx_HAL_Driver/Inc
    Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
    Drivers/CMSIS/Include
    FreeRTOS/inc
    FreeRTOS
)


#На этом моменте происходит компиляция и линковка проекта в .elf
add_executable(${PROJECT_NAME} ${STARTUP_LOCATION} ${INCLUDE_FILES} ${SOURCE_FILES})

#Превращаем .elf в .hex
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex)
#Превращаем .elf в .bin
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)
#Вывод в консоль данных о размере секций .bss, .data и т.д., а так же всего проекта
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${SIZE} ARGS --format=berkeley ${PROJECT_NAME}.elf)