///*
// * morse_utils.c
// *
// *  Created on: Apr 16, 2025
// *      Author: vital
// */
//
//
//#include "morse_utils.h"
//#include <string.h>
//#include <stdio.h>
//
//
//
//MorseContext morse_ctx;
//
//uint32_t dot_duration 	= 100;
//uint32_t dash_duration 	= 300;
//uint32_t letter_gap    	= 300;
//uint32_t word_gap     	= 700;
//
//extern UART_HandleTypeDef huart2;
//
///*FUNZIONI TRADUZIONE MORSE TO LETTER*/
//void morse_init(MorseContext* ctx) {
//	ctx->dot_duration = dot_duration;
//    ctx->morse_word_index = 0;
//    ctx->morse_word[0] = '\0';
//}
//
//
//
//void add_morse_symbol(MorseContext* ctx, char symbol) {
//    if (ctx->morse_word_index < sizeof(ctx->morse_word) - 1) {
//        ctx->morse_word[ctx->morse_word_index++] = symbol;
//        ctx->morse_word[ctx->morse_word_index] = '\0';
//    }
//}
//
//void reset_morse_word(MorseContext* ctx) {
//    ctx->morse_word_index = 0;
//    ctx->morse_word[0] = '\0';
//}
//
//const char* get_morse_word(const MorseContext* ctx) {
//    return ctx->morse_word;
//}
//
///*char sentence → è un solo carattere*/
//void send_uart_char(char symbol) {
//    HAL_UART_Transmit(&huart2, (uint8_t*)&symbol, sizeof(symbol), HAL_MAX_DELAY);
//    HAL_Delay(5);
//}
//
///*const char* sentence → è una stringa*/
//void print_uart(const char* sentence) {
//    HAL_UART_Transmit(&huart2, (uint8_t*)sentence, strlen(sentence), HAL_MAX_DELAY);
//    HAL_Delay(5);
//}
///***************************************************************************************************/
///*FUNZIONI LAVORO TRADUZIONE MORSE TO LETTER*/
///***************************************************************************************************/
//void gestisci_inizio_pressione(MorseContext* ctx, uint32_t now) {
//    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET && ctx->button_was_pressed == 0) {
//        ctx->t_start = now;
//        HAL_Delay(10);
//        ctx->button_was_pressed = 1;
//    }
//}
//
//void gestisci_fine_pressione(MorseContext* ctx, uint32_t now) {
//    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET && ctx->button_was_pressed == 1) {
//        ctx->t_end = now;
//        HAL_Delay(5);
//        ctx->button_was_pressed = 0;
//
//        uint32_t duration = ctx->t_end - ctx->t_start;
//        uint32_t dash_threshold = 3 * ctx->dot_duration;
//
//        if (duration <= dash_threshold) {
//            send_uart_char('.');
//            add_morse_symbol(ctx, '.');
//        } else {
//            send_uart_char('-');
//            add_morse_symbol(ctx, '-');
//        }
//
////        add_press_time(ctx, duration);
//        ctx->last_release_time = now;
//        ctx->space_sent = 0;
//        ctx->slash_sent = 0;
//    }
//}
//
//void gestisci_fine_pressione_debug(MorseContext* ctx, uint32_t now) {
//    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET && ctx->button_was_pressed == 1) {
//        ctx->t_end = now;
//        HAL_Delay(5);
//        ctx->button_was_pressed = 0;
//
//        uint32_t duration = ctx->t_end - ctx->t_start;
//        uint32_t dash_threshold = 3 * ctx->dot_duration;
//
//        if (duration <= dash_threshold) {
///*ZONA DI DEBUG PER SAPERE I TEMPI PRIMA DELL INVIO DI CARATTERE*/
//char buffer[32];
//sprintf(buffer, "[%lu ms] ", duration);  // \r\n per andare a capo
//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
///*------------------------------------------------------------------------*/
//        	send_uart_char('.');
//            add_morse_symbol(ctx, '.');
//        } else {
//
///*ZONA DI DEBUG PER SAPERE I TEMPI PRIMA DELL INVIO DI CARATTERE*/
//
//char buffer[32];
//sprintf(buffer, "[%lu ms] ", duration);  // \r\n per andare a capo
//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
///*------------------------------------------------------------------------*/
//
//            send_uart_char('-');
//            add_morse_symbol(ctx, '-');
//        }
//
////      add_press_time(ctx, duration);
//        ctx->last_release_time = now;
//        ctx->space_sent = 0;
//        ctx->slash_sent = 0;
//    }
//}
//
//void gestisci_spaziatura(MorseContext* ctx, uint32_t now) {
////    uint32_t letter_gap = 3 * ctx->dot_duration;
////  uint32_t word_gap = 7 * ctx->dot_duration;
//
//    if (!ctx->button_was_pressed && (now - ctx->last_release_time > letter_gap)) {
//        if ((now - ctx->last_release_time > word_gap) && !ctx->slash_sent) {
//        	send_uart_char('/');
//            send_uart_char(' ');
//            add_morse_symbol(ctx, '/');
//            ctx->slash_sent = 1;
//        } else if (!ctx->space_sent) {
//            send_uart_char(' ');
//            add_morse_symbol(ctx, ' ');
//            ctx->space_sent = 1;
//        }
//    }
//}
//
//void gestisci_spaziatura_debug(MorseContext* ctx, uint32_t now) {
////    uint32_t letter_gap = 3 * ctx->dot_duration;
////  uint32_t word_gap = 7 * ctx->dot_duration;
//
//    if (!ctx->button_was_pressed && (now - ctx->last_release_time > letter_gap)) {
//        if ((now - ctx->last_release_time > word_gap) && !ctx->slash_sent) {
//
///*ZONA DI DEBUG PER SAPERE I TEMPI PRIMA DELL INVIO DI CARATTERE*/
//uint32_t gap_duration = now - ctx->last_release_time;
//char buffer[32];
//sprintf(buffer, "[%lu ms] ", gap_duration);  // \r\n per andare a capo
//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
///*------------------------------------------------------------------------*/
//
//        	send_uart_char('/');
//            send_uart_char(' ');
//            add_morse_symbol(ctx, '/');
//            ctx->slash_sent = 1;
//        } else if (!ctx->space_sent) {
///*ZONA DI DEBUG PER SAPERE I TEMPI PRIMA DELL INVIO DI CARATTERE*/
//uint32_t gap_duration = now - ctx->last_release_time;
//char buffer[32];
//sprintf(buffer, "[%lu ms] ", gap_duration);  // \r\n per andare a capo
//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
//            /*------------------------------------------------------------------------*/
//			send_uart_char(' ');
//            add_morse_symbol(ctx, ' ');
//            ctx->space_sent = 1;
//        }
//    }
//}
//
//
////void invia_parola(MorseContext* ctx, uint32_t now) {
////    if ((now - ctx->last_release_time > 3 * word_gap) && ctx->morse_word_index > 0) {
////        send_uart_char('\n');
////        send_uart_char('\r');
////        HAL_UART_Transmit(&huart2, (uint8_t*)"Parola inviata: ", 16, HAL_MAX_DELAY);
////
////        for (int i = 0; i < ctx->morse_word_index; i++) {
////            send_uart_char(ctx->morse_word[i]);
////        }
////
////        HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);
////        reset_morse_word(ctx);
////    }
////}
//
//void invia_parola(MorseContext* ctx, uint32_t now) {
//    if ((now - ctx->last_release_time > 3 * word_gap) && ctx->morse_word_index > 0) {
//        send_uart_char('\n');
//        send_uart_char('\r');
//        HAL_UART_Transmit(&huart2, (uint8_t*)"Parola in Morse: ", 17, HAL_MAX_DELAY);
//
//        for (int i = 0; i < ctx->morse_word_index; i++) {
//            send_uart_char(ctx->morse_word[i]);
//        }
//
//        HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);
//
//        // Traduzione
//        char tradotta[64];
//        decode_morse_sentence(ctx->morse_word, tradotta);
//
//        HAL_UART_Transmit(&huart2, (uint8_t*)"Tradotta in lettere: ", 22, HAL_MAX_DELAY);
//        HAL_UART_Transmit(&huart2, (uint8_t*)tradotta, strlen(tradotta), HAL_MAX_DELAY);
//        HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);
//
//        lcd_clear();
//        lcd_send_string(tradotta); // stampa su LCD
//
//        reset_morse_word(ctx);
//    }
//}
//
//
//
//
//void invia_parola_debug(MorseContext* ctx, uint32_t now) {
//    if ((now - ctx->last_release_time > 3 * word_gap) && ctx->morse_word_index > 0) {
//
///*ZONA DI DEBUG PER SAPERE I TEMPI PRIMA DELL INVIO DI CARATTERE*/
//uint32_t gap_duration = now - ctx->last_release_time;
//char buffer[32];
//sprintf(buffer, "[Gap: %lu ms]\r\n", gap_duration);  // \r\n per andare a capo
//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
///*------------------------------------------------------------------------*/
//
//    	send_uart_char('\n');
//        send_uart_char('\r');
//        HAL_UART_Transmit(&huart2, (uint8_t*)"Parola inviata: ", 16, HAL_MAX_DELAY);
//
//        for (int i = 0; i < ctx->morse_word_index; i++) {
//            send_uart_char(ctx->morse_word[i]);
//        }
//
//        HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);
//        reset_morse_word(ctx);
//    }
//}
//
//
//// Restituisce il carattere corrispondente a una sequenza morse
//char convert_from_morse(const char* morse_code) {
//    const char *morse_table[] = {
//        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",     // A–I
//        ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",   // J–R
//        "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",          // S–Z
//        "-----", ".----", "..---", "...--", "....-", ".....",              // 0–5
//        "-....", "--...", "---..", "----."                                 // 6–9
//    };
//
//    for (int i = 0; i < 36; i++) {
//        if (strcmp(morse_code, morse_table[i]) == 0) {
//            if (i < 26)
//                return 'A' + i;
//            else
//                return '0' + (i - 26);
//        }
//    }
//    return '?'; // carattere sconosciuto
//}
//
//
//void decode_morse_sentence(const char* morse_sentence, char* output) {
//    output[0] = '\0';
//    char buffer[10];
//    int buf_index = 0;
//
//    for (int i = 0; morse_sentence[i] != '\0'; i++) {
//        if (morse_sentence[i] == '.' || morse_sentence[i] == '-') {
//            buffer[buf_index++] = morse_sentence[i];
//            buffer[buf_index] = '\0';
//        }
//        else if (morse_sentence[i] == ' ' || morse_sentence[i] == '/') {
//            if (buf_index > 0) {
//                char c = convert_from_morse(buffer);
//                strncat(output, &c, 1);
//                buf_index = 0;
//            }
//
//            if (morse_sentence[i] == '/')
//                strcat(output, " ");
//        }
//    }
//
//    // gestisce l'ultimo simbolo
//    if (buf_index > 0) {
//        char c = convert_from_morse(buffer);
//        strncat(output, &c, 1);
//    }
//}
//
//
//
//
//
//
