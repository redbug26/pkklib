#include "main.h"

#include "pkklib.h"

int main() {
    pkk_init();

    pkk_lcd_clear();

    while (!pkk_key_pressed_withWait(KEY_SELECT)) {
        // trace un rectangle aleatoire dans la zone 0-319, 0-319 a l'aide de la
        // fonction draw_rect_spi

        int x1 = rand() % 320;
        int y1 = rand() % 320;
        int x2 = rand() % 320;
        int y2 = rand() % 320;
        u16 color = rand() % 0xFFFF;

        pkk_draw_rect_fill(x1, y1, x2, y2, color);
    }

    pkk_reboot();

    return 0;
} /* main */