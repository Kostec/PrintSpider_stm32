set(TEST_NAME Print_test)

add_compile_definitions(STM32F411xE)

file(GLOB TARGET_INCLUDE_DIRECTORY 
    ${MOCKED_HEADERS_GLOBAL}
    ${TEST_TARGET_DIRECTORY}/Core/Inc
    ${TEST_TARGET_DIRECTORY}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${TEST_TARGET_DIRECTORY}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

set(TARGET_SOURCES
    ${TEST_TARGET_DIRECTORY}/Core/Src/Print/print.c
    ${TEST_TARGET_DIRECTORY}/Core/Src/Print/printspider.c
    ${GLOBAL_MOCK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
)

set(TEST_SOURCES
    Print_tests.cpp 
    ${TEST_MAIN}
)

set(TEST_MOCKS
    
)

# message(Print_test)
# message(TARGET_INCLUDE_DIRECTORY=${TARGET_INCLUDE_DIRECTORY})
# message(TARGET_SOURCES=${TARGET_SOURCES})
# message(TEST_SOURCES=${TEST_SOURCES})
# message(TEST_MOCKS=${TEST_MOCKS})
# message(===================RUN GENERATOR===================)

add_test_runner(Print_test ${TARGET_INCLUDE_DIRECTORY} ${TARGET_SOURCES} ${TEST_SOURCES} ${TEST_MOCKS})