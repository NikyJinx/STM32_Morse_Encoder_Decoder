# STM32 Morse Code Translator  
This embedded systems project implements a complete **Morse code translator** using the **STM32F401RE Nucleo** board.  
It supports both encoding (Text → Morse) and decoding (Morse → Text) using some hardware peripherals: **button**, **LED**, **buzzer**, **UART**, and **1602 LCD via I2C**.



###  Features and Functionalities
- Bi-directional Morse translation
- Real-time decoding from physical button (with timing-based symbol detection)
- Visual and acoustic feedback via:
  - LED (PA5)
  - Buzzer (PA6)
  - UART output (PA2/PA3)
  - I2C LCD display (PB8/PB9)
- Dynamic control of Morse timing (dot duration)
- UART menu for selecting modes
- Debug mode to analyze symbol timing


###  Library Organization

The current implementation is structured around:

- `Morse.c/h` → Unified and updated logic for all Morse-related functionalities  
- `lcd_i2c.c/h` → I2C driver for the 1602 LCD

> -> **Note**:  
> The files `morse_utils.c/h` and `morse_io_utils.c/h` are legacy modules from earlier development phases.  
> They have been fully merged and refactored into `Morse.c/h` for improved modularity and maintainability.  
> You may still explore them if you're curious, but they are no longer required.



##  Setup Instructions

### Option 1: Use the Full Project (Recommended)

1. Clone or download this repository
2. Open **STM32CubeIDE**
3. Import the folder into the workspace
4. The `.ioc` file is already included so all peripheral configurations are ready to use


### Option 2: Integrate into Your Own Project

Starting from scratch in STM32CubeIDE:

1. Create a new STM32 project with your custom `.ioc` (If you're using the F401RE, it is recommended to use the same pin config.) 
2. Copy the following files into the appropriate folders:
   - `Core/Src/Morse.c`
   - `Core/Src/lcd_i2c.c`
   - `Core/Inc/Morse.h`
   - `Core/Inc/lcd_i2c.h`
3. Write your own `main.c` logic using the provided functions



##  Supported Modes

###  Text → Morse
- Input via UART
- Output via:
  - LED blink
  - Buzzer
  - UART
  - LCD scrolling display

###  Morse → Text
- Input from button (PC13)
- Real-time decoding based on press duration
- Output via UART and LCD

###  Debug Mode
- Print symbol and gaps timings via UART
- Helps calibrate the dot threshold for custom input speed



##  Hardware Requirements

| Component          | Pin(s)         | Description                  |
|-------------------|----------------|------------------------------|
| LED               | PA5            | Onboard LED or external      |
| Buzzer            | PA6            | External active buzzer       |
| User Button       | PC13           | Onboard push button          |
| UART              | PA2 / PA3      | UART2 (TX / RX) via USB      |
| I2C LCD (1602)    | PB8 / PB9      | I2C1_SCL / I2C1_SDA          |

> UART configured at **115200 baud**, 8N1  
> LCD address expected to be **0x27** (default for most I2C modules)



##  Project Requirements

- **STM32CubeIDE** (for development, flashing, and debugging)
- **HAL drivers** auto-generated via CubeMX
- **No external libraries required** beyond HAL
- **Terminal to operate** such as PuTTy or TeraTerm 


## Extras

A full technical report (including hardware details, software structure, problems and solutions, and final considerations) is available at the following link:

-> [View Full Project Report (Word Document)](https://uniroma3-my.sharepoint.com/:w:/g/personal/nik_vitali_stud_uniroma3_it/EdLvl1YhtG5Mnz2qMNDI98cBlhqbKrtx-U_R7HBXcIzhtA?e=VL1X2D)




