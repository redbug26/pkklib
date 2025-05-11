#include "pkklib.h"

#include "pico/stdlib.h"
#include <hardware/watchdog.h>

#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "i2ckbd.h"
#include "lcdspi.h"

void kbd_interrupt();

// Keyboard I2C

static uint8_t keycheck = 0;
static uint8_t keyread = 0;
static int joystick_pins[KEY_COUNT] = {0};  // Joystick pins emulation

static uint TICKSPERSEC = 1000;
static uint8_t KEYCHECKTIME = 16;

static u8 frkey[KEY_COUNT];  // key pressed with wait


static absolute_time_t now;

static void __attribute__ ((optimize("-Os"))) __not_in_flash_func(timer_tick_cb)(unsigned alarm) {

    absolute_time_t next;

    update_us_since_boot(&next, to_us_since_boot(now) + ( TICKSPERSEC));
    if (hardware_alarm_set_target(0, next)) {
        update_us_since_boot(&next, time_us_64() + ( TICKSPERSEC));
        hardware_alarm_set_target(0, next);
    }

    kbd_interrupt();
    if (keycheck) {
        keycheck--;
    }

}

void pkk_keyboard_init(void)
{
    // prepare the timer for the keyboard
    hardware_alarm_claim(0);
    update_us_since_boot(&now, time_us_64());
    hardware_alarm_set_callback(0, timer_tick_cb);
    hardware_alarm_force_irq(0);
}



// keyboard key status to joystick_pins map
void set_kdb_key(uint8_t pin_offset, uint8_t key_status)
{
    if (key_status == 1) {
        joystick_pins[pin_offset] = 1;
    } else if (key_status == 3) {
        joystick_pins[pin_offset] = 0;
    }
}

void kbd_interrupt()
{
    int kbd_ret = -1;
    int c;
    static int ctrlheld = 0;
    uint8_t key_stat = 0; // press,release, or hold

    if (keycheck == 0) {
        if (keyread == 0) {
            kbd_ret = write_i2c_kbd();
            keyread = 1;
        } else {
            kbd_ret = read_i2c_kbd();
            keyread = 0;
        }
        keycheck = KEYCHECKTIME; // we check keys every 16 frames
    }

    if (kbd_ret < 0) {
        if (check_if_failed() > 0) {
            // printf("try to reset i2c\n");
            reset_failed();
            init_i2c_kbd(); // re-init
        }
    }

    if (kbd_ret) {

        // char str[20];
        // sprintf(str, "%04X ", kbd_ret);
        // lcd_print_string(str);

        if (kbd_ret == 0xA503)
            ctrlheld = 0;
        else if (kbd_ret == 0xA502) {
            ctrlheld = 1;
        } else if ((kbd_ret & 0xff) == 1) { // pressed
            key_stat = 1;
        } else if ((kbd_ret & 0xff) == 3) {
            key_stat = 3;
        }

        c = kbd_ret >> 8;
        int realc = -1;
        switch (c) {
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
                realc = -1; // skip shift alt ctrl keys
                break;
            default:
                realc = c;
                break;
        }

        c = realc;
        if (c >= 'a' && c <= 'z' && ctrlheld)
            c = c - 'a' + 1;

        switch (c) {

            // scanCode in https://github.com/clockworkpi/PicoCalc/blob/master/Code/picocalc_keyboard/keyboard.h

            // switch (kbd_ret) {
            case 0xb5: // UP
                set_kdb_key(KEY_UP, key_stat);
                break;
            case 0xb6: // DOWN
                set_kdb_key(KEY_DOWN, key_stat);
                break;
            case 0xb4: // LEFT
                set_kdb_key(KEY_LEFT, key_stat);
                break;
            case 0xb7: // RIGHT
                set_kdb_key(KEY_RIGHT, key_stat);
                break;
            case 0xb1: // select - ESCAPE
                set_kdb_key(KEY_SELECT, key_stat);
                break;
            case 0x0a: // start
                set_kdb_key(KEY_START, key_stat);
                break;
            case '[': // B
                set_kdb_key(KEY_B, key_stat);
                break;
            case ']': // A
                set_kdb_key(KEY_A, key_stat);
                break;

            case 0xC1: // CAPS_LOCK
                if (ctrlheld) {
                    watchdog_reboot(0, 0, 0);
                }
                break;

            default:
                break;
        } /* switch */
    }
} /* kbd_interrupt */






char pkk_key_pressed(int key)
{
    return joystick_pins[key];
}


char pkk_key_pressed_withWait(int key)
{
    if (pkk_key_pressed(key)) {
        if (frkey[key] == 0) {
            frkey[key] = 20;
            return 1;
        } else {
            frkey[key]--;
        }
    } else {
        frkey[key] = 0;
    }
    return 0;
}

char pkk_AnyKeyPressed(void)
{
    for (int i = 0; i < KEY_COUNT; i++) {
        if (joystick_pins[i]) {
            return 1;
        }
    }
    return 0;
} /* pkk_AnyKeyPressed */