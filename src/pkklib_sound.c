#include "hardware/pwm.h"
#include "hxcmod.h"
#include "pico/stdlib.h"  // irq_handler_t
#include "pkklib.h"
#include "ymplay.h"

#define AUDIO_PIN_L 26
#define AUDIO_PIN_R 27

void debug_addchar(char c);

// ---

#define PKK_SOUND_STREAM_SIZE 1024

uint32_t freq_playback = 44100;

static unsigned char stream[PKK_SOUND_STREAM_SIZE * 2];
static int wav_position = PKK_SOUND_STREAM_SIZE;

// pkk_setSoundCallback

static Pkklib_sound_fillbuffer pkklib_sound_fillbuffer = NULL;
static void *pkklib_sound_context = NULL;

static volatile bool in_pwm_interrupt = false;

void pkk_setFrequencyPlayback(uint32_t freq) { freq_playback = freq; }

void pkk_setSoundCallback(Pkklib_sound_fillbuffer callback, void *context) {
    while (in_pwm_interrupt) {
        // Wait for the interrupt to finish
    }

    if (callback) {
        // Set the callback function and context
        pkklib_sound_fillbuffer = callback;
        pkklib_sound_context = context;
        wav_position = PKK_SOUND_STREAM_SIZE;
    } else {
        // Reset the callback function and context
        pkklib_sound_fillbuffer = NULL;
        pkklib_sound_context = NULL;
    }
}

void pwm_interrupt_handler() {
    int slice_l = pwm_gpio_to_slice_num(AUDIO_PIN_L);
    int slice_r = pwm_gpio_to_slice_num(AUDIO_PIN_R);

    pwm_clear_irq(slice_l);
    pwm_clear_irq(slice_r);

    u8 left = 0, right = 0;

    in_pwm_interrupt = true;

    if (pkklib_sound_fillbuffer) {
        if (wav_position == PKK_SOUND_STREAM_SIZE) {
            pkklib_sound_fillbuffer(pkklib_sound_context, stream,
                                    PKK_SOUND_STREAM_SIZE);
            wav_position = 0;
        }

        left = stream[wav_position * 2];
        right = stream[wav_position * 2 + 1];

        wav_position++;
    }

    in_pwm_interrupt = false;

    pwm_set_chan_level(slice_l, PWM_CHAN_A, left);
    pwm_set_chan_level(slice_r, PWM_CHAN_B, right);

} /* pwm_interrupt_handler */

// MOD

modcontext modloaded;

void hxcmod_fillbuffer_pkk(void *context, u8 *buffer, u32 size) {
    hxcmod_fillbuffer((modcontext *)context, (msample *)buffer, size, NULL);
}

void pkk_loadSoundModule(const void *mod_data, int mod_data_size, u8 play) {
    pkk_setSoundCallback(NULL, NULL);
    pkk_stopSound();

    hxcmod_init(&modloaded);
    hxcmod_setcfg(&modloaded, freq_playback, 1, 0);
    hxcmod_load(&modloaded, (void *)mod_data, (int)mod_data_size);

    pkk_setSoundCallback(hxcmod_fillbuffer_pkk, &modloaded);

    if (play) {
        pkk_playSound();
    }
}

// AY

ymcontext ymloaded;

static u16 x = 0, y = 0;

void debug_addchar(char c) {
    char str[2];
    str[0] = c;
    str[1] = 0;

    if (c == 0) {
        y += 10;
        x = 0;
        return;
    }

    pkk_draw_text(NULL, x, y, str, RGB565(255, 255, 255), RGB565(0, 0, 0));
    x += 8;
    if (x >= 320) {
        x = 0;
        y += 10;
        if (y >= 320) {
            y = 0;
        }
    }
}

void pkk_loadSoundYM(const void *ym_data, int ym_data_size, u8 play) {
    irq_set_enabled(PWM_IRQ_WRAP, false);

    pkk_setSoundCallback(NULL, NULL);
    pkk_stopSound();

    if (ymloaded.pMusic) {
        ymplay_destroy(&ymloaded);
    }

    ymplay_init(&ymloaded);

    ymplay_setcfg(&ymloaded, freq_playback, 1, 0);

    // plante
    ymplay_load(&ymloaded, (void *)ym_data, (int)ym_data_size);

    pkk_setSoundCallback(ymplay_fillbuffer, &ymloaded);

    if (play) {
        pkk_playSound();
    }

    irq_set_enabled(PWM_IRQ_WRAP, true);
}

void pkk_stopSound(void) {
    int slice_l = pwm_gpio_to_slice_num(AUDIO_PIN_L);
    int slice_r = pwm_gpio_to_slice_num(AUDIO_PIN_R);

    pwm_set_irq_enabled(slice_l, false);
    pwm_set_irq_enabled(slice_r, false);
}

void pkk_playSound(void) {
    int slice_l = pwm_gpio_to_slice_num(AUDIO_PIN_L);
    int slice_r = pwm_gpio_to_slice_num(AUDIO_PIN_R);

    pwm_set_irq_enabled(slice_l, true);
    pwm_set_irq_enabled(slice_r, true);
}

extern uint32_t pwm_clock_khz;
extern uint32_t freq_playback;

void init_pwm(irq_handler_t my_handler) {
    gpio_set_function(AUDIO_PIN_L, GPIO_FUNC_PWM);
    gpio_set_function(AUDIO_PIN_R, GPIO_FUNC_PWM);

    int slice_l = pwm_gpio_to_slice_num(AUDIO_PIN_L);
    int slice_r = pwm_gpio_to_slice_num(AUDIO_PIN_R);

    uint32_t SYS_CLK_FREQ = pwm_clock_khz * 1000;
    int pwm_period = 1 << 8;
    float pwm_clkdiv = ((float)SYS_CLK_FREQ / freq_playback) / pwm_period;

    pwm_clear_irq(slice_l);
    pwm_clear_irq(slice_r);
    pwm_set_irq_enabled(slice_l, true);
    pwm_set_irq_enabled(slice_r, true);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, my_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, pwm_clkdiv);
    pwm_config_set_wrap(&config, pwm_period - 1);

    pwm_init(slice_l, &config, true);
    pwm_init(slice_r, &config, true);

    pwm_set_chan_level(slice_l, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_r, PWM_CHAN_B, 0);
} /* init_pwm */

void pkk_sound_init(void) { init_pwm(pwm_interrupt_handler); }