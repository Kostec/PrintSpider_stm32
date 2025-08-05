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

**Note**: In case of start debug (F5) SWO task will starts with error - just continue debugging.

