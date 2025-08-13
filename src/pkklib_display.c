#include <hardware/watchdog.h>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "i2ckbd.h"
#include "lcdspi.h"
#include "pico/stdlib.h"
#include "pkklib.h"

#define FPS 50

static uint64_t nextVSYNC = 0;
static u8 dispBattery = 0;

volatile bool vsync_flag = false;

int64_t vsync_callback(alarm_id_t id, void *user_data) {
    vsync_flag = true;
    return (1000 / FPS) * 1000;  // reschedule in 20ms
}

void pkk_display_init(void) {
    // prepare the timer for the VSYNC
    add_alarm_in_us((1000 / FPS) * 1000, vsync_callback, NULL, true);

} /* pkk_lcd_init */

void pkk_lcd_waitVSYNC(void) {
    // kbd_interrupt();
    // if (keycheck) {
    //     keycheck--;
    // }

    if (dispBattery == 0) {
        dispBattery = 255;

        // display Battery Voltage

        // if we need battery information
        // ** we should power on the picocalc only with batteries
        // ** then plug the type c cable
        // otherwise we won't get working battery infos

        char buf[64];
        int bat_pcnt = read_battery();
        if (bat_pcnt < 0) {
            return;
        }
        bat_pcnt = bat_pcnt >> 8;
        int bat_charging = bitRead(bat_pcnt, 7);
        bitClear(bat_pcnt, 7);
        sprintf(buf, "%d%%", bat_pcnt);

        pkk_draw_text(NULL, 0, 0, buf, 0xFFFF, 0x0000);

        // if (bat_charging == 0 ) {
        //     sprintf(buf, "battery is not charging\n");
        // } else {
        //     sprintf(buf, "battery is charging\n");
        // }
        // lcd_print_string(buf);

    } else {
        dispBattery--;
    }

    while (!vsync_flag) {
        tight_loop_contents();  // idle loop
    }

    vsync_flag = false;

    // if (nextVSYNC == 0) {
    //     nextVSYNC = time_us_64();
    // }

    // // wait for the next VSYNC
    // uint64_t now = time_us_64();
    // if (now < nextVSYNC) {
    //     busy_wait_us(nextVSYNC - now);
    // }
    // nextVSYNC += 20 * 1000; // 20ms for the next VSYNC (50Hz)
} /* scr_waitVSYNC */

void pkk_lcd_clear() { lcd_clear(); }
