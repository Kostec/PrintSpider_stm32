#############################################################################################################################
# file:  CMakeLists.txt
# brief: Template "CMakeLists.txt" for building of executables and static libraries.
#
# usage: Edit "VARIABLES"-section to suit project requirements.
#        For debug build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Debug -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
#          make -C Debug VERBOSE=1 -j
#        For release build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Release -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#          make -C Release VERBOSE=1 -j
#############################################################################################################################
cmake_minimum_required(VERSION 3.20)

###################### VARIABLES ######################################
set (PROJECT_NAME             "PrintSpider_stm32")
set (PROJECT_TYPE             "exe")
set (LINKER_SCRIPT            "${CMAKE_CURRENT_LIST_DIR}/STM32F411RETX_FLASH.ld")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CORE_SOURCES
    # CORE FEATURES
    Core/Src/Print/printspider.c
    Core/Src/Print/print.c
    Core/Src/LED/ws2812.c
)

file(GLOB_RECURSE SOURCES
    Drivers/STM32F4xx_HAL_Driver/Src/*.*
    Core/Src/*.*
    FATFS/*.*
)
set (PROJECT_SOURCES
    # LIST OF ALL PROJECT FILE SOURCES
	Core/Startup/startup_stm32f411retx.s
	${SOURCES}
	)

set (PROJECT_DEFINES
	# LIST COMPILER DEFINITIONS HERE
    STM32F411xE
    )

set (CORE_INCLUDES 
    Core/Inc Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

set (PROJECT_INCLUDES
	# LIST INCLUDE DIRECTORIES HERE
    Core/Inc
    Core/Inc/SD
    Core/Inc/LED
    Core/Inc/LOG
    Core/Inc/OLED
    Core/Inc/OLED/MENU
    Core/Inc/Print
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include

    FATFS/App
    FATFS/Target
)

file(GLOB_RECURSE SOURCES
    Drivers/STM32F4xx_HAL_Driver/Src/*.*
    Core/Src/*.*
)
# ==============================MIDDLEWARE BEGIN==============================
set (FREE_RTOS_INCLUDES
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)

file(GLOB_RECURSE FREE_RTOS_SOURCES
    Middlewares/Third_Party/FreeRTOS/*.*
)

set (FatFs_MIDDLEWARE_INCLUDES
    Middlewares/Third_Party/FatFs/src
)

file(GLOB_RECURSE FatFs_MIDDLEWARE_SOURCES
    Middlewares/Third_Party/FatFs/src/*.*
)

set (USB_DEVICE_MIDDLEWARE_INCLUDES
    Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc
)

file(GLOB_RECURSE USB_DEVICE_MIDDLEWARE_SOURCES
    Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/*.*
    Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.*

)

set(MIDDLEWARE_INCLUDES
    ${FREE_RTOS_INCLUDES}
    ${FatFs_MIDDLEWARE_INCLUDES}
    ${USB_DEVICE_MIDDLEWARE_INCLUDES}
)

set(MIDDLEWARE_SOURCES
    ${FREE_RTOS_SOURCES}
    ${FatFs_MIDDLEWARE_SOURCES}
    ${USB_DEVICE_MIDDLEWARE_SOURCES}
)
# ==============================MIDDLEWARE END==============================

# ==============================OLED BEGIN==============================
set(OLED_INCLUDES ssd1306)

file(GLOB_RECURSE OLED_SOURCES ssd1306/*.*)
# ==============================OLED END==============================

# ==============================USB BEGIN==============================
set(USB_INCLUDES
    USB_DEVICE/App
    USB_DEVICE/Target
)

file(GLOB_RECURSE USB_SOURCES
    USB_DEVICE/App/*.*
    USB_DEVICE/Target/*.*
)
# ==============================USB END==============================


set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=softfp
)
################## PROJECT SETUP ######################################
project(${PROJECT_NAME} C CXX ASM)
enable_language(ASM)
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
# Tests definition
message(STATUS BUILD_TESTS=${BUILD_TESTS} CMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING})
if(BUILD_TESTS)
    message(STATUS "Configure Tests")
    add_compile_definitions(${PROJECT_DEFINES})    
    LIST(REMOVE_ITEM ${SOURCES} D:/Projects/STM32CubeIDE/PrintSpider_stm32/Core/Src/syscalls.c)
    add_library(core_lib STATIC ${SOURCES})
    target_include_directories(core_lib PUBLIC 
        ${PROJECT_INCLUDES}
        ${MIDDLEWARE_INCLUDES}
    )

    message(STATUS "add_subdirectory(tests)")
    add_subdirectory(tests)
else()
    message(STATUS "Configure Firmware")
    add_compile_options(${MCU_FLAGS})
    add_compile_definitions(ENABLE_SYSCALLS)
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${GLOB_RECURSE}
        ${MIDDLEWARE_SOURCES}
        ${OLED_SOURCES}
        ${USB_SOURCES}
    )

    add_custom_command(TARGET ${PROJECT_NAME}   POST_BUILD
                    COMMAND arm-none-eabi-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}>
    )

    add_compile_definitions(${PROJECT_DEFINES})
    include_directories (
        ${PROJECT_INCLUDES}
        ${MIDDLEWARE_INCLUDES}
        ${OLED_INCLUDES}
        ${USB_INCLUDES}
    )

    set (CMAKE_EXECUTABLE_SUFFIX ".elf")
    set (CMAKE_STATIC_LIBRARY_SUFFIX ".a")
    set (CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${RUNTIME_LIBRARY_SYSCALLS} -Wl,-Map=PrintSpider_stm32.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group")

    set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
        Building ${BIN_FILE}"
    )
endif()
