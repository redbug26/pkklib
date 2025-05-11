#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pkklib.h"


void ReadBackgroundGifInfo(u32 *w, u32 *h, unsigned char *from, int size);
int ReadBackgroundGif16(u16 *dest, unsigned char *from, int size);
