#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "i2ckbd.h"
#include "lcdspi.h"
#include "pico/stdlib.h"
#include "pkklib.h"

void draw_bitmap_spi(int x1, int y1, int width, int height, int scale,
                     uint16_t fc, uint16_t bc, unsigned char *bitmap);
void draw_rect_spi(int x1, int y1, int x2, int y2, uint16_t col);
void define_region_spi(int xstart, int ystart, int xend, int yend, int rw);

extern unsigned char font1[];

// text

void pkk_putChar(u16 current_x, u16 current_y, char c, u16 fc, u16 bc) {
    unsigned char *p, *fp, *np = NULL;
    int height, width;

    // to get the +, - and = chars for font 6 we fudge them by scaling up font 1
    fp = (unsigned char *)font1;

    height = fp[1];
    width = fp[0];
    // printf("fp %d, c %d ,height %d width %d\n",fp,c, height,width);

    if (c >= fp[2] && c < fp[2] + fp[3]) {
        p = fp + 4 + (int)(((c - fp[2]) * height * width) / 8);
        np = p;

        draw_bitmap_spi(current_x, current_y, width, height, 1, fc, bc, np);
    } else {
        draw_rect_spi(current_x, current_y, current_x + width,
                      current_y + height, bc);
    }

} /* pkk_putChar */

void pkk_draw_text(u16 current_x, u16 current_y, const char *s, u16 fc,
                   u16 bc) {
    while (*s) {
        pkk_putChar(current_x, current_y, *s, fc, bc);

        current_x += 8;

        s++;
    }
}

void pkk_draw_line_vertical(int x, int y1, int y2, uint16_t col) {
    define_region_spi(x, y1, x, y2, 1);

    u16 n;
    for (n = 0; n < y2 - y1 + 1; n++) {
        spi_write_fast(Pico_LCD_SPI_MOD, (const uint8_t *)&col, 2);
    }

    spi_finish(Pico_LCD_SPI_MOD);
    lcd_spi_raise_cs();
}

void pkk_draw_line_horizontal(int x1, int x2, int y, uint16_t col) {
    define_region_spi(x1, y, x2, y, 1);

    u16 n;
    for (n = 0; n < x2 - x1 + 1; n++) {
        spi_write_fast(Pico_LCD_SPI_MOD, (const uint8_t *)&col, 2);
    }

    spi_finish(Pico_LCD_SPI_MOD);
    lcd_spi_raise_cs();
}

void pkk_draw_rect(int x1, int y1, int x2, int y2, uint16_t col) {
    pkk_draw_line_vertical(x1, y1, y2, col);
    pkk_draw_line_vertical(x2, y1, y2, col);
    pkk_draw_line_horizontal(x1, x2, y1, col);
    pkk_draw_line_horizontal(x1, x2, y2, col);

} /* draw_rect */

void pkk_draw_rect_fill(int x1, int y1, int x2, int y2, uint16_t col) {
    draw_rect_spi(x1, y1, x2, y2, col);  // TODO - replace by the function
} /* draw_rect_fill */

void pkk_draw_buf_spi(int x1, int y1, int w, int h, char *buf) {
    define_region_spi(x1, y1, x1 + w - 1, y1 + h - 1, 1);
    spi_write_fast(Pico_LCD_SPI_MOD, buf, w * h * 2);

    spi_finish(Pico_LCD_SPI_MOD);
    lcd_spi_raise_cs();

} /* draw_buf_spi */
