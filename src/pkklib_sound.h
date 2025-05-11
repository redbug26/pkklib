/**
 * @file pkklib_sound.h
 * @brief Sound management functions for the PicoCalc project.
 *
 * This header provides declarations for managing the sound of the PicoCalc system.
 * It is part of the pkklib library.
 *
 * @author Miguel Vanhive
 * @date 2025-05-08
 */

typedef void (*Pkklib_sound_fillbuffer)(void *context, u8 *buffer, u32 size);

/**
 * @brief Sets the frequency for sound playback. (to call before pkk_init)
 *
 * @param freq Frequency in Hz.
 */
void pkk_setFrequencyPlayback(uint32_t freq);

/**
 * @brief Sets the sound callback function and context.
 *
 * @param callback Pointer to the callback function. (null to stop sound)
 * @param context Pointer to the context to be passed to the callback.
 */
void pkk_setSoundCallback(Pkklib_sound_fillbuffer callback, void *context);

/**
 * @brief Loads a sound module into memory.
 *
 * @param mod_data Pointer to the sound module data.
 * @param mod_data_size Size of the sound module data in bytes.
 * @param play Non - 1 to start playback immediately, 0 to load without playing.
 */
void pkk_loadSoundModule(const void *mod_data, int mod_data_size, u8 play);

/**
 * @brief Loads a YM sound module into memory.
 *
 * @param ay_data Pointer to the YM sound module data.
 * @param ay_data_size Size of the YM sound module data in bytes.
 * @param play Non - 1 to start playback immediately, 0 to load without playing.
 */
void pkk_loadSoundYM(const void *ay_data, int ay_data_size, u8 play);

/**
 * @brief Stops the currently playing sound.
 */
void pkk_stopSound(void);

/**
 * @brief Starts playback of the loaded sound module.
 */
void pkk_playSound(void);

