#ifndef LCDSPI_H
#define LCDSPI_H
#include <hardware/spi.h>

#include "pico/multicore.h"

// #define LCD_SPI_SPEED   6000000
#define LCD_SPI_SPEED 25000000
// #define LCD_SPI_SPEED 50000000

#define Pico_LCD_SCK 10  //
#define Pico_LCD_TX 11   // MOSI
#define Pico_LCD_RX 12   // MISO
#define Pico_LCD_CS 13   //
#define Pico_LCD_DC 14
#define Pico_LCD_RST 15

#define ILI9488 1
#ifdef ILI9488
#define LCD_WIDTH 320
#define LCD_HEIGHT 320
#endif

#define PIXFMT_BGR 1

#define TFT_SLPOUT 0x11
#define TFT_INVOFF 0x20
#define TFT_INVON 0x21

#define TFT_DISPOFF 0x28
#define TFT_DISPON 0x29
#define TFT_MADCTL 0x36

#define ILI9341_MEMCONTROL 0x36
#define ILI9341_MADCTL_MX 0x40
#define ILI9341_MADCTL_BGR 0x08

#define ILI9341_COLADDRSET 0x2A
#define ILI9341_PAGEADDRSET 0x2B
#define ILI9341_MEMORYWRITE 0x2C
#define ILI9341_RAMRD 0x2E

#define ILI9341_Portrait ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR

#define ORIENT_NORMAL 0

// Pico spi0 or spi1 must match GPIO pins used above.
#define Pico_LCD_SPI_MOD spi1
#define nop asm("NOP")
// xmit_byte_multi == HW1SendSPI

#define PORTCLR 1
#define PORTSET 2
#define PORTINV 3
#define LAT 4
#define LATCLR 5
#define LATSET 6
#define LATINV 7
#define ODC 8
#define ODCCLR 9
#define ODCSET 10
#define CNPU 12
#define CNPUCLR 13
#define CNPUSET 14
#define CNPUINV 15
#define CNPD 16
#define CNPDCLR 17
#define CNPDSET 18

#define ANSELCLR -7
#define ANSELSET -6
#define ANSELINV -5
#define TRIS -4
#define TRISCLR -3
#define TRISSET -2

extern void __not_in_flash_func(spi_write_fast)(spi_inst_t *spi,
                                                const uint8_t *src, size_t len);
extern void __not_in_flash_func(spi_finish)(spi_inst_t *spi);
extern void hw_read_spi(unsigned char *buff, int cnt);
extern void hw_send_spi(const unsigned char *buff, int cnt);
extern unsigned char __not_in_flash_func(hw1_swap_spi)(unsigned char data_out);

extern void lcd_spi_raise_cs(void);
extern void lcd_spi_lower_cs(void);
extern void spi_write_data(unsigned char data);
extern void spi_write_command(unsigned char data);
extern void spi_write_cd(unsigned char command, int data, ...);
extern void spi_write_data24(uint32_t data);

extern void spi_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
extern void lcd_putc(uint8_t devn, uint8_t c);
extern int lcd_getc(uint8_t devn);
extern void lcd_sleeping(uint8_t devn);

extern char lcd_put_char(char c, int flush);
extern void lcd_print_string(char *s);

extern void lcd_spi_init();
extern void lcd_init();
extern void lcd_clear();
extern void reset_controller(void);
extern void pin_set_bit(int pin, unsigned int offset);

#endif /* ifndef LCDSPI_H */
