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



inline void clearScreen(){

    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memset(fb, 0, 320 * 240 * 3);

}

void printCenteredText(const char* text, int row) {
    int len = strlen(text);
    int col = (50 - len) / 2;
    if (col < 0) col = 0;
    printf("\x1b[%d;%dH%s", row, col, text);
}

const void *memes[]  = {meme1_raw_bin, meme2_raw_bin, meme3_raw_bin, meme4_raw_bin};
size_t meme_sizes[] = {meme1_raw_bin_size, meme2_raw_bin_size, meme3_raw_bin_size, meme4_raw_bin_size};
char meme_messages[4][128] = {"\x1b[16;14HTIENES 14?? ACTIVA CAM!!", "\x1b[16;17HSALE BALATRITO??", "\x1b[16;14HHAPPY BIRTHDAY DANIEL!!", "\x1b[16;11HWHAT IS THIS DIDDYBLUD DOING??"};

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

int idx = 4;

int main(void) {
    //Se inicializa los gráficos y el romfs
    gfxInitDefault();
    romfsInit();
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
       
    // Parte del mensaje
    consoleInit(GFX_TOP, NULL);
    
   
    int new_idx = 4; 
    
   for (int i = 0; i < 5; i++) { //Se cargan los binarios de los audios y mientras se pone un mensaje de espera
        printCenteredText("Loading assets, please wait...", 16);
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

    
    consoleClear();//Limpiamos la consola y escribimos el mensaje de bienvenida junto con los controles

	puts("\x1B[1;33mWelcome to Meme Scroller v1.1.2 by HFMaker!\x1b[0m");
    puts("");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;36mright or left D-Pad\x1B[0m \x1B[1;37mto scroll\nthrough memes\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m \x1B[1;37mon a meme to return to this menu\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;35mX\x1B[0m \x1B[1;37mon a meme to use the keyboard\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;31mSTART\x1B[0m \x1B[1;37mto exit\x1B[0m");


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

        if (hidKeysDown() & KEY_B && idx != 4){//Al darle a la B para ir "volver al menú", tenemos que redibujar todo el menú de nuevo

            consoleClear();
            clearScreen();
            idx = 4;
    
            puts("\x1B[1;33mWelcome to Meme Scroller v1.1.2 by HFMaker!\x1b[0m");
            puts("");
            puts("");
            puts("\x1B[1;37m-Press\x1B[0m \x1B[1;36mright or left D-Pad\x1B[0m \x1B[1;37mto scroll\nthrough memes\x1B[0m");
            puts("");
            puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m \x1B[1;37mon a meme to return to this menu\x1B[0m");
            puts("");
            puts("\x1B[1;37m-Press\x1B[0m \x1B[1;35mX\x1B[0m \x1B[1;37mon a meme to use the keyboard\x1B[0m");
            puts("");
            puts("\x1B[1;37m-Press\x1B[0m \x1B[1;31mSTART\x1B[0m \x1B[1;37mto exit\x1B[0m");
    
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
    
        static SwkbdState swkbd;
        static char text[128];


            
        if (hidKeysDown() & KEY_X && idx != 4){

            text[0] = '\0';
            swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, sizeof(text));
            swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT | SWKBD_DARKEN_TOP_SCREEN);
            swkbdSetHintText(&swkbd, "Enter a custom meme message...");
            swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "No, thanks", false);
            swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "I'm done", true);

            if (swkbdInputText(&swkbd, text, sizeof(text)) == SWKBD_BUTTON_RIGHT){

                        snprintf(meme_messages[idx], sizeof(meme_messages[idx]), "%s", text);

                        consoleClear();
                        printCenteredText(meme_messages[idx], 16);
                        printf("\x1b[30;16H\x1B[1;37mPress\x1B[0m \x1B[1;31mStart\x1B[0m \x1B[1;37mto exit.\x1B[0m");

                    }

        }
    
        if (hidKeysDown() & KEY_DRIGHT){
    
            idx = (idx == 4) ? 0 : (idx + 1) % 4;            

            consoleClear();
            u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
            memcpy(fb, memes[idx], meme_sizes[idx]);

            printCenteredText(meme_messages[idx], 16);
            printf("\x1b[30;16H\x1B[1;37mPress\x1B[0m \x1B[1;31mStart\x1B[0m \x1B[1;37mto exit.\x1B[0m");

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

       if (hidKeysDown() & KEY_DLEFT){
            
            idx = (idx == 4) ? 3 : (idx + 3) % 4;

            consoleClear();
            
            printCenteredText(meme_messages[idx], 16);
	        printf("\x1b[30;16H\x1B[1;37mPress\x1B[0m \x1B[1;31mStart\x1B[0m \x1B[1;37mto exit.\x1B[0m");

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
