#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define WAIT_TIMEOUT 300000000ULL

//Variables globales
u8 *audios[5];
long audioSizes[5];
int idx = 4;
char meme_messages[4][128] = {"\x1b[16;14HTIENES 14?? ACTIVA CAM!!", "\x1b[16;17HSALE BALATRITO??", "\x1b[16;14HHAPPY BIRTHDAY DANIEL!!", "\x1b[16;11HWHAT IS THIS DIDDYBLUD DOING??"};
static ndspWaveBuf waveBuf;


void writePictureToFramebufferRGB565(void *fb, void *img, u16 x, u16 y, u16 width, u16 height) {
	u8 *fb_8 = (u8*) fb;
	u16 *img_16 = (u16*) img;
	int i, j, draw_x, draw_y;
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			draw_y = y + height - j;
			draw_x = x + i;
			u32 v = (draw_y + draw_x * height) * 3;
			u16 data = img_16[j * width + i];
			uint8_t b = ((data >> 11) & 0x1F) << 3;
			uint8_t g = ((data >> 5) & 0x3F) << 2;
			uint8_t r = (data & 0x1F) << 3;
			fb_8[v] = r;
			fb_8[v+1] = g;
			fb_8[v+2] = b;
		}
	}
}
   
inline void clearScreen(){

    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memset(fb, 0, 320 * 240 * 3);

}

inline void clearTopScreen(){

    u8 *fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    memset(fb, 0, 400 * 240 * 2);

}


void printCenteredText(const char* text, int row) {
    int len = strlen(text);
    int col = (50 - len) / 2;
    if (col < 0) col = 0;
    printf("\x1b[%d;%dH%s", row, col, text);
}

void printMenuScreen(void){

    puts("\x1B[1;33mWelcome to Meme Scroller v1.1.3 by HFMaker!\x1b[0m");
    puts("");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;36mright or left D-Pad/C-Pad\x1B[0m \x1B[1;37mto scroll\nthrough memes\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m \x1B[1;37mon a meme to return to this menu\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;35mX\x1B[0m \x1B[1;37mon a meme to use the keyboard\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;31mSTART\x1B[0m \x1B[1;37mto exit\x1B[0m");

}

void printMemeGenScreen(void){

    puts("\x1B[1;37mMeme Generator mode activated!\x1b[0m");
    puts("");
    puts(""); 
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m to go back to the main menu");
    puts("");
    puts("\x1B[1;37m-Select the meme you want to use as a template:\x1B[0m");
    puts("");


}

void loadImage(int idx){

    u8 *img = malloc(320 * 240 * 3);
    if (!img){printf("Error while trying to load the image"); free(img);} 
    char path[64];
    snprintf(path, sizeof(path), "romfs:/img/meme%d.bin", idx);
    FILE *f = fopen(path, "rb");
    if (!f){printf("Image not found or path is wrong");}
    fread(img, 1, 320 * 240 * 3, f);
    fclose(f);

    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memcpy(fb, img, 320 * 240 * 3);

}


void playAudio(int idx){


    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, 44100.0f);
    ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

    memset(&waveBuf, 0, sizeof(waveBuf));
    waveBuf.data_vaddr = (const void *)audios[idx];
    waveBuf.nsamples = audioSizes[idx] / 2;
    waveBuf.looping = true;
    ndspChnWaveBufAdd(0, &waveBuf);
    DSP_FlushDataCache((u32*)audios[idx], audioSizes[idx]);

}



int main(void) {
    //Se inicializa los gráficos y el romfs
    gfxInitDefault();
    romfsInit();
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    gfxSetDoubleBuffering(GFX_TOP, false);
       
    // Parte del mensaje
    consoleInit(GFX_TOP, NULL);
    
   
    int new_idx = 4; 
    
   for (int i = 0; i < 5; i++) { //Se cargan los binarios de los audios y mientras se pone un mensaje de espera
        printCenteredText("Loading assets, please wait...", 16);
        char path[64];
        snprintf(path, sizeof(path), "romfs:/sfx/audio%d.bin", i);
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

    printMenuScreen();	

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
            printMenuScreen();    
            playAudio(idx); 
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
    
        if (hidKeysDown() & KEY_DRIGHT || hidKeysDown() & KEY_CPAD_RIGHT){
    
            idx = (idx == 4) ? 0 : (idx + 1) % 4;            

            consoleClear();
            loadImage(idx);
            playAudio(idx);


            printCenteredText(meme_messages[idx], 16);
            printf("\x1b[30;16H\x1B[1;37mPress\x1B[0m \x1B[1;31mStart\x1B[0m \x1B[1;37mto exit.\x1B[0m");

             
        }

       if (hidKeysDown() & KEY_DLEFT || hidKeysDown() & KEY_CPAD_LEFT){
            
            idx = (idx == 4) ? 3 : (idx + 3) % 4;

            consoleClear();
            loadImage(idx);
            playAudio(idx);

            
            printCenteredText(meme_messages[idx], 16);
	        printf("\x1b[30;16H\x1B[1;37mPress\x1B[0m \x1B[1;31mStart\x1B[0m \x1B[1;37mto exit.\x1B[0m");

           
           

       

        }


        if (hidKeysDown() & (KEY_L | KEY_R )){

        idx = 4;
        int selectedMemeIdx = 0;
        bool memeGenMode = true, memeGenEditor = false;
        char *memeNames[5] = {"Tienes 14 activa cam", "Sale balatrito?", "Aura monster", "Diddyblud", "Take a photo"};
       
        consoleClear();
        clearScreen();
        playAudio(idx);

        printMemeGenScreen();
        for (int i = 0; i < 5; i++) i == selectedMemeIdx ? printf("> %s\n", memeNames[i]) : puts(memeNames[i]);


        while (memeGenMode){
        
                hidScanInput();
                    
                if (hidKeysDown() & KEY_B){consoleClear(); printMenuScreen(); memeGenMode = false;}

                if (hidKeysDown() & KEY_DUP){

                    if (selectedMemeIdx != 0) --selectedMemeIdx;
                    
                    consoleClear();
                    printMemeGenScreen();
                    for (int i = 0; i < 5; i++) i == selectedMemeIdx ? printf("> %s\n", memeNames[i]) : puts(memeNames[i]);

                }

                if (hidKeysDown() & KEY_DDOWN){

                    if (selectedMemeIdx != 4) ++selectedMemeIdx;
                    
                    consoleClear();
                    printMemeGenScreen();
                    for (int i = 0; i < 5; i++) i == selectedMemeIdx ? printf("> %s\n", memeNames[i]) : puts(memeNames[i]);

                }

                if (hidKeysDown() & KEY_A){
                        
                        memeGenMode = false;
                        memeGenEditor = true;
                        consoleClear();
                        
                        if (selectedMemeIdx == 4){puts("");}

                      
                        /*u32 bufsize;
                        Handle event;

                        gfxFlushBuffers();
                        gspWaitForVBlank();
                        gfxScreenSwapBuffers(GFX_TOP, false);   

                        camInit();
                        CAMU_SetSize(SELECT_OUT1_OUT2, SIZE_CTR_TOP_LCD, CONTEXT_A);
                        CAMU_SetOutputFormat(SELECT_OUT1_OUT2, OUTPUT_RGB_565, CONTEXT_A);
                        CAMU_SetNoiseFilter(SELECT_OUT1_OUT2, true);   
                        CAMU_SetAutoExposure(SELECT_OUT1, true);
                        CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
                        CAMU_SetTrimming(PORT_CAM1, false);

                        CAMU_SetTransferBytes(PORT_BOTH, bufsize, 400, 240);
                        
                        u8 *buf = malloc(bufsize);
                        if (!buf) break;

                        
                        CAMU_Activate(SELECT_OUT1_OUT2);
                        CAMU_ClearBuffer(PORT_BOTH);
                        CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);
                        CAMU_StartCapture(PORT_BOTH); 

                        CAMU_SetReceiving(&event, buf, PORT_CAM1, 400 * 240 * 2, (s16)bufsize);
                        svcWaitSynchronization(event, WAIT_TIMEOUT);
                        svcCloseHandle(event);

                        CAMU_StopCapture(PORT_BOTH);
                        CAMU_Activate(SELECT_NONE);
                
                    
                        u8 *fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
                        writePictureToFramebufferRGB565(fb, buf, 0, 0, 400, 240);
                        free(buf);

                        }*/


                        loadImage(selectedMemeIdx);

                        while(memeGenEditor){

                            hidScanInput();

                            if (hidKeysDown() & KEY_B){

                                consoleClear();
                                clearScreen();
                                clearTopScreen();
                        

                                printMemeGenScreen();
                                for (int i = 0; i < 5; i++) i == selectedMemeIdx ? printf("> %s\n", memeNames[i]) : puts(memeNames[i]);
                                memeGenMode = true; 
                                memeGenEditor = false;


                            }


                            gfxFlushBuffers();
                            gfxScreenSwapBuffers(GFX_TOP, false);
                            gfxScreenSwapBuffers(GFX_BOTTOM, false);
                            gspWaitForVBlank();



                        }
                }
            }
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
