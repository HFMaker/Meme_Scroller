#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../build/meme_raw_bin.h"
#include "../build/meme2_raw_bin.h"
#include "../build/meme3_raw_bin.h"
#include "../build/meme4_raw_bin.h"

u8 *audios[5];
long audioSizes[5];

int main(void) {
    //Se inicializa los gráficos y el romfs
    gfxInitDefault();
    romfsInit();
    
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memcpy(fb, meme_raw_bin, meme_raw_bin_size);
    
    // Parte del mensaje
    consoleInit(GFX_TOP, NULL);
	printf("\x1b[16;15HTIENES 14?? ACTIVA CAM!!");

	printf("\x1b[30;16HPress Start to exit.");
    
    //Se abre el binario del audio y se hacen los ajustes necesarios para que
    //este se escuche bien
    
    int new_idx = 1; 
    
   for (int i = 0; i < 5; i++) {
        char path[64];
        snprintf(path, sizeof(path), "romfs:/audio%d.bin", i);
        FILE *f = fopen(path, "rb");
        if (!f) { audioSizes[i] = 0; continue; }
        fseek(f, 0, SEEK_END);
        audioSizes[i] = ftell(f);
        rewind(f);
        audios[i] = linearAlloc(audioSizes[i]);
        fread(audios[i], 1, audioSizes[i], f);
        fclose(f);
        DSP_FlushDataCache(audios[i], audioSizes[i]);
    } 
    


    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_MONO);
    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, 44100.0f);
    ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

    float mix[12];
    memset(mix, 0, sizeof(mix));
    mix[0] = 1.0f;
    ndspChnSetMix(0, mix);

    if (audios[new_idx] && audioSizes[new_idx] > 0){
    ndspWaveBuf waveBuf;
    memset(&waveBuf, 0, sizeof(waveBuf));
    waveBuf.data_vaddr = (const void *)audios[new_idx];
    waveBuf.nsamples = audioSizes[new_idx] / 2;
    waveBuf.looping = true;
    ndspChnWaveBufAdd(0, &waveBuf);
    }

    /*void play_audio(int idx){


        ndspChnReset(0);
        ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
        ndspChnSetRate(0, 44100.0f);
        ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

        ndspWaveBuf waveBuf;
        memset(&waveBuf, 0, sizeof(waveBuf));
        waveBuf.data_vaddr = (const void *)audios[idx];
        waveBuf.nsamples = audioSizes[idx] / 2;
        waveBuf.looping = true;
        ndspChnWaveBufAdd(0, &waveBuf);

    }*/


    //Bucle principal del programa

    while (aptMainLoop()) {
        hidScanInput();

        if (hidKeysDown() & KEY_START) break;

        if (hidKeysDown() & KEY_DUP){//T14AC
            
            consoleClear();
    
	        printf("\x1b[16;15HTIENES 14?? ACTIVA CAM!!");

	        printf("\x1b[30;16HPress Start to exit.");


            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, meme_raw_bin, meme_raw_bin_size);


            ndspChnReset(0);
            ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
            ndspChnSetRate(0, 44100.0f);
            ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

            new_idx = 1; 
            ndspWaveBuf waveBuf;
            memset(&waveBuf, 0, sizeof(waveBuf));
            waveBuf.data_vaddr = (const void *)audios[new_idx];
            waveBuf.nsamples = audioSizes[new_idx] / 2;
            waveBuf.looping = true;
            ndspChnWaveBufAdd(0, &waveBuf);



          
        }

        if (hidKeysDown() & KEY_DRIGHT){//Sale balatrito?
            
            consoleClear();
	        printf("\x1b[16;17HSALE BALATRITO???");

	        printf("\x1b[30;16HPress Start to exit.");

        

            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, meme2_raw_bin, meme2_raw_bin_size);

            ndspChnReset(0);
            ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
            ndspChnSetRate(0, 44100.0f);
            ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

            new_idx = 2;
            ndspWaveBuf waveBuf;
            memset(&waveBuf, 0, sizeof(waveBuf));
            waveBuf.data_vaddr = (const void *)audios[new_idx];
            waveBuf.nsamples = audioSizes[new_idx] / 2;
            waveBuf.looping = true;
            ndspChnWaveBufAdd(0, &waveBuf);



        }

        if (hidKeysDown() & KEY_DDOWN){

            consoleClear();
	        printf("\x1b[16;14HHAPPY BIRTHDAY DANIEL!!");

	        printf("\x1b[30;16HPress Start to exit.");

        

            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, meme3_raw_bin, meme3_raw_bin_size);

            ndspChnReset(0);
            ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
            ndspChnSetRate(0, 44100.0f);
            ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

            new_idx = 3;
            ndspWaveBuf waveBuf;
            memset(&waveBuf, 0, sizeof(waveBuf));
            waveBuf.data_vaddr = (const void *)audios[new_idx];
            waveBuf.nsamples = audioSizes[new_idx] / 2;
            waveBuf.looping = true;
            ndspChnWaveBufAdd(0, &waveBuf);

        }


       if (hidKeysDown() & KEY_DLEFT){

            consoleClear();
	        printf("\x1b[16;13HWHAT IS THIS DIDDIBLUD DOING??");

	        printf("\x1b[30;16HPress Start to exit.");

        

            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, meme4_raw_bin, meme4_raw_bin_size);

            ndspChnReset(0);
            ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
            ndspChnSetRate(0, 44100.0f);
            ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

            new_idx = 4;
            ndspWaveBuf waveBuf;
            memset(&waveBuf, 0, sizeof(waveBuf));
            waveBuf.data_vaddr = (const void *)audios[new_idx];
            waveBuf.nsamples = audioSizes[new_idx] / 2;
            waveBuf.looping = true;
            ndspChnWaveBufAdd(0, &waveBuf);

        }



        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
       

      
    }

    //Se limpia todo lo necesario y se sale del programa

    ndspChnWaveBufClear(0);
    ndspExit();
    for (int i = 0; i < 5; i++) if (audios[i]) linearFree(audios[i]);
    romfsExit();
    gfxExit();
    return 0;
}   
