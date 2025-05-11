#include <hardware/watchdog.h>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "i2ckbd.h"
#include "lcdspi.h"
#include "pico/stdlib.h"
#include "pkklib.h"

void pkk_sound_init(void);
void pkk_display_init(void);
void pkk_keyboard_init(void);

const uint LEDPIN = 25;

uint32_t pwm_clock_khz = 0;

extern uint32_t freq_playback;

// void panic(const char *format, ...) {
//     char buffer[128];

//     va_list args;
//     va_start(args, format);
//     vsnprintf(buffer, sizeof(buffer), format, args);
//     va_end(args);

//     pkk_guru_meditation(buffer);

//     while (1);
// }

void pkk_set_pwm_clock_khz(uint32_t clock_khz) { pwm_clock_khz = clock_khz; }

void pkk_init(void) {
    if (pwm_clock_khz != 0) {
        set_sys_clock_khz(pwm_clock_khz, true);
    } else {
        pwm_clock_khz = clock_get_hz(clk_sys) / 1000;  // get the default clock
    }

    if (freq_playback == 0) {
        freq_playback = 44100;
    }

    stdio_init_all();

    uart_init(uart0, 115200);

    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);  // 8-N-1
    uart_set_fifo_enabled(uart0, false);

    lcd_init();

    init_i2c_kbd();

    pkk_keyboard_init();

    pkk_display_init();

    gpio_init(LEDPIN);
    gpio_set_dir(LEDPIN, GPIO_OUT);

    pkk_sound_init();
} /* pkk_init */

void pkk_reboot(void) {
    watchdog_reboot(0, 0, 0);  // Redémarrage logiciel
    while (1);                 // En attendant que le reset se produise
}

void pkk_guru_meditation(const char *str) {
    u8 blink = 0;
    lcd_clear();

    pkk_draw_text((320 - 36 * 8) / 2, 50,
                  "Guru Medidation. Press ESC to reboot", RGB565(255, 0, 0),
                  RGB565(0, 0, 0));
    pkk_draw_text((320 - strlen(str) * 8) / 2, 60, str, RGB565(255, 0, 0),
                  RGB565(0, 0, 0));

    // wait for escape key

    while (1) {
        if (pkk_key_pressed(KEY_SELECT) == 1) {  // Escape
            break;
        }
        sleep_ms(50);

        if (blink == 0) {
            pkk_draw_rect(0, 40, 320, 80, RGB565(255, 0, 0));
            pkk_draw_rect(1, 41, 319, 79, RGB565(255, 0, 0));
        } else if (blink == 8) {
            pkk_draw_rect(0, 40, 320, 80, RGB565(0, 0, 0));
            pkk_draw_rect(1, 41, 319, 79, RGB565(0, 0, 0));
        }

        blink++;
        if (blink > 16) {
            blink = 0;
        }
    }

    pkk_reboot();
} /* pkk_guru_mediation */