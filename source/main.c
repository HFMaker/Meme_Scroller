#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../build/meme1_raw_bin.h"
#include "../build/meme2_raw_bin.h"
#include "../build/meme3_raw_bin.h"
#include "../build/meme4_raw_bin.h"

u8 *audios[5];
long audioSizes[5];

/*todo: hacer que los memes se muestren desplazándote con las direcciones
 * izquierda y derecha de la cruzeta en vez de que cada meme tenga su direccion de cruzeta*/

inline void clearScreen(){

    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memset(fb, 0, 320 * 240 * 3);

}


const void *memes[]  = {meme1_raw_bin, meme2_raw_bin, meme3_raw_bin, meme4_raw_bin};
size_t meme_sizes[] = {meme1_raw_bin_size, meme2_raw_bin_size, meme3_raw_bin_size, meme4_raw_bin_size};
char *meme_messages[] = {"\x1b[16;14HTIENES 14?? ACTIVA CAM!!", "\x1b[16;17HSALE BALATRITO??", "\x1b[16;14HHAPPY BIRTHDAY DANIEL!!", "\x1b[16;11HWHAT IS THIS DIDDYBLUD DOING??"};


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


/*printf("\x1b[16;14HTIENES 14?? ACTIVA CAM!!");
printf("\x1b[16;14HHAPPY BIRTHDAY DANIEL!!");
printf("\x1b[16;17HSALE BALATRITO??");
printf("\x1b[16;11HWHAT IS THIS DIDDYBLUD DOING??");*/


int idx = 4;

int main(void) {
    //Se inicializa los gráficos y el romfs
    gfxInitDefault();
    romfsInit();
    
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
       
    // Parte del mensaje
    consoleInit(GFX_TOP, NULL);
	puts("\x1B[1;33mWelcome to Meme Scroller v1.0.0 by HFMaker!\x1b[0m");
    puts("");
    puts("");
    puts("-Press \x1B[1;36mright or left D-Pad\x1B[0m to scroll\nthrough memes");
    puts("");
    puts("-Press \x1B[1;32mB\x1B[0m while you're on a meme to\nreturn to this menu");
    puts("");
    puts("-Press \x1B[1;31mSTART\x1B[0m to exit");


    
    //Se abre el binario del audio y se hacen los ajustes necesarios para que
    //este se escuche bien
    
    int new_idx = 4; 
    
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

    

    //Bucle principal del programa

    while (aptMainLoop()) {
        hidScanInput();

        if (hidKeysDown() & KEY_START) break;

        if (hidKeysDown() & KEY_B){

            consoleClear();
            clearScreen();
            idx = 4;
    
            puts("\x1B[1;33mWelcome to Meme Scroller v1.0.0 by HFMaker!\x1b[0m");
            puts("");
            puts("");
            puts("-Press \x1B[1;36mright or left D-Pad\x1B[0m to scroll\nthrough memes");
            puts("");
            puts("-Press \x1B[1;32mB\x1B[0m while you're on a meme to\nreturn to this menu");
            puts("");
            puts("-Press \x1B[1;31mSTART\x1B[0m to exit");

    
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

        }

    
        if (hidKeysDown() & KEY_DRIGHT){

            
    
            if (idx < 3 && idx != 4) ++idx;
            else if (idx > 0 || idx == 4 ) idx = 0;


        
            
            consoleClear();
            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, memes[idx], meme_sizes[idx]);

            printf(meme_messages[idx]);
            printf("\x1b[30;16HPress \x1B[1;31mStart\x1B[0m to exit.");

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

        }

        

       if (hidKeysDown() & KEY_DLEFT){//Diddyblud
            
        
            if (idx > 0 && idx != 4) --idx;
            else if (idx == 0 || idx == 4) idx = 3;

            consoleClear();
        
            printf(meme_messages[idx]);
	        printf("\x1b[30;16HPress \x1B[1;31mStart\x1B[0m to exit.");

            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, memes[idx], meme_sizes[idx]);

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
