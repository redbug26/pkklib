#include "gif.h"

#include "lcdspi.h"

void define_region_spi(int xstart, int ystart, int xend, int yend, int rw);

struct ColorEntry {
    u8 red, green, blue;
};

u32 dwWidth;   // Buffer width in pixels
u32 dwHeight;  // Buffer height in pixels

typedef s16 (*pfctWritePixel)(u8);

pfctWritePixel WritePixel;

// ENTREE

static int gif_format;

void OpenGif(u8 *buf, int size);

s16 SkipObject(void);

s16 ReadByte(void);

s16 ReadScreenDesc(u32 *w, u32 *h, s16 *ColorRez, s16 *FillColor,
                   u16 *NumColors, struct ColorEntry ColorMap[],
                   s16 ColorMapSize);
s16 ReadImageDesc(s16 *LeftEdge, s16 *TopEdge, s16 *Width, s16 *Height,
                  s16 *Interlaced, u16 *NumColors, struct ColorEntry ColorMap[],
                  s16 ColorMapSize);

s16 read_code(void);
void init_table(s16 min_code_size);

s16 Expand_Data(void);
void DisplayPictures(u16 *pWidth, u16 *pHeight);

static int outpos;

static u8 *inbuf;
static int inpos;
static int insize;

static s16 BackdropWidth, BackdropHeight;  // size of the GIF virtual screen */
static s16 LeftEdge, TopEdge;              // coordinates of the GIF image or */
static s16 RightEdge, BottomEdge;          // text object */
static u16 DefaultNumColors;
static u16 LocalNumColors;
static s16 X, Y;  // current point on screen */
static s16 InterlacePass;
static s16 Interlaced;  // image is "interlaced" */

static struct ColorEntry DefaultColorMap[256];
static struct ColorEntry LocalColorMap[256];

static struct ColorEntry *colorMap;

static char GIFsignature[7];

static s16 BaseLine[5];
static s16 LineOffset[5];

static s16 hWidth, hHeight;

struct code_entry {
    s16 prefix;   // prefix code
    char suffix;  // suffix character
    char stack;
};

static s16 code_size;
static s16 clear_code;
static s16 eof_code;
static s16 first_free;
static s16 bit_offset;
static u16 byte_offset, bits_left;
static s16 max_code;
static s16 free_code;
static s16 old_code;
static s16 input_code;
static s16 code;
static s16 suffix_char;
static s16 final_char;
static s16 bytes_unread;
static unsigned char code_buffer[64];
static struct code_entry *code_table;

static s16 mask[12];

void InitGif(u8 *buf, int size);

void ReadBackgroundGifInfo(u32 *w, u32 *h, unsigned char *pImageFileMem,
                           int dwImageFileSize) {
    InitGif(pImageFileMem, dwImageFileSize);

    *w = dwWidth;
    *h = dwHeight;
}

void InitGif(u8 *buf, int size) {
    s16 ColorRez;  /* not used yet */
    s16 FillColor; /* color index of fill color */

    char DGIFsignature[7] = "GIF87a";

    s16 DBaseLine[5] = {0, 4, 2, 1, 0};
    s16 DLineOffset[5] = {8, 8, 4, 2, 0};

    s16 Dmask[12] = {0x001, 0x003, 0x007, 0x00F, 0x01F, 0x03F,
                     0x07F, 0x0FF, 0x1FF, 0x3FF, 0x7FF, 0xFFF};

    memcpy(GIFsignature, DGIFsignature, 7 * sizeof(char));
    memcpy(BaseLine, DBaseLine, 5 * sizeof(s16));
    memcpy(LineOffset, DLineOffset, 5 * sizeof(s16));
    memcpy(mask, Dmask, 12 * sizeof(s16));
    dwHeight = 0;
    dwWidth = 0;

    inbuf = buf;
    inpos = 0;
    insize = size;

    // Get the screen description

    if (!ReadScreenDesc(&dwWidth, &dwHeight, &ColorRez, &FillColor,
                        &DefaultNumColors, DefaultColorMap,
                        256)) {  // Invalid GIF dataset
        dwWidth = 0;
        dwHeight = 0;
        return;
    }
} /* InitGif */

void OpenGif(u8 *buf, int size) {
    s16 Width, Height;
    unsigned int Done;

    s16 Id;     /* object identifier */
    s16 Status; /* return status code */

    outpos = 0;

    Done = FALSE;

    /* Now display one or more GIF objects */

    while (!Done) {
        switch (ReadByte()) {
            case -1:  // ERREUR DANS LE GIF ! A SUPPRIMER APRES // ADD BY REDBUG
                      // ?
            case ';':  // End of the GIF dataset
                Done = TRUE;
                break;

            case ',':  // Start of an image object Read the image description.
                if (!ReadImageDesc(&LeftEdge, &TopEdge, &Width, &Height,
                                   &Interlaced, &LocalNumColors, LocalColorMap,
                                   256)) {
                    dwWidth = 0;
                    dwHeight = 0;
                    return;
                }

                /* Setup the color palette for the image */
                dwHeight = Height;
                dwWidth = Width;

                if (LocalNumColors > 0) {  // Change the palette table
                } else if (DefaultNumColors >
                           0) {  // Reset the palette table back to the default
                                 // setting
                }

                X = LeftEdge;
                Y = TopEdge;
                RightEdge = (u16)(LeftEdge + Width - 1);
                BottomEdge = (u16)(TopEdge + Height - 1);
                InterlacePass = 0;

                Status = Expand_Data();

                if (Status != 0) {  // Error expanding the raster image
                    dwWidth = 0;
                    dwHeight = 0;
                    return;
                    // Done = TRUE;
                }

                break;

            case '!':
                /* Start of an extended object (not in rev 87a) */

                Id = ReadByte(); /* Get the object identifier */

                if (Id < 0) {  // Error reading object identifier
                    dwWidth = 0;
                    dwHeight = 0;
                    return;
                    //      Done = TRUE;
                    break;
                }

                /* Since there no extented objects defined in rev 87a, we
                 * will just skip over them.  In the future, we will handle
                 * the extended object here. */

                if (!SkipObject()) Done = TRUE;

                break;

            default:  // Error
                dwWidth = 0;
                dwHeight = 0;
                return;
                // Done = TRUE;
                break;
        } /* switch */
    }
} /* OpenGif */

void init_table(s16 min_code_size) {
    code_size = (s16)(min_code_size + 1);
    clear_code = (s16)(1 << min_code_size);
    eof_code = (s16)(clear_code + 1);
    first_free = (s16)(clear_code + 2);
    free_code = first_free;
    max_code = (s16)(1 << code_size);
}

s16 read_code(void) {
    s16 bytes_to_move, i, ch;
    s32 temp;

    byte_offset = (u16)(bit_offset >> 3);
    bits_left = (u16)(bit_offset & 7);

    if (byte_offset >= 61) {
        bytes_to_move = (s16)(64 - byte_offset);

        for (i = 0; i < bytes_to_move; i++) {
            code_buffer[i] = code_buffer[byte_offset + i];
        }

        while (i < 64) {
            if (bytes_unread ==
                0) {  // Get the length of the next record. A zero-length record
                      // denotes "end of data".
                bytes_unread = ReadByte();

                if (bytes_unread < 1) {
                    if (bytes_unread == 0) /* end of data */
                        break;
                    else {
                        free((char *)code_table);
                        return (s16)(bytes_unread);
                    }
                }
            }

            ch = ReadByte();

            if (ch < 0) {
                return (s16)(ch);
            }
            code_buffer[i++] = (unsigned char)ch;
            bytes_unread--;
        }

        bit_offset = bits_left;
        byte_offset = 0;
    }

    bit_offset = (s16)(bit_offset + code_size);
    temp = (long)code_buffer[byte_offset] |
           (long)code_buffer[byte_offset + 1] << 8 |
           (long)code_buffer[byte_offset + 2] << 16;

    if (bits_left > 0) temp >>= bits_left;

    return temp & mask[code_size - 1];
} /* read_code */

/*
 * Function:
 *	Decompress a LZW compressed data stream.
 *
 * Inputs:
 *	get_byte_routine - address of the caller's "get_byte" routine.
 *	put_byte_routine - address of the caller's "put_byte" routine.
 *
 * Returns:
 *	0	OK
 *	-1	expected end-of-file
 *	-2	cannot allocate memory
 *	-3	bad "min_code_size"
 *	< -3	error status from the get_byte or put_byte routine
 */

s16 Expand_Data(void) {
    int status;
    s16 sp; /* stack ptr */
    s16 min_code_size;
    int largest_code = 4095;

    code_table = (struct code_entry *)malloc(sizeof(struct code_entry) *
                                             (largest_code + 1));

    if (code_table == NULL) {
        return -2;
    }

    /* Get the minimum code size (2 to 9) */

    min_code_size = ReadByte();

    if (min_code_size < 0) {
        free(code_table);
        return (s16)(min_code_size);
    } else if ((min_code_size < 2) || (min_code_size > 9)) {
        free(code_table);
        return (s16)(-3);
    }

    init_table(min_code_size);
    sp = 0;
    bit_offset = 64 * 8;  // force "read_code" to start a new
    bytes_unread = 0;     // record

    while ((code = (s16)read_code()) != eof_code) {
        if (code == -1) {
            break;
        }
        if (code == clear_code) {
            init_table(min_code_size);
            code = (s16)read_code();
            old_code = code;
            suffix_char = code;
            final_char = code;
            if ((status = WritePixel((unsigned char)suffix_char)) != 0) {
                free((char *)code_table);
                return (s16)(status);
            }
        } else {
            input_code = code;

            if (code >= free_code) {
                code = old_code;
                code_table[sp++].stack = (char)final_char;
            }

            while (code >= first_free) {
                code_table[sp++].stack = code_table[code].suffix;
                code = code_table[code].prefix;
            }

            final_char = code;
            suffix_char = code;
            code_table[sp++].stack = (char)final_char;

            while (sp > 0) {
                if ((status = WritePixel(code_table[--sp].stack)) != 0) {
                    free((char *)code_table);
                    return (s16)(status);
                }
            }

            code_table[free_code].suffix = (char)suffix_char;
            code_table[free_code].prefix = old_code;
            free_code++;
            old_code = input_code;

            if (free_code >= max_code) {
                if (code_size < 12) {
                    code_size++;
                    max_code <<= 1;
                }
            }
        }
        // if (code==0) break; // ADD BY REDBUG ?
    }

    free((char *)code_table);
    return 0;
} /* Expand_Data */

/*
 * Read the signature, the screen description, and the optional default
 * color map.
 */

s16 ReadScreenDesc(u32 *w, u32 *h, s16 *ColorRez, s16 *FillColor,
                   u16 *NumColors, struct ColorEntry ColorMap[],
                   s16 ColorMapSize) {
    u8 Buffer[16];
    s16 I, J, Status, HaveColorMap, NumPlanes;

    for (I = 0; I < 13; I++) {
        Status = ReadByte();
        if (Status < 0) {
            return FALSE;
        }
        Buffer[I] = (u8)Status;
    }

    gif_format = 0;

    for (I = 0; I < 6; I++) {
        if ((Buffer[I] != GIFsignature[I]) & (I != 4)) {
            return FALSE;
        }
    }

    if (Buffer[4] == '7') {
        gif_format = 87;
    }
    if (Buffer[4] == '9') {
        gif_format = 89;
    }
    if (gif_format == 0) {
        return FALSE;
    }

    *w = (u32)(Buffer[6] | Buffer[7] << 8);
    *h = (u32)(Buffer[8] | Buffer[9] << 8);
    NumPlanes = (s16)((Buffer[10] & 0x0F) + 1);

    /* bit 3 should be 0 in rev 87a */

    *ColorRez = (s16)(((Buffer[10] & 0x70) >> 4) + 1);
    HaveColorMap = (Buffer[10] & 0x80) != 0;
    *NumColors = (u16)(1 << NumPlanes);
    if (*NumColors > 256) {
        *NumColors = 256;
    }
    *FillColor = Buffer[11];
    /*  Reserved = Buffer[12]; */

    if (HaveColorMap) {
        colorMap = ColorMap;

        for (I = 0; I < *NumColors; I++) {
            for (J = 0; J < 3; J++) {
                Status = ReadByte();
                if (Status < 0) return FALSE;
                Buffer[J] = Status & 255;
            }

            if (I < ColorMapSize) {
                ColorMap[I].red = Buffer[0];    // NDS: >> 3
                ColorMap[I].green = Buffer[1];  // NDS: >> 3
                ColorMap[I].blue = Buffer[2];   // NDS: >> 3
            }
        }
    } else {
        *NumColors = 0;
    }

    return TRUE;
} /* ReadScreenDesc */

/*
 * Read the image description and the optional color map.
 */

s16 ReadImageDesc(s16 *LeftEdge, s16 *TopEdge, s16 *Width, s16 *Height,
                  s16 *Interlaced, u16 *NumColors, struct ColorEntry ColorMap[],
                  s16 ColorMapSize) {
    unsigned char Buffer[16];
    s16 I, J, NumPlanes, HaveColorMap, Status;

    for (I = 0; I < 9; I++) {
        if ((Status = (ReadByte())) < 0) return FALSE;
        Buffer[I] = (unsigned char)Status;
    }

    *LeftEdge = (s16)(Buffer[0] | Buffer[1] << 8);
    *TopEdge = (s16)(Buffer[2] | Buffer[3] << 8);
    *Width = (s16)(Buffer[4] | Buffer[5] << 8);
    *Height = (s16)(Buffer[6] | Buffer[7] << 8);

    NumPlanes = (s16)((Buffer[8] & 0x0F) + 1);
    *NumColors = (s16)(1 << NumPlanes);

    /* Bits 3, 4, and 5 should be zero (reserved) in rev 87a */

    *Interlaced = (Buffer[8] & 0x40) != 0;
    HaveColorMap = (Buffer[8] & 0x80) != 0;

    if (HaveColorMap) {
        colorMap = ColorMap;

        for (I = 0; I < *NumColors; I++) {
            for (J = 0; J < 3; J++) {
                if ((Status = (ReadByte())) < 0) return FALSE;
                Buffer[J] = (unsigned char)Status;
            }

            if (I < ColorMapSize) {
                ColorMap[I].red = Buffer[0];    // NDS >> 3
                ColorMap[I].green = Buffer[1];  // NDS >> 3
                ColorMap[I].blue = Buffer[2];   // NDS >> 3
            }
        }
    } else
        *NumColors = 0;

    return TRUE;
} /* ReadImageDesc */

/*
 * Read the next byte from the GIF data stream.
 *
 * Returns:
 *	0 .. 255	the byte
 *	-1		end of data
 *	-4		read error
 */

s16 ReadByte(void) {
    s16 a;

    if (inpos >= insize) {
        return -1;
    }

    a = inbuf[inpos];
    inpos++;

    return a;
}

/*
 * Skip over an extended GIF object.
 */

s16 SkipObject(void) {
    s16 Count;

    while ((Count = ReadByte()) > 0) do {
            if (ReadByte() < 0) { /* Error reading data stream */
                dwWidth = 0;
                dwHeight = 0;
                return FALSE;
            }
        } while (--Count > 0);

    if (Count < 0) { /* Error reading data stream */
        dwWidth = 0;
        dwHeight = 0;
        return FALSE;
    } else
        return TRUE;
}

s16 WritePixel_spi(u8 Pixel) {
    // if (outpos >= dwHeight * dwWidth) {
    //     return 1;
    // }

    if (1 == 0) {
        uint8_t rgb[3];

        rgb[0] = DefaultColorMap[Pixel].red;
        rgb[1] = DefaultColorMap[Pixel].green;
        rgb[2] = DefaultColorMap[Pixel].blue;

        spi_write_fast(Pico_LCD_SPI_MOD, rgb, 3);
    } else {
        uint16_t rgb565 = RGB565(colorMap[Pixel].red, colorMap[Pixel].green,
                                 colorMap[Pixel].blue);

        spi_write_fast(Pico_LCD_SPI_MOD, (const uint8_t *)&rgb565, 2);
    }

    // outpos++;

    return 0;
} /* WritePixel_spi */

void pkk_displayGIF(unsigned char *pImageFileMem, int dwImageFileSize) {
    WritePixel = WritePixel_spi;

    InitGif(pImageFileMem, dwImageFileSize);

    define_region_spi(0, 0, 319, 319, 1);

    OpenGif(pImageFileMem, dwImageFileSize);

    spi_finish(Pico_LCD_SPI_MOD);
    lcd_spi_raise_cs();
}
