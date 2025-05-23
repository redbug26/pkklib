/*-----------------------------------------------------------------------------
 *
 *       ST-Sound ( YM files player library )
 *
 *       Manage YM file depacking and parsing
 *
 *  -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * ST-Sound, ATARI-ST Music Emulator
 * Copyright (c) 1995-1999 Arnaud Carre ( http://leonard.oxg.free.fr )
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  -----------------------------------------------------------------------------*/

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "LZH.H"
#include "YmMusic.h"

extern "C" void pkk_guru_meditation(const char *str);

// Silence some lame errors from msvc
#if _MSC_VER
#define strdup _strdup          /* don't bug me about strdup being deprecated */
#pragma warning(disable : 4996) /* don't bug me about fopen */
#endif

static ymu16 ymVolumeTable[16] = {62,   161,   265,   377,  580,  774,
                                  1155, 1575,  2260,  3088, 4570, 6233,
                                  9330, 13187, 21220, 32767};

static void signeSample(ymu8 *ptr, yms32 size) {
    if (size > 0) {
        do {
            *ptr++ ^= 0x80;
        } while (--size);
    }
}

ymu32 readMotorolaDword(ymu8 **ptr) {
    ymu32 n;
    ymu8 *p = *ptr;

    n = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    p += 4;
    *ptr = p;
    return n;
}

ymu16 readMotorolaWord(ymu8 **ptr) {
    ymu16 n;
    ymu8 *p = *ptr;

    n = (p[0] << 8) | p[1];
    p += 2;
    *ptr = p;
    return n;
}

ymchar *readNtString(ymchar **ptr) {
    ymchar *p;

    p = strdup(*ptr);
    (*ptr) += strlen(*ptr) + 1;
    return p;
}

yms32 ReadLittleEndian32(ymu8 *pLittle) {
    yms32 v = ((pLittle[0] << 0) | (pLittle[1] << 8) | (pLittle[2] << 16) |
               (pLittle[3] << 24));

    return v;
}

yms32 ReadBigEndian32(ymu8 *pBig) {
    yms32 v =
        ((pBig[0] << 24) | (pBig[1] << 16) | (pBig[2] << 8) | (pBig[3] << 0));

    return v;
}

unsigned char *CYmMusic::depackFile(ymu32 checkOriginalSize) {
    lzhHeader_t header;
    ymu8 *pNew;
    ymu8 *pSrc;

    header.size = (ymu8)pBigMalloc[0];
    header.sum = (ymu8)pBigMalloc[1];
    header.id[0] = (ymu8)pBigMalloc[2];
    header.id[1] = (ymu8)pBigMalloc[3];
    header.id[2] = (ymu8)pBigMalloc[4];
    header.id[3] = (ymu8)pBigMalloc[5];
    header.id[4] = (ymu8)pBigMalloc[6];
    header.packed = pBigMalloc[7] << 0 | pBigMalloc[8] << 8 |
                    pBigMalloc[9] << 16 | pBigMalloc[10] << 24;  // litle endian
    header.original = pBigMalloc[11] << 0 | pBigMalloc[12] << 8 |
                      pBigMalloc[13] << 16 |
                      pBigMalloc[14] << 24;  // litle endian
    header.reserved[0] = (ymu8)pBigMalloc[15];
    header.reserved[1] = (ymu8)pBigMalloc[16];
    header.reserved[2] = (ymu8)pBigMalloc[17];
    header.reserved[3] = (ymu8)pBigMalloc[18];
    header.reserved[4] = (ymu8)pBigMalloc[19];
    header.level =
        (ymu8)pBigMalloc[20];  // 0: non-extended header, 1, 2 = extended header
    header.name_lenght = (ymu8)pBigMalloc[21];
    //		pHeader = (lzhHeader_t*)pBigMalloc;

    //		if ((pHeader->size==0) ||
    //// NOTE: Endianness works because value is 0
    //			(strncmp(pHeader->id,"-lh5-",5)))
    if ((header.size == 0) ||
        (strncmp(
            header.id, "-lh5-",
            5))) {  // Le fichier n'est pas compresse, on retourne l'original.
        return pBigMalloc;
    }

    fileSize = (ymu32)-1;

    if (header.level > 1) {  // Compression LH5, header > 1 : Error.
        free(pBigMalloc);
        pBigMalloc = NULL;
        pkk_guru_meditation("LHARC Header must be 0 or 1 !");
        return NULL;
    }

    //		fileSize = ReadLittleEndian32((ymu8*)&pHeader->original);
    fileSize = header.original;
    pNew = (ymu8 *)malloc(fileSize);
    if (!pNew) {
        pkk_guru_meditation("MALLOC Failed !");
        free(pBigMalloc);
        pBigMalloc = NULL;
        return NULL;
    }

    //		pSrc = pBigMalloc+sizeof(lzhHeader_t)+pHeader->name_lenght;
    //// NOTE: Endianness works because name_lenght is a byte
    pSrc = pBigMalloc + 22 + header.name_lenght;

    pSrc += 2;  // skip CRC16

    if (header.level ==
        1) {     // https://github.com/jca02266/lha/blob/master/header.doc.md
        pSrc++;  // skip os-type

        ymu16 nextHeaderSize;

        do {
            nextHeaderSize = pSrc[0] << 0 | pSrc[1] << 8;
            pSrc += 2;
            pSrc += nextHeaderSize;
        } while (nextHeaderSize != 0);
    }

    //		const int		packedSize =
    // ReadLittleEndian32((ymu8*)&pHeader->packed);
    ymu32 packedSize = header.packed;

    checkOriginalSize -= ymu32(pSrc - pBigMalloc);

    if (packedSize > checkOriginalSize) packedSize = checkOriginalSize;

    // Check for corrupted archive
    if (packedSize <= checkOriginalSize) {
        // alloc space for depacker and depack data
        CLzhDepacker *pDepacker = new CLzhDepacker;
        const bool bRet = pDepacker->LzUnpack(pSrc, packedSize, pNew, fileSize);
        delete pDepacker;

        if (!bRet) {  // depacking error
            pkk_guru_meditation("LH5 Depacking Error !");
            free(pNew);
            pNew = NULL;
        }
    } else {
        pkk_guru_meditation("LH5 Depacking Error !");
        free(pNew);
        pNew = NULL;
    }

    // Free up source buffer, whatever depacking fail or success
    free(pBigMalloc);

    return pNew;
}  // CYmMusic::depackFile

static ymint fileSizeGet(FILE *h) {
    ymint size;
    ymint old;

    old = ftell(h);
    fseek(h, 0, SEEK_END);
    size = ftell(h);
    fseek(h, old, SEEK_SET);
    return size;
}

ymbool CYmMusic::deInterleave(void) {
    yms32 nextPlane[32];
    ymu8 *pW, *tmpBuff;
    yms32 j, k;

    if (attrib & A_STREAMINTERLEAVED) {
        pkk_guru_meditation("STREAM INTERLEAVED not supported");
        return YMFALSE;
    }

    if (1 == 0) {
        tmpBuff = (ymu8 *)malloc(nbFrame * streamInc);
        if (!tmpBuff) {
            pkk_guru_meditation("Malloc error in deInterleave()\n");
            return YMFALSE;
        }

        // Precalcul les offsets.
        for (j = 0; j < streamInc; j++) {
            nextPlane[j] = nbFrame * j;
        }

        pW = tmpBuff;
        for (j = 0; j < nextPlane[1]; j++) {
            for (k = 0; k < streamInc; k++) {
                pW[k] = pDataStream[j + nextPlane[k]];
            }
            pW += streamInc;
        }

        free(pBigMalloc);

        pBigMalloc = tmpBuff;
        pDataStream = tmpBuff;

        attrib &= (~A_STREAMINTERLEAVED);
    }

    return YMTRUE;
}  // CYmMusic::deInterleave

enum {
    e_YM2a = ('Y' << 24) | ('M' << 16) | ('2' << 8) | ('!'),  // 'YM2!'
    e_YM3a = ('Y' << 24) | ('M' << 16) | ('3' << 8) | ('!'),  // 'YM3!'
    e_YM3b = ('Y' << 24) | ('M' << 16) | ('3' << 8) | ('b'),  // 'YM3b'
    e_YM4a = ('Y' << 24) | ('M' << 16) | ('4' << 8) | ('!'),  // 'YM4!'
    e_YM5a = ('Y' << 24) | ('M' << 16) | ('5' << 8) | ('!'),  // 'YM5!'
    e_YM6a = ('Y' << 24) | ('M' << 16) | ('6' << 8) | ('!'),  // 'YM6!'
    e_MIX1 = ('M' << 24) | ('I' << 16) | ('X' << 8) | ('1'),  // 'MIX1'
    e_YMT1 = ('Y' << 24) | ('M' << 16) | ('T' << 8) | ('1'),  // 'YMT1'
    e_YMT2 = ('Y' << 24) | ('M' << 16) | ('T' << 8) | ('2'),  // 'YMT2'
};

ymbool CYmMusic::ymDecode(void) {
    ymu8 *pUD;
    ymu8 *ptr;
    ymint skip;
    ymint i;
    ymu32 sampleSize;
    yms32 tmp;
    ymu32 id;

    id = ReadBigEndian32((unsigned char *)pBigMalloc);
    switch (id) {
        case e_YM2a:  // 'YM2!':		// MADMAX specific.

            songType = YM_V2;
            nbFrame = (fileSize - 4) / 14;
            loopFrame = 0;
            ymChip.setClock(ATARI_CLOCK);
            setPlayerRate(50);
            pDataStream = pBigMalloc + 4;
            streamInc = 14;
            nbDrum = 0;
            setAttrib(A_STREAMINTERLEAVED | A_TIMECONTROL);
            pSongName = strdup("Unknown");
            pSongAuthor = strdup("Unknown");
            pSongComment = strdup("Converted by Leonard.");
            pSongType = strdup("YM 2");
            pSongPlayer = strdup("YM-Chip driver");
            break;

        case e_YM3a:  // 'YM3!':		// Standart YM-Atari format.

            songType = YM_V3;
            nbFrame = (fileSize - 4) / 14;
            loopFrame = 0;
            ymChip.setClock(ATARI_CLOCK);
            setPlayerRate(50);
            pDataStream = pBigMalloc + 4;
            streamInc = 14;
            nbDrum = 0;
            setAttrib(A_STREAMINTERLEAVED | A_TIMECONTROL);
            pSongName = strdup("Unknown");
            pSongAuthor = strdup("Unknown");
            pSongComment = strdup("");
            pSongType = strdup("YM 3");
            pSongPlayer = strdup("YM-Chip driver");
            break;

        case e_YM3b:  // 'YM3b':		// Standart YM-Atari format +
                      // Loop info.

            pUD = (ymu8 *)(pBigMalloc + fileSize - 4);
            songType = YM_V3;
            nbFrame = (fileSize - 4) / 14;
            loopFrame = ReadLittleEndian32(pUD);
            ymChip.setClock(ATARI_CLOCK);
            setPlayerRate(50);
            pDataStream = pBigMalloc + 4;
            streamInc = 14;
            nbDrum = 0;
            setAttrib(A_STREAMINTERLEAVED | A_TIMECONTROL);
            pSongName = strdup("Unknown");
            pSongAuthor = strdup("Unknown");
            pSongComment = strdup("");
            pSongType = strdup("YM 3b (loop)");
            pSongPlayer = strdup("YM-Chip driver");
            break;

        case e_YM4a:  // 'YM4!':		// Extended ATARI format.

            pkk_guru_meditation("No more YM4! support. Use YM5! format.");
            return YMFALSE;
            break;

        case e_YM5a:  // 'YM5!':		// Extended YM2149 format, all
                      // machines.
        case e_YM6a:  // 'YM6!':		// Extended YM2149 format, all
                      // machines.

            if (strncmp((const char *)(pBigMalloc + 4), "LeOnArD!", 8)) {
                pkk_guru_meditation("Not a valid YM format !");
                return YMFALSE;
            }

            ptr = pBigMalloc + 12;
            nbFrame = readMotorolaDword(&ptr);
            setAttrib(readMotorolaDword(&ptr));
            nbDrum = readMotorolaWord(&ptr);
            ymChip.setClock(readMotorolaDword(&ptr));
            setPlayerRate(readMotorolaWord(&ptr));
            loopFrame = readMotorolaDword(&ptr);
            skip = readMotorolaWord(&ptr);
            ptr += skip;
            if (nbDrum > 0) {
                pDrumTab = (digiDrum_t *)malloc(nbDrum * sizeof(digiDrum_t));
                for (i = 0; i < nbDrum; i++) {
                    pDrumTab[i].size = readMotorolaDword(&ptr);
                    if (pDrumTab[i].size) {
                        pDrumTab[i].pData = (ymu8 *)malloc(pDrumTab[i].size);

                        memcpy(pDrumTab[i].pData, ptr, pDrumTab[i].size);
                        if (attrib & A_DRUM4BITS) {
                            ymu32 j;
                            ymu8 *pw = pDrumTab[i].pData;
                            for (j = 0; j < pDrumTab[i].size; j++) {
                                *pw = ymVolumeTable[(*pw) & 15] >> 7;
                                pw++;
                            }
                        }
                        ptr += pDrumTab[i].size;
                    } else {
                        pDrumTab[i].pData = NULL;
                    }
                }
                attrib &= (~A_DRUM4BITS);
            }
            pSongName = readNtString((char **)&ptr);
            pSongAuthor = readNtString((char **)&ptr);
            pSongComment = readNtString((char **)&ptr);
            songType = YM_V5;
            if (id == e_YM6a) {  // 'YM6!')
                songType = YM_V6;
                pSongType = strdup("YM 6");
            } else {
                pSongType = strdup("YM 5");
            }
            pDataStream = ptr;
            streamInc = 16;
            pSongPlayer = strdup("YM-Chip driver");
            break;

        case e_MIX1:  // 'MIX1':		// ATARI Remix digit format.

            if (strncmp((const char *)(pBigMalloc + 4), "LeOnArD!", 8)) {
                pkk_guru_meditation("Not a valid YM format !");
                return YMFALSE;
            }
            ptr = pBigMalloc + 12;
            songType = YM_MIX1;
            tmp = readMotorolaDword(&ptr);
            setAttrib(0);
            if (tmp & 1) setAttrib(A_DRUMSIGNED);
            sampleSize = readMotorolaDword(&ptr);
            nbMixBlock = readMotorolaDword(&ptr);
            pMixBlock = (mixBlock_t *)malloc(nbMixBlock * sizeof(mixBlock_t));
            for (i = 0; i < nbMixBlock; i++) {  // Lecture des block-infos.
                pMixBlock[i].sampleStart = readMotorolaDword(&ptr);
                pMixBlock[i].sampleLength = readMotorolaDword(&ptr);
                pMixBlock[i].nbRepeat = readMotorolaWord(&ptr);
                pMixBlock[i].replayFreq = readMotorolaWord(&ptr);
            }
            pSongName = readNtString((char **)&ptr);
            pSongAuthor = readNtString((char **)&ptr);
            pSongComment = readNtString((char **)&ptr);

            pBigSampleBuffer = (unsigned char *)malloc(sampleSize);
            memcpy(pBigSampleBuffer, ptr, sampleSize);

            if (!(attrib & A_DRUMSIGNED)) {
                signeSample(pBigSampleBuffer, sampleSize);
                setAttrib(A_DRUMSIGNED);
            }

            setAttrib(getAttrib() | A_TIMECONTROL);
            computeTimeInfo();

            mixPos = -1;  // numero du block info.
            currentPente = 0;
            currentPos = 0;
            pSongType = strdup("MIX1");
            pSongPlayer = strdup("Digi-Mix driver");

            break;

        case e_YMT1:  // 'YMT1':		// YM-Tracker
        case e_YMT2:  // 'YMT2':		// YM-Tracker
                      /*;
                       * ; Format du YM-Tracker-1
                       * ;
                       * ; 4  YMT1
                       * ; 8  LeOnArD!
                       * ; 2  Nb voice
                       * ; 2  Player rate
                       * ; 4  Music lenght
                       * ; 4  Music loop
                       * ; 2  Nb digidrum
                       * ; 4  Flags		; Interlace, signed, 8 bits, etc...
                       * ; NT Music Name
                       * ; NT Music author
                       * ; NT Music comment
                       * ; nb digi *
                       */

            if (strncmp((const char *)(pBigMalloc + 4), "LeOnArD!", 8)) {
                pkk_guru_meditation("Not a valid YM format !");
                return YMFALSE;
            }
            ptr = pBigMalloc + 12;
            songType = YM_TRACKER1;
            nbVoice = readMotorolaWord(&ptr);
            setPlayerRate(readMotorolaWord(&ptr));
            nbFrame = readMotorolaDword(&ptr);
            loopFrame = readMotorolaDword(&ptr);
            nbDrum = readMotorolaWord(&ptr);
            attrib = readMotorolaDword(&ptr);
            pSongName = readNtString((char **)&ptr);
            pSongAuthor = readNtString((char **)&ptr);
            pSongComment = readNtString((char **)&ptr);
            if (nbDrum > 0) {
                pDrumTab = (digiDrum_t *)malloc(nbDrum * sizeof(digiDrum_t));
                for (i = 0; i < (ymint)nbDrum; i++) {
                    pDrumTab[i].size = readMotorolaWord(&ptr);
                    pDrumTab[i].repLen = pDrumTab[i].size;
                    if (e_YMT2 == id) {  // ('YMT2' == id)
                        pDrumTab[i].repLen = readMotorolaWord(&ptr);  // repLen
                        readMotorolaWord(&ptr);                       // flag
                    }
                    if (pDrumTab[i].repLen > pDrumTab[i].size) {
                        pDrumTab[i].repLen = pDrumTab[i].size;
                    }

                    if (pDrumTab[i].size) {
                        pDrumTab[i].pData = (ymu8 *)malloc(pDrumTab[i].size);
                        memcpy(pDrumTab[i].pData, ptr, pDrumTab[i].size);
                        ptr += pDrumTab[i].size;
                    } else {
                        pDrumTab[i].pData = NULL;
                    }
                }
            }

            ymTrackerFreqShift = 0;
            if (e_YMT2 == id) {  // ('YMT2' == id)
                ymTrackerFreqShift = (attrib >> 28) & 15;
                attrib &= 0x0fffffff;
                pSongType = strdup("YM-T2");
            } else {
                pSongType = strdup("YM-T1");
            }

            pDataStream = ptr;
            ymChip.setClock(ATARI_CLOCK);

            ymTrackerInit(100);  // 80% de volume maxi.
            streamInc = 16;
            setTimeControl(YMTRUE);
            pSongPlayer = strdup("Universal Tracker");
            break;

        default:
            pkk_guru_meditation("Unknow YM format !");
            return YMFALSE;
            break;
    }  // switch

    if (!deInterleave()) {
        return YMFALSE;
    }

    return YMTRUE;
}  // CYmMusic::ymDecode

ymbool CYmMusic::checkCompilerTypes() {
    ymbool result = YMTRUE;

    if (1 != sizeof(ymu8)) result = YMFALSE;
    if (1 != sizeof(yms8)) result = YMFALSE;
    if (1 != sizeof(ymchar)) result = YMFALSE;

    if (2 != sizeof(ymu16)) result = YMFALSE;
    if (2 != sizeof(yms16)) result = YMFALSE;
    if (4 != sizeof(ymu32)) result = YMFALSE;
    if (4 != sizeof(yms32)) result = YMFALSE;

    if (2 != sizeof(ymsample)) result = YMFALSE;

#ifdef YM_INTEGER_ONLY
    if (8 != sizeof(yms64)) result = YMFALSE;
#endif

    if (sizeof(ymint) < 4) result = YMFALSE;  // ymint should be at least 32bits

    if (result == YMFALSE) {
        pkk_guru_meditation("Compiler types are not correct !");
    }
    return result;
}  // CYmMusic::checkCompilerTypes

ymbool CYmMusic::loadMemory(void *pBlock, ymu32 size) {
    stop();
    unLoad();

    if (!checkCompilerTypes()) return YMFALSE;

    // ---------------------------------------------------
    // Allocation d'un buffer pour lire le fichier.
    // ---------------------------------------------------
    fileSize = size;
    pBigMalloc = (unsigned char *)malloc(fileSize);
    if (!pBigMalloc) {
        pkk_guru_meditation("MALLOC Error");
        return YMFALSE;
    }

    // ---------------------------------------------------
    // Chargement du fichier complet.
    // ---------------------------------------------------
    memcpy(pBigMalloc, pBlock, size);

    // ---------------------------------------------------
    // Transforme les données en données valides.
    // ---------------------------------------------------
    pBigMalloc = depackFile(size);
    if (!pBigMalloc) {
        pkk_guru_meditation("Error in depackFile");
        return YMFALSE;
    }

    // ---------------------------------------------------
    // Lecture des données YM:
    // ---------------------------------------------------
    if (!ymDecode()) {
        free(pBigMalloc);

        pBigMalloc = NULL;
        return YMFALSE;
    }

    ymChip.reset();

    bMusicOk = YMTRUE;
    bPause = YMFALSE;
    return YMTRUE;
}  // CYmMusic::loadMemory

void myFree(void **pPtr) {
    if (*pPtr) free(*pPtr);
    *pPtr = NULL;
}

void CYmMusic::unLoad(void) {
    bMusicOk = YMFALSE;
    bPause = YMTRUE;
    bMusicOver = YMFALSE;
    myFree((void **)&pSongName);
    myFree((void **)&pSongAuthor);
    myFree((void **)&pSongComment);
    myFree((void **)&pSongType);
    myFree((void **)&pSongPlayer);
    myFree((void **)&pBigMalloc);
    if (nbDrum > 0) {
        for (ymint i = 0; i < nbDrum; i++) {
            myFree((void **)&pDrumTab[i].pData);
        }
        nbDrum = 0;
        myFree((void **)&pDrumTab);
    }
    myFree((void **)&pBigSampleBuffer);
    myFree((void **)&pMixBlock);

    myFree((void **)&m_pTimeInfo);

}  // CYmMusic::unLoad

void CYmMusic::stop(void) {
    bPause = YMTRUE;
    currentFrame = 0;
    m_iMusicPosInMs = 0;
    m_iMusicPosAccurateSample = 0;
    mixPos = -1;
}

void CYmMusic::play(void) { bPause = YMFALSE; }

void CYmMusic::pause(void) { bPause = YMTRUE; }
