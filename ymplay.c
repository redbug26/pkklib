

#include "ymplay.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "libstsound/StSoundLibrary.h"

void ymplay_destroy(ymcontext *modctx) {
    if (modctx->pMusic) {
        ymMusicDestroy(modctx->pMusic);
        modctx->pMusic = NULL;
    }
}

int ymplay_init(ymcontext *modctx) {
    modctx->pMusic = ymMusicCreate();  // 44100
}

int ymplay_setcfg(ymcontext *modctx, int samplerate, int stereo_separation,
                  int filter) {
    return 0;
}

int ymplay_load(ymcontext *modctx, void *mod_data, int mod_data_size) {
    ymMusicLoadMemory(modctx->pMusic, mod_data, mod_data_size);
    ymMusicSetLoopMode(modctx->pMusic, 1);
}

static int16_t buffer16[1024 * 2];  // nbsample is max 1024

void ymplay_fillbuffer(void *context, uint8_t *outbuffer, uint32_t nbsample) {
    ymcontext *modctx = (ymcontext *)context;

    ymMusicCompute(modctx->pMusic, buffer16, nbsample);

    // convert int16_t buffer16 to uint8_t outbuffer

    int i;
    for (i = 0; i < nbsample; i++) {
        ((uint8_t *)outbuffer)[i * 2] =
            (uint8_t)(buffer16[i * 2] >> 8) + 127;  // left
        ((uint8_t *)outbuffer)[i * 2 + 1] =
            (uint8_t)(buffer16[i * 2 + 1] >> 8) + 127;  // right
    }

} /* ymplay_fillbuffer */
