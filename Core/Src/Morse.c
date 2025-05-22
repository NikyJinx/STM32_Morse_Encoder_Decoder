/* morse.c - Unificazione di morse_utils.c e morse_io_utils.c */

#include "morse.h"
#include "lcd_i2c.h"
#include <string.h>
#include <stdio.h>

MorseContext morse_ctx;

// Configurazione default durate morse (ms)
uint32_t dot_duration  = 100;
uint32_t dash_duration = 300;
uint32_t letter_gap    = 300;
uint32_t word_gap      = 700;

extern UART_HandleTypeDef huart2;

// Tabella Morse A-Z + 0-9
const char *morse_table[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    "-----", ".----", "..---", "...--", "....-", ".....",
    "-....", "--...", "---..", "----."
};

// ===================== IMPOSTAZIONI =====================
void configura_dot_duration() {
    char input[16];
    const char* prompt = 	"Please enter a value between 50 and 3000 ms\r\n"
							"Suggested value: 100 ms\r\n"
							"Dash duration = 3 × dot duration\r\n"
							"Letter gap = 3 × dot duration\r\n"
							"Word gap = 7 × dot duration\r\n"
							"\r\n"
							"Enter dot duration (ms):\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)prompt, strlen(prompt), HAL_MAX_DELAY);
    if (ricevi_comando_uart(input)) return;
    uint32_t nuovo_dot = atoi(input);
    if (nuovo_dot >= 50 && nuovo_dot <= 3000) {
        dot_duration  = nuovo_dot;
        dash_duration = 3 * dot_duration;
        letter_gap    = 3 * dot_duration;
        word_gap      = 7 * dot_duration;
        char conferma[64];
        sprintf(conferma, "Dot duration set to %lu ms\r\n", dot_duration);
        HAL_UART_Transmit(&huart2, (uint8_t*)conferma, strlen(conferma), HAL_MAX_DELAY);
    } else {
        const char* err = 	"Invalid value. Range: 50 - 1000 ms\r\n"
        					"Dot duration will be set to default value: 100 ms\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)err, strlen(err), HAL_MAX_DELAY);
    }
}




/*************************** Funzioni di Inizializzazione ***************************/
void morse_init(MorseContext* ctx) {
    ctx->dot_duration = dot_duration;
    ctx->morse_word_index = 0;
    ctx->morse_word[0] = '\0';
}

void add_morse_symbol(MorseContext* ctx, char symbol) {
    if (ctx->morse_word_index < sizeof(ctx->morse_word) - 1) {
        ctx->morse_word[ctx->morse_word_index++] = symbol;
        ctx->morse_word[ctx->morse_word_index] = '\0';
    }
}

void reset_morse_word(MorseContext* ctx) {
    ctx->morse_word_index = 0;
    ctx->morse_word[0] = '\0';
}

const char* get_morse_word(const MorseContext* ctx) {
    return ctx->morse_word;
}

/*************************** UART Output ***************************/
void send_uart_char(char symbol) {
    HAL_UART_Transmit(&huart2, (uint8_t*)&symbol, sizeof(symbol), HAL_MAX_DELAY);
    HAL_Delay(5);
}

void print_uart(const char* sentence) {
    HAL_UART_Transmit(&huart2, (uint8_t*)sentence, strlen(sentence), HAL_MAX_DELAY);
    HAL_Delay(5);
}

/*************************** Conversione Morse <-> Lettere ***************************/
void convert_to_morse(const char *message, char *output) {
    output[0] = '\0';
    for (int i = 0; i < strlen(message); i++) {
        char c = message[i];
        int index;

        if (c == ' ') {
            strcat(output, " / ");
            continue;
        }
        if (c >= 'A' && c <= 'Z') index = c - 'A';
        else if (c >= 'a' && c <= 'z') index = c - 'a';
        else if (c >= '0' && c <= '9') index = c - '0' + 26;
        else continue;

        strcat(output, morse_table[index]);
        strcat(output, " ");
    }
}

char convert_from_morse(const char* morse_code) {
    for (int i = 0; i < 36; i++) {
        if (strcmp(morse_code, morse_table[i]) == 0) {
            return (i < 26) ? ('A' + i) : ('0' + (i - 26));
        }
    }
    return '?';
}

void decode_morse_sentence(const char* morse_sentence, char* output) {
    output[0] = '\0';
    char buffer[10];
    int buf_index = 0;

    for (int i = 0; morse_sentence[i] != '\0'; i++) {
        if (morse_sentence[i] == '.' || morse_sentence[i] == '-') {
            buffer[buf_index++] = morse_sentence[i];
            buffer[buf_index] = '\0';
        }
        else if (morse_sentence[i] == ' ' || morse_sentence[i] == '/') {
            if (buf_index > 0) {
                char c = convert_from_morse(buffer);
                strncat(output, &c, 1);
                buf_index = 0;
            }
            if (morse_sentence[i] == '/') strcat(output, " ");
        }
    }
    if (buf_index > 0) {
        char c = convert_from_morse(buffer);
        strncat(output, &c, 1);
    }
}

/*************************** I/O Pulsante ***************************/
void gestisci_inizio_pressione(MorseContext* ctx, uint32_t now) {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET && ctx->button_was_pressed == 0) {
        ctx->t_start = now;
        HAL_Delay(5);
        ctx->button_was_pressed = 1;
    }
}

void gestisci_fine_pressione(MorseContext* ctx, uint32_t now) {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET && ctx->button_was_pressed == 1) {
        ctx->t_end = now;
        HAL_Delay(5);
        ctx->button_was_pressed = 0;

        uint32_t duration = ctx->t_end - ctx->t_start;
//        uint32_t dash_threshold = 3 * ctx->dot_duration;

        if (duration <= dot_duration) {
            send_uart_char('.');
            add_morse_symbol(ctx, '.');
        } else {
            send_uart_char('-');
            add_morse_symbol(ctx, '-');
        }

        ctx->last_release_time = now;
        ctx->space_sent = 0;
        ctx->slash_sent = 0;
    }
}

void gestisci_fine_pressione_debug(MorseContext* ctx, uint32_t now) {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET && ctx->button_was_pressed == 1) {
        ctx->t_end = now;
        HAL_Delay(5);
        ctx->button_was_pressed = 0;

        uint32_t duration = ctx->t_end - ctx->t_start;
        char buffer[32];
        sprintf(buffer, "[%lu ms] ", duration);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

        if (duration <= dot_duration) {
            send_uart_char('.');
            add_morse_symbol(ctx, '.');
        } else {
            send_uart_char('-');
            add_morse_symbol(ctx, '-');
        }

        ctx->last_release_time = now;
        ctx->space_sent = 0;
        ctx->slash_sent = 0;
    }
}

void gestisci_spaziatura(MorseContext* ctx, uint32_t now) {
    if (!ctx->button_was_pressed && (now - ctx->last_release_time > letter_gap)) {
        if ((now - ctx->last_release_time > word_gap) && !ctx->slash_sent) {
            send_uart_char('/');
            send_uart_char(' ');
            add_morse_symbol(ctx, '/');
            ctx->slash_sent = 1;
        } else if (!ctx->space_sent) {
            send_uart_char(' ');
            add_morse_symbol(ctx, ' ');
            ctx->space_sent = 1;
        }
    }
}

void gestisci_spaziatura_debug(MorseContext* ctx, uint32_t now) {
    if (ctx->last_release_time == 0) return;

    if (!ctx->button_was_pressed && (now - ctx->last_release_time > letter_gap)) {
        uint32_t gap_duration = now - ctx->last_release_time;

        if ((gap_duration > word_gap) && !ctx->slash_sent) {
            char buffer[32];
            sprintf(buffer, "[%lu ms] ", gap_duration);
            HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

            send_uart_char('/');
            send_uart_char(' ');
            add_morse_symbol(ctx, '/');
            ctx->slash_sent = 1;
        } else if (!ctx->space_sent) {
            char buffer[32];
            sprintf(buffer, "[%lu ms] ", gap_duration);
            HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

            send_uart_char(' ');
            add_morse_symbol(ctx, ' ');
            ctx->space_sent = 1;
        }
    }
}

void invia_parola(MorseContext* ctx, uint32_t now) {
    if ((now - ctx->last_release_time > 3 * word_gap) && ctx->morse_word_index > 0) {
        send_uart_char('\n');
        send_uart_char('\r');
        HAL_UART_Transmit(&huart2, (uint8_t*)"Word in Morse_code: ", 20, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)ctx->morse_word, ctx->morse_word_index, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);

        char tradotta[64];
        decode_morse_sentence(ctx->morse_word, tradotta);

        HAL_UART_Transmit(&huart2, (uint8_t*)"Translated in letter: ", 22, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)tradotta, strlen(tradotta), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)"\n\r", 2, HAL_MAX_DELAY);

        lcd_clear();
        lcd_send_string(tradotta);
        reset_morse_word(ctx);
    }
}

void invia_parola_debug(MorseContext* ctx, uint32_t now) {
    if ((now - ctx->last_release_time > 3 * word_gap) && ctx->morse_word_index > 0) {
        uint32_t gap_duration = now - ctx->last_release_time;
        char buffer[32];
        sprintf(buffer, "[Gap: %lu ms]\r\n", gap_duration);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

        invia_parola(ctx, now);  // riusa la funzione standard per evitare duplicazione
    }
}

bool ricevi_comando_uart(char *buffer) {
    uint8_t i = 0;
    char c;
    while (1) {
        HAL_UART_Receive(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
        if (c == 27 || c == 127) {
            buffer[0] = '\0';
            return true; //case pressed esc or del to implement a check and go back in a switch_case
        }
        if (c == '\r' || c == '\n') {
            buffer[i] = '\0';
            break;
        } else {
            if (i < 31) buffer[i++] = c;
        }
        HAL_UART_Transmit(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
    }
    const char* crlf = "\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)crlf, strlen(crlf), HAL_MAX_DELAY);
    return false;
}

void esegui_traduzione_morse(uint8_t use_led, uint8_t use_buzzer, uint8_t use_uart) {
    char comando[32];
    char morse_output[256];

    const char* prompt = "Enter a word to translate into Morse:\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)prompt, strlen(prompt), HAL_MAX_DELAY);

    ricevi_comando_uart(comando);
    convert_to_morse(comando, morse_output);

    lcd_clear();
    lcd_send_string(morse_output);

    if (use_uart) {
        const char* prefix = "Will send at rhythm:\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)prefix, strlen(prefix), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)morse_output, strlen(morse_output), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
    }

    play_morse_code(morse_output, use_led, use_buzzer, use_uart);
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}

void play_morse_code(const char *morse_output, uint8_t use_led, uint8_t use_buzzer, uint8_t use_uart) {
    for (int i = 0; i < strlen(morse_output); i++) {
        char c = morse_output[i];
        lcd_scroll_auto_tick((char*)morse_output);
        uint16_t pin_mask = 0;
        if (use_led) pin_mask |= GPIO_PIN_5;
        if (use_buzzer) pin_mask |= GPIO_PIN_6;

        if (c == '.') {
            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)".", 1, 100);
            if (pin_mask) HAL_GPIO_WritePin(GPIOA, pin_mask, GPIO_PIN_SET);
            HAL_Delay(dot_duration);
            if (pin_mask) HAL_GPIO_WritePin(GPIOA, pin_mask, GPIO_PIN_RESET);
        }
        else if (c == '-') {
            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)"-", 1, 100);
            if (pin_mask) HAL_GPIO_WritePin(GPIOA, pin_mask, GPIO_PIN_SET);
            HAL_Delay(dash_duration);
            if (pin_mask) HAL_GPIO_WritePin(GPIOA, pin_mask, GPIO_PIN_RESET);
        }
        else if (c == ' ') {
            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)" ", 1, 100);
            HAL_Delay(letter_gap - dot_duration);
        }
        else if (c == '/') {
            if (use_uart) HAL_UART_Transmit(&huart2, (uint8_t*)" / ", 3, 100);
            HAL_Delay(word_gap - dot_duration);
        }

        HAL_Delay(dot_duration);
    }
}
