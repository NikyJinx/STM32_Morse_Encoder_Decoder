///*
// * morse_io_utils.c
// *
// *  Created on: Apr 17, 2025
// *      Author: vital
// */
//
//#include "morse_io_utils.h"
//#include "lcd_i2c.h"
//
//
//extern UART_HandleTypeDef huart2;
//#define COMANDO_MAX     32
//
//
//// Pin definitions and hardware port
//#define LED_PORT        GPIOA
//#define LED_PIN         GPIO_PIN_5
//#define BUZZER_PIN      GPIO_PIN_6
//
//// time in ms for the morse code
//#define DOT_DURATION     200
//#define DASH_DURATION    (3 * DOT_DURATION)
//#define LETTER_GAP       (3 * DOT_DURATION)
//#define WORD_GAP         (7 * DOT_DURATION)
//#define SYMBOL_GAP       DOT_DURATION
//
//// Morse table convertion A-Z + 0-9 (index: 0–25 = letter, 26–35 = number)
//const char *morse_table[] = {
//    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",     // A–I
//    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",   // J–R
//    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",          // S–Z
//    "-----", ".----", "..---", "...--", "....-", ".....",              // 0–5
//    "-....", "--...", "---..", "----."                                 // 6–9
//};
//
//void convert_to_morse(const char *message, char *output) {
//    output[0] = '\0'; // Initialize the output string
//
//    for (int i = 0; i < strlen(message); i++) {
//        char c = message[i];
//
//        if (c == ' ') {
//            strcat(output, " / "); // Separator between words
//            continue;
//        }
//
//        int index;
//        if (c >= 'A' && c <= 'Z') {
//            index = c - 'A';
//        } else if (c >= 'a' && c <= 'z') {
//            index = c - 'a';
//        } else if (c >= '0' && c <= '9') {
//            index = c - '0' + 26;
//        } else {
//            continue; // Ignore of invalid character
//        }
//
//        strcat(output, morse_table[index]);
//        strcat(output, " ");
//    }
//}
//
//// Funzione per lampeggiare il LED e inviare il Morse tramite UART
///*l'uso del trasmit con il timeout mi modifica i tempi di accensione del led se arriva
// * ad aspettare 100ms giusto?
// * so che Il baudrate è 115200 bit al secondo.
// * Un carattere è composto da 10 bit (1 bit di start + 8 bit di dati + 1 bit di stop).
// * quindi ho T = 86.8us per 4 caratteri circa 347.2us quindi sto apposto?*/
//
//void play_morse_code(const char *morse_output, uint8_t use_led, uint8_t use_buzzer, uint8_t use_uart) {
//    for (int i = 0; i < strlen(morse_output); i++) {
//        char c = morse_output[i];
//
//        // Aggiorna il display in modo non bloccante
//        lcd_scroll_auto_tick((char*)morse_output);
//
//        uint16_t pin_mask = 0;
//        if (use_led)    pin_mask |= LED_PIN;
//        if (use_buzzer) pin_mask |= BUZZER_PIN;
//
//        if (c == '.') {
//            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)".", 1, 100);
//            if (pin_mask) HAL_GPIO_WritePin(LED_PORT, pin_mask, GPIO_PIN_SET);
//            HAL_Delay(DOT_DURATION);
//            if (pin_mask) HAL_GPIO_WritePin(LED_PORT, pin_mask, GPIO_PIN_RESET);
//        }
//        else if (c == '-') {
//            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)"-", 1, 100);
//            if (pin_mask) HAL_GPIO_WritePin(LED_PORT, pin_mask, GPIO_PIN_SET);
//            HAL_Delay(DASH_DURATION);
//            if (pin_mask) HAL_GPIO_WritePin(LED_PORT, pin_mask, GPIO_PIN_RESET);
//        }
//        else if (c == ' ') {
//            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)" ", 1, 100);
//            HAL_Delay(LETTER_GAP - SYMBOL_GAP);
//        }
//        else if (c == '/') {
//            if (use_uart) {
//                const char* gap = " / ";
//                HAL_UART_Transmit(&huart2, (uint8_t*)gap, strlen(gap), 100);
//            }
//            HAL_Delay(WORD_GAP - SYMBOL_GAP);
//        }
//
//        HAL_Delay(SYMBOL_GAP);
//    }
//}
//
////void ricevi_comando_uart(char *buffer) {
////    uint8_t i = 0;
////    char c;
////
////    while (1) {
////        HAL_UART_Receive(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
////
////        if (c == '\r' || c == '\n') {
////            buffer[i] = '\0';
////            break;
////        } else {
////            if (i < COMANDO_MAX - 1) {
////                buffer[i++] = c;
////            }
////        }
////
////        HAL_UART_Transmit(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
////    }
////
////    const char* crlf = "\r\n";
////    HAL_UART_Transmit(&huart2, (uint8_t*)crlf, strlen(crlf), HAL_MAX_DELAY);
////}
//
//
//
//bool ricevi_comando_uart(char *buffer) {
//    uint8_t i = 0;
//    char c;
//
//    while (1) {
//        HAL_UART_Receive(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
//
//        // ESC o DEL → Return immediately and signal exit
//        if (c == 27 || c == 127) {
//            buffer[0] = '\0';
//            return true;  // exit request
//        }
//
//        if (c == '\r' || c == '\n') {
//            buffer[i] = '\0';
//            break;
//        } else {
//            if (i < COMANDO_MAX - 1) {
//                buffer[i++] = c;
//            }
//        }
//
//        HAL_UART_Transmit(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
//    }
//
//    const char* crlf = "\r\n";
//    HAL_UART_Transmit(&huart2, (uint8_t*)crlf, strlen(crlf), HAL_MAX_DELAY);
//
//    return false;  // Normal completion
//}
//
//void esegui_traduzione_morse(uint8_t use_led, uint8_t use_buzzer, uint8_t use_uart) {
//    char comando[COMANDO_MAX];
//    char morse_output[256];
//
//    // 1. Ask the user to enter a word.
//    const char* prompt = "Enter a word to translate into Morse:\r\n";
//    HAL_UART_Transmit(&huart2, (uint8_t*)prompt, strlen(prompt), HAL_MAX_DELAY);
//
//    // 2. Receive the command from the terminal.
//    ricevi_comando_uart(comando);
//
//    // 3. Convert it to Morse code.
//    convert_to_morse(comando, morse_output);
//
//    // --- STAMPA SU LCD ---
//        lcd_clear();                       // clr display
//        lcd_send_string(morse_output);     // write morse code
//        //lcd_debug_print_shadow_hex();
//
//    // (Optional) Print the plain translation in advance.
//    if (use_uart) {
//        const char* prefix = "Will send at rhythm:\r\n";
//        HAL_UART_Transmit(&huart2, (uint8_t*)prefix, strlen(prefix), HAL_MAX_DELAY);
//        HAL_UART_Transmit(&huart2, (uint8_t*)morse_output, strlen(morse_output), HAL_MAX_DELAY);
//        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
//    }
//
//    // 4. Perform Morse playback (LED, BUZZER, UART).
//    play_morse_code(morse_output, use_led, use_buzzer, use_uart);
//    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
//}
//
