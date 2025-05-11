/**
 * @file pkklib_keyboard.h
 * @brief Keyboard management functions for the PicoCalc project.
 *
 * This header provides declarations for managing the keyboard of the PicoCalc system.
 * It is part of the pkklib library.
 *
 * @author Miguel Vanhive
 * @date 2025-05-08
 */

/**
 * @brief Enumeration for joystick inputs emulation.
 */
typedef enum {
    KEY_A = 0,      /* Key A - ] */
    KEY_B,          /* Key B - [ */
    KEY_SELECT,     /* Select key - ESC */
    KEY_START,      /* Start key - Enter */
    KEY_UP,         /* Up key */
    KEY_DOWN,       /* Down key */
    KEY_LEFT,       /* Left key */
    KEY_RIGHT,      /* Right key */
    KEY_COUNT       /* Total number of keys */
} MY_KEY;


/**
 * @brief Tests if a specific key is pressed.
 *
 * @param key The key to test (use values from MY_KEY).
 * @return Non-zero if the key is pressed, 0 otherwise.
 */
char pkk_key_pressed(int key);

/**
 * @brief Tests if a specific key is pressed, with a wait until the key is pressed.
 *
 * @param key The key to test (use values from MY_KEY).
 * @return Non-zero if the key is pressed, 0 otherwise.
 */
char pkk_key_pressed_withWait(int key);

/**
 * @brief Checks if any key is currently pressed.
 *
 * @return Non-zero if any key is pressed, 0 otherwise.
 */
char pkk_AnyKeyPressed(void);
