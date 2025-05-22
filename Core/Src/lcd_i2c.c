/*
 * lcd_i2c.c
 *
 *  Created on: Apr 15, 2025
 *      Author: vital
 */

#include "lcd_i2c.h"
#include <stdio.h>
#include <string.h>

// Static variable to store the I2C handle pointer.
// Initialized to NULL for safety. PRIVATE to this file.
static I2C_HandleTypeDef *s_lcd_i2c_handle = NULL;
extern UART_HandleTypeDef huart2;

static uint32_t last_lcd_tick = 0;
static int lcd_index = 0;
static char *lcd_text = NULL;
static uint8_t failed = 0;

static int lcd_row = 0;
static int lcd_col = 0;
static char lcd_shadow[LCD_ROWS][LCD_COLS];



#define LCD_COLS 16

void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
  data_u = (cmd&0xf0);
  data_l = ((cmd<<4)&0xf0);
  uint8_t data_t[4];
  data_t[0] = data_u|0x0C;  //en=1, rs=0 -> bxxxx1100
  data_t[1] = data_u|0x08;  //en=0, rs=0 -> bxxxx1000
  data_t[2] = data_l|0x0C;  //en=1, rs=0 -> bxxxx1100
  data_t[3] = data_l|0x08;  //en=0, rs=0 -> bxxxx1000

  int retry = 3;
      HAL_StatusTypeDef ret;
      do {
          ret = HAL_I2C_Master_Transmit(s_lcd_i2c_handle, SLAVE_ADDRESS_LCD, data_t, 4, 100);
      } while (ret != HAL_OK && --retry > 0);

      if (ret != HAL_OK) {
          const char* err_msg = "LCD ERROR: I2C transmit failed (cmd)\r\n";
          HAL_UART_Transmit(&huart2, (uint8_t*)err_msg, strlen(err_msg), HAL_MAX_DELAY);
      }

}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[1] = data_u|0x09;  //en=0, rs=1 -> bxxxx1001
	data_t[2] = data_l|0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[3] = data_l|0x09;  //en=0, rs=1 -> bxxxx1001

	int retry = 3;
	    HAL_StatusTypeDef ret;
	    do {
	        ret = HAL_I2C_Master_Transmit(s_lcd_i2c_handle, SLAVE_ADDRESS_LCD, data_t, 4, 100);
	        ////////////////////////////////////////////////
	        //per il debug in lcd_debug_print_shadow_hex
	        if (lcd_row < LCD_ROWS && lcd_col < LCD_COLS)
	            lcd_shadow[lcd_row][lcd_col] = data;

	        lcd_col++;
	        if (lcd_col >= LCD_COLS) {
	            lcd_col = 0;
	            lcd_row = (lcd_row + 1) % LCD_ROWS;
	        }
	       ///////////////////////////////////////////////////
	    } while (ret != HAL_OK && --retry > 0);



	    if (ret != HAL_OK) {
	    	if (failed == 0){

			const char* err_msg = 	"*************************************\n\r"
									"LCD ERROR: I2C transmit failed (data)\r\n"
									"The message transmitted has an error\n\r"
									"*************************************\n\r";
			HAL_UART_Transmit(&huart2, (uint8_t*)err_msg, strlen(err_msg), HAL_MAX_DELAY);
	        failed = 1;
	    	}
	    }
}


void lcd_init(I2C_HandleTypeDef *hi2c) // Takes handle from user
  {
      s_lcd_i2c_handle = hi2c; // Store the handle in our private variable

      // Now, perform LCD hardware initialization using the stored handle
      if (s_lcd_i2c_handle != NULL)
      {
          // Send initialization commands (example sequence)
          HAL_Delay(50); 		// Wait after power on
          lcd_send_cmd(0x30); 	// Function set based on datasheet needs...
          HAL_Delay(20);
          lcd_send_cmd(0x30);
          HAL_Delay(20);
          lcd_send_cmd(0x30);
          HAL_Delay(20);
          lcd_send_cmd(0x20); 	// Set 4-bit mode based on datasheet needs...
          HAL_Delay(20);

          lcd_send_cmd(0x28); 	// Function Set: 4-bit, 2 lines, 5x8 font
          HAL_Delay(20);
          lcd_send_cmd(0x08); 	// Display Off
          HAL_Delay(20);
          lcd_send_cmd(0x01); 	// Clear Display
          HAL_Delay(20); 		// Clear takes longer
          lcd_send_cmd(0x06); 	// Entry Mode Set: Increment cursor, no shift
          HAL_Delay(20);
          lcd_send_cmd(0x0C); 	// Display On: Display on, cursor off, blink off
      }
      // else { Handle error - init called with NULL handle }
  }


void lcd_scroll_auto_tick(char *str)
{
    if (lcd_text != str) {
        lcd_text = str;
        lcd_index = 0;
        last_lcd_tick = HAL_GetTick();
        lcd_clear();
    }

    // Aggiorna ogni 1000 ms
    if (HAL_GetTick() - last_lcd_tick < 1000) return;

    int len = strlen(lcd_text);

    lcd_clear();
    lcd_put_cur(0, 0);
    for (int i = 0; i < 16 && (lcd_index + i) < len; i++) {
        lcd_send_data(lcd_text[lcd_index + i]);
    }

    lcd_index += 16;
    if (lcd_index >= len) lcd_index = 0;
    HAL_Delay(10);
    last_lcd_tick = HAL_GetTick();
}


void lcd_send_string (char *str)
{
    int col = 0;
    int row = 0;
    lcd_put_cur(row, col);  // Inizia da (0,0)

    while (*str)
    {
        lcd_send_data(*str++);
        col++;

        if (col >= LCD_COLS)
        {
            col = 0;
            row++;
            if (row > 1) break;  // Solo 2 righe nel display 16x2
            lcd_put_cur(row, col);
        }
    }
}

void lcd_put_cur(int row, int col)
{
	lcd_row = row;
	lcd_col = col;
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_send_cmd (col);
}

/*steo preliminare per sapere l'indirizzo del lcd stampa 0x27 e va shiftato di 1 quindi (0x27 << 1)  // = 0x4E
poiche l'indirizzo è a 7bit e lo si invia come un byte dove il LSB indica R/W
1 = read, 0 = write, devo usare lcd come scrittura per printare quindi lo sfifto ponendo il LSB a zero
0x27 = 0b00100111  → shift di 1 bit → 0x4E = 0b01001110 */
void I2C_Scanner(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart)
{
  char info[64];
  HAL_StatusTypeDef result;
  for (uint16_t i = 1; i < 128; i++) {
    result = HAL_I2C_IsDeviceReady(hi2c, (i << 1), 1, 10);
    if (result == HAL_OK) {
      sprintf(info, "Dispositivo trovato all'indirizzo: 0x%X\r\n", i);
      HAL_UART_Transmit(huart, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);
    }
  }
}

void lcd_clear (void)
{
lcd_send_cmd(0x01);
   HAL_Delay(20);


   // Azzera il buffer shadow
   for (int r = 0; r < LCD_ROWS; r++)
       for (int c = 0; c < LCD_COLS; c++)
           lcd_shadow[r][c] = ' ';

   lcd_row = 0;
   lcd_col = 0;
}

uint8_t lcd_compare_string(const char* expected) {
    for (int row = 0; row < LCD_ROWS; row++) {
        for (int col = 0; col < LCD_COLS; col++) {
            int idx = row * LCD_COLS + col;
            if (expected[idx] == '\0') return 1; // fine stringa, tutto combacia
            if (lcd_shadow[row][col] != expected[idx]) return 0; // mismatch
        }
    }
    return 1; // tutto combacia
}

void lcd_debug_print_shadow_hex(void)
{
    char buf[6];  // per contenere "XX " + terminatore
    for (int row = 0; row < LCD_ROWS; row++) {
        for (int col = 0; col < LCD_COLS; col++) {
            uint8_t val = (uint8_t)lcd_shadow[row][col];
            snprintf(buf, sizeof(buf), "%02X ", val);
            HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
        }
        // Vai a capo alla fine di ogni riga
        const char newline[] = "\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)newline, strlen(newline), HAL_MAX_DELAY);
    }
}
