/**
 * @file pkklib_display.h
 * @brief Display management functions for the PicoCalc project.
 *
 * This header provides declarations for managing the display of the PicoCalc system.
 * It is part of the pkklib library.
 *
 * @author Miguel Vanhive
 * @date 2025-05-08
 */


/**
 * @brief Waits for the vertical synchronization (VSYNC) signal.
 */
void pkk_lcd_waitVSYNC(void);

/**
 * @brief Clears the LCD screen.
 */
void pkk_lcd_clear(void);