///*
// * morse_utils.h
// *
// *  Created on: Apr 16, 2025
// *      Author: vital
// */
//
//#ifndef INC_MORSE_UTILS_H_
//#define INC_MORSE_UTILS_H_
//
//#include "stm32f4xx_hal.h"  // Per HAL_GetTick, GPIO, UART, ecc.
//
//
//extern uint32_t dot_duration;
//
//
//
////Definizione della struct che usi per il contesto
//typedef struct {
//    uint32_t t_start;
//    uint32_t t_end;
//    uint32_t last_release_time;
//    uint32_t dot_duration;
//    uint8_t morse_word_index;
//    char morse_word[100];  // Dimensione dell'array può essere adattata
//    uint8_t button_was_pressed;
//    uint8_t space_sent;
//    uint8_t slash_sent;
//} MorseContext;
//
//extern MorseContext morse_ctx;
///*Voglio morire 1h per capire che extern MorseContext morse_ctx;
// * va inserito dopo la dichiarazione di typedef struct{...} MorseContext;
// * mannaggia a chi compila  */
//
//// 📌 Prototipi delle funzioni (dichiarazioni)
//void gestisci_inizio_pressione(MorseContext* ctx, uint32_t now);
//void gestisci_fine_pressione(MorseContext* ctx, uint32_t now);
//void gestisci_spaziatura(MorseContext* ctx, uint32_t now);
//void invia_parola(MorseContext* ctx, uint32_t now);
//char convert_from_morse(const char* morse_code);
//void decode_morse_sentence(const char* morse_sentence, char* output);
//
//void print_uart(const char* sentence);
//
//void send_uart_char(char symbol);  // Se è usata anche fuori
//void add_morse_symbol(MorseContext* ctx, char symbol);
//void reset_morse_word(MorseContext* ctx);
//
//void morse_init(MorseContext* ctx);
//void loop_morse_to_letter();
//
//
///*FUNZIONI DI DEBUG PER TEMPI DI DOT DASH SLASH WORD*/
//
//void gestisci_fine_pressione_debug(MorseContext* ctx, uint32_t now);
//void gestisci_spaziatura_debug(MorseContext* ctx, uint32_t now);
//void invia_parola_debug(MorseContext* ctx, uint32_t now);
//
//
//
////uint32_t average_press_time(const MorseContext* ctx);
////void add_press_time(MorseContext* ctx, uint32_t duration);
//
//
//#endif /* INC_MORSE_UTILS_H_ */
