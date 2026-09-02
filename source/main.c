#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../build/meme_raw_bin.h"


int main(void) {
    gfxInitDefault();
    romfsInit();
    
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memcpy(fb, meme_raw_bin, meme_raw_bin_size);
    

    consoleInit(GFX_TOP, NULL);
	printf("\x1b[16;15HTIENES 14?? ACTIVA CAM!!");

	printf("\x1b[30;16HPress Start to exit.");
    
    u8 *audio = NULL;
    long audioSize = 0;
    FILE *audFile = fopen("romfs:/meme_pcm.bin", "rb");
    if (audFile) {
        fseek(audFile, 0, SEEK_END);
        audioSize = ftell(audFile);   
        rewind(audFile);
        audio = linearAlloc(audioSize); 
        fread(audio, 1, audioSize, audFile);
        fclose(audFile);
        DSP_FlushDataCache(audio, audioSize);
    }   

    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, 44100.0f);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    float mix[12];
    memset(mix, 0, sizeof(mix));
    mix[0] = 1.0f;
    mix[1] = 1.0f;
    ndspChnSetMix(0, mix);

    if (audio){
    ndspWaveBuf waveBuf;
    memset(&waveBuf, 0, sizeof(waveBuf));
    waveBuf.data_vaddr = (const void *)audio;
    waveBuf.nsamples = audioSize / 4;
    waveBuf.looping = true;
    ndspChnWaveBufAdd(0, &waveBuf);
    }

    while (aptMainLoop()) {
        hidScanInput();
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        if (hidKeysDown() & KEY_START) break;

      
    }

    ndspChnWaveBufClear(0);
    ndspExit();
    linearFree(audio);
    romfsExit();
    gfxExit();
    return 0;
}   
