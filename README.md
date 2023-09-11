# DAT-Firmware
Firmware for the Dalhousie Autonomous Underwater Vechile Design Team
Contains two firmware projects
1. Thrust Controller project is using the NUCLEO-F334R8
2. Sensor Controller project is using the NUCLEO-F767ZI 

Please use the STMCube IDE to program the corresponding boards 

## VS Code configuration
To use VS Code as your IDE please follow the steps below:
1. Software Install
    Ensure that CMake and Ninja are added to your [environment variables](https://docs.oracle.com/en/database/oracle/machine-learning/oml4r/1.5.1/oread/creating-and-modifying-environment-variables-on-windows.html#:~:text=On%20the%20Windows%20taskbar%2C%20right,Settings%2C%20click%20Advanced%20system%20settings.&text=On%20the%20Advanced%20tab%2C%20click%20Environment%20Variables.&text=Click%20New%20to%20create%20a%20new%20environment%20variable.) 
    1. [CMake](https://cmake.org/download/)
    2. [Ninja Build System](https://ninja-build.org/)
    3. [STM32CubeCLT](https://www.st.com/en/development-tools/stm32cubeclt.html)
    4. [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
    5. [ST-MCU_FINDER-PC](https://www.st.com/en/development-tools/st-mcu-finder-pc.html)
    6. [GIT](https://git-scm.com/)
2. VS Code Extensions 
    1. [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
    2. [STM32 VS Code Extension](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension)
3. Import one of the Firmware Projects using the STM32 Extension
    1. Use the Import a local project option in the STM32 extension
    2. Find and open the corresponding .cproject file
    3. Once the project has been imported following the prompts and select the debug preset when asked
    4. Build and run the code with the NUCLEO MCU connected
## WSL Install
Windows Subsystem for Linux or WSL is a way to run a unix shell from within a windows machine, this can be used to test the firmware with 
software that will run on the SOC's (System On a Chip). The following [link](https://ubuntu.com/tutorials/install-ubuntu-on-wsl2-on-windows-10#1-overview) 
is a guide to setup WSL, currenlty the target OS is Ubuntu 20.04.



If you are having issues please reachout to our teams firmware devs!