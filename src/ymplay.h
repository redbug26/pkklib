#include <stdint.h>

typedef struct {
    void *pMusic;
} ymcontext;

int ymplay_init(ymcontext *modctx);
void ymplay_destroy(ymcontext *modctx);
int ymplay_setcfg(ymcontext *modctx, int samplerate, int stereo_separation,
                  int filter);
int ymplay_load(ymcontext *modctx, void *mod_data, int mod_data_size);
void ymplay_fillbuffer(void *context, uint8_t *outbuffer, uint32_t nbsample);