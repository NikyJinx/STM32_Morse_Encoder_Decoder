/*
 * lcd_i2c.h
 *
 *  Created on: Apr 15, 2025
 *      Author: vital
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>

// Indirizzo I2C del modulo LCD (PCF8574)
#ifndef SLAVE_ADDRESS_LCD
#define SLAVE_ADDRESS_LCD    (0x27 << 1)  // modifica se diverso
#endif

// Dimensioni del display
#define LCD_COLS  16
#define LCD_ROWS  2

// Inizializza il driver LCD, deve essere chiamato una volta all'avvio
void lcd_init(I2C_HandleTypeDef *hi2c);

// Comandi di basso livello
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);

// Posizionamento e gestione display
void lcd_clear(void);
void lcd_put_cur(int row, int col);

// Funzioni di alto livello
void lcd_send_string(char *str);
void lcd_scroll_auto_tick(char *str);

// Debug e verifica
uint8_t lcd_compare_string(const char* expected);
void lcd_debug_print_shadow(void);

#ifdef __cplusplus
}
#endif

#endif // LCD_I2C_H
