/**
 * @file pkklib_system.h
 * @brief System management functions for the PicoCalc project.
 *
 * This header provides declarations for managing the PicoCalc system.
 * It is part of the pkklib library.
 *
 * @author Miguel Vanhive
 * @date 2025-05-08
 */

/**
 * @brief Initializes the PicoCalc system.
 */
void pkk_init(void);

/**
 * @brief Reboot the PicoCalc system.
 */
void pkk_reboot(void);

/**
 * @brief Sets the PWM clock frequency in kHz - To run before pkk_init
 *
 * @param clock_khz The desired PWM clock frequency in kHz (0 to set to default).
 */
void pkk_set_pwm_clock_khz(uint32_t clock_khz);

/**
 * @brief Display a guru mediation message on the LCD.
 *        More information on http://obligement.free.fr/articles/gurumeditation.php
 *
 * @param str Error message.
 */
void pkk_guru_meditation(const char *str);

