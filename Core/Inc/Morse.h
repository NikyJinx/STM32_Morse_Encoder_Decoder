/*
 * Morse.h
 *
 *  Created on: Apr 25, 2025
 *      Author: vital
 */

#ifndef INC_MORSE_H_
#define INC_MORSE_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// Struct per il contesto Morse
typedef struct {
    uint32_t t_start;
    uint32_t t_end;
    uint32_t last_release_time;
    uint32_t dot_duration;
    uint8_t morse_word_index;
    char morse_word[100];
    uint8_t button_was_pressed;
    uint8_t space_sent;
    uint8_t slash_sent;
} MorseContext;

extern MorseContext morse_ctx;
extern uint32_t dot_duration;
extern uint32_t dash_duration;
extern uint32_t letter_gap;
extern uint32_t word_gap;

// Inizializzazione
void morse_init(MorseContext* ctx);

// Codifica e decodifica Morse
void configura_dot_duration(void);
void convert_to_morse(const char *message, char *output);
char convert_from_morse(const char* morse_code);
void decode_morse_sentence(const char* morse_sentence, char* output);

// Gestione stringa morse (input)
void add_morse_symbol(MorseContext* ctx, char symbol);
void reset_morse_word(MorseContext* ctx);
const char* get_morse_word(const MorseContext* ctx);

// UART I/O
void send_uart_char(char symbol);
void print_uart(const char* sentence);

// Gestione pulsante
void gestisci_inizio_pressione(MorseContext* ctx, uint32_t now);
void gestisci_fine_pressione(MorseContext* ctx, uint32_t now);
void gestisci_spaziatura(MorseContext* ctx, uint32_t now);
void invia_parola(MorseContext* ctx, uint32_t now);

void gestisci_fine_pressione_debug(MorseContext* ctx, uint32_t now);
void gestisci_spaziatura_debug(MorseContext* ctx, uint32_t now);
void invia_parola_debug(MorseContext* ctx, uint32_t now);
// Funzioni di interfaccia
bool ricevi_comando_uart(char *buffer);
void esegui_traduzione_morse(uint8_t use_led, uint8_t use_buzzer, uint8_t use_uart);
void play_morse_code(const char *morse_output, uint8_t use_led, uint8_t use_buzzer, uint8_t use_uart);

#endif /* INC_MORSE_H_ */

