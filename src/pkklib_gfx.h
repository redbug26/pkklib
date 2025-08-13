/**
 * @file pkklib_gfx.h
 * @brief GFX management functions for the PicoCalc project.
 *
 * This header provides declarations for managing the GFX of the PicoCalc
 * system. It is part of the pkklib library.
 *
 * @author Miguel Vanhive
 * @date 2025-05-08
 */

/**
 * @brief Displays a GIF image on the screen.
 *
 * @param buf Pointer to the buffer containing the GIF data.
 * @param size Size of the GIF data in bytes.
 */
void pkk_displayGIF(unsigned char *buf, int size);

/**
 * @brief Displays a string on the screen at the specified position.
 *
 * @param font Pointer to the font data.
 * @param current_x X-coordinate of the starting position.
 * @param current_y Y-coordinate of the starting position.
 * @param s Pointer to the string to display.
 * @param fc Foreground color.
 * @param bc Background color.
 */
void pkk_draw_text(const u8 *font, u16 current_x, u16 current_y, const char *s,
                   u16 fc, u16 bc);

/**
 * @brief Draws a rectangle on the screen with the specified coordinates and
 * color.
 *
 * @param x1 The x-coordinate of the top-left corner of the rectangle.
 * @param y1 The y-coordinate of the top-left corner of the rectangle.
 * @param x2 The x-coordinate of the bottom-right corner of the rectangle.
 * @param y2 The y-coordinate of the bottom-right corner of the rectangle.
 * @param col The color of the rectangle, represented as a 16-bit unsigned
 * integer.
 */
void pkk_draw_rect(int x1, int y1, int x2, int y2, uint16_t col);

/**
 * @brief Draws a filled rectangle on the screen.
 *
 * This function renders a filled rectangle defined by the coordinates of its
 * top-left corner (x1, y1) and bottom-right corner (x2, y2) with the specified
 * color.
 *
 * @param x1 The x-coordinate of the top-left corner of the rectangle.
 * @param y1 The y-coordinate of the top-left corner of the rectangle.
 * @param x2 The x-coordinate of the bottom-right corner of the rectangle.
 * @param y2 The y-coordinate of the bottom-right corner of the rectangle.
 * @param col The color of the rectangle, represented as a 16-bit unsigned
 * integer.
 */
void pkk_draw_rect_fill(int x1, int y1, int x2, int y2, uint16_t col);

/**
 * @brief Draws a buffer to the screen using SPI communication.
 *
 * @param x1 X-coordinate of the top-left corner.
 * @param y1 Y-coordinate of the top-left corner.
 * @param w Width of the buffer.
 * @param h Height of the buffer.
 * @param buf Pointer to the buffer containing the pixel data.
 */
void pkk_draw_buf_spi(int x1, int y1, int w, int h, uint16_t *buf);
