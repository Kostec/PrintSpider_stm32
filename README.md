## Summary
This project is attempts to control printer cartridge H123 with STM32F411RE MCU.

## Based On
- [PrintSpider](https://github.com/lichnost/PrintSpider_Arduino) - PrintSpider implementation for Arduino.
- [SMT32F411RE template](https://github.com/Kostec/test_411RE) with support CubeIDE and VS Code Cmake development.

## Required Tools
To initialize the project, you I have used the following tools:
1. Cmake (3.20)
2. Cube IDE (1.19)
3. OpenOCD (0.12 from Cube IDE if you want to debug)
4. GNU ARM Toolchain (14.3)
5. VS Code (1.99.3)
6. Plink (0.81)
7. LLVM (19.0)

## How To Debug and Develop
There are two ways to Debug this project: with CubeIDE or with VSCode.
### With CubeIDE
1. Open the project in CubeIDE
2. Download the package for STM32F411RE
3. Build the project
4. Flash/Debug the firmware on your board (STM32F411RE)

### With VSCode
1. Make sure that your have installed recommended extensions for this project
2. Open the project in VS Code
3. Set correct paths to .vscode/settings.json
4. Choose correct preset in CMake extension tab
5. Build or Debug the project

**Note**: 
    - In case of start debug (F5) SWO task will starts with error - just continue debugging.
    - Debugging with SWD interface takes CPU time. To avoid this, it can be reworked to use UART via DMA

### With command line
To build the project, call the following commands::
1. cmake --preset PrintSpider_stm32
2. cmake --build --preset build-fw



## How To write tests for this project

### How To add new tests
1. Add stubs for driver dependencies to `test/mocks` folder.
2. Define the module to Core/{module name}.
3. Add CMakeLists.txt to this folder.
4. Use the next macroses and call `add_test_runner` function:
    - TARGET_INCLUDE_DIRECTORY for all test include directories,
    - TARGET_SOURCES for all module sources,
    - TEST_SOURCES for all unit test sources,
    - TEST_MOCKS for all moct sources.
5. Write tests.

### How To run tests in VSCode
1. Open "CMake" tab on left side bar.
2. Select "Host Test" configuration.
3. Select "Debug" target if you want to debug tests.
4. Build with "Build" button on bottom bar.
5. Open "Testing" tab on left side bar.
6. Click "Run Tests" button or "Run Test" for specific test.

### How To run tests in command line
To run tests, call the following commands:
1. cmake --preset host-tests
2. cmake --build --preset build-tests
3. ctest --test-dir ./build/build-host/tests







