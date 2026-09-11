#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define WAIT_TIMEOUT 300000000ULL
#define WIDTH 400
#define HEIGHT 240
#define SCREEN_SIZE WIDTH * HEIGHT * 2
#define BUF_SIZE SCREEN_SIZE * 2

//Variables globales
u8 *audios[5];
long audioSizes[5];
int idx = 4;
char meme_messages[4][128] = {"\x1b[16;14HTIENES 14?? ACTIVA CAM!!", "\x1b[16;17HSALE BALATRITO??", "\x1b[16;14HHAPPY BIRTHDAY DANIEL!!", "\x1b[16;11HWHAT IS THIS DIDDYBLUD DOING??"};
char *memeNames[5] = {"Tienes 14 activa cam", "Sale balatrito?", "Aura monster", "Diddyblud"};
int selectedMemeIdx = 0;
static ndspWaveBuf waveBuf;

   
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
    puts("\x1B[1;37m-Use\x1B[0m \x1B[1;36mright or left D-Pad/C-Pad\x1B[0m \x1B[1;37mto scroll\nthrough memes\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m \x1B[1;37mon a meme to return to this menu\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;35mX\x1B[0m \x1B[1;37mon a meme to use the keyboard\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;33mL\x1B[0m \x1B[1;37mto enter Meme Generator mode\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;31mSTART\x1B[0m \x1B[1;37mto exit\x1B[0m");

}

void printMemeGenHelpScreen(void){

    puts("\x1B[1;33mMeme Generator Help:\x1b[0m");
    puts("");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m \x1B[1;37mto quit\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;35mX\x1B[0m \x1B[1;37mto use the keyboard\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Use the\x1B[0m \x1B[1;36mTouch Screen\x1B[0m \x1B[1;37mto drag the messages\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;33mA\x1B[0m \x1B[1;37mto hide this help\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;31mY\x1B[0m \x1B[1;37mto save the image as BMP\x1B[0m");

}

void printMemeGenScreenMessage(void){

    puts("\x1B[1;33mMeme Generator mode activated!\x1b[0m");
    puts("");
    puts(""); 
    puts("\x1B[1;37m-Press\x1B[0m \x1B[1;32mB\x1B[0m to go back to the main menu");
    puts("");
    puts("\x1B[1;37m-Move with\x1B[0m \x1B[1;36mup and down D-Pad\x1B[0m");
    puts("");
    puts("\x1B[1;37m-Select with A the meme you want as a template:\x1B[0m");
    puts("");

}

void printMemeGenScreen(void){

    printMemeGenScreenMessage();
    for (int i = 0; i < 4; i++) i == selectedMemeIdx ? printf("> \x1B[1;31m%s\x1B[0m\n", memeNames[i]) : puts(memeNames[i]);
}


void loadImage(int idx){

    u8 *img = malloc(320 * 240 * 3);
    if (!img) return; 
    char path[64];
    snprintf(path, sizeof(path), "romfs:/img/meme%d.bin", idx);
    FILE *f = fopen(path, "rb");
    if (!f){free(img);}
    fread(img, 1, 320 * 240 * 3, f);
    fclose(f);

    u8 *fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memcpy(fb, img, 320 * 240 * 3);
    free(img);

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
    acInit();
    romfsInit();
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    gfxSetDoubleBuffering(GFX_TOP, true);
       
    // Parte del mensaje
    consoleInit(GFX_TOP, NULL);
    

    u8 *buf = malloc(BUF_SIZE);
    if (!buf) goto cleanup;

   
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

    
   
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_MONO);
    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, 44100.0f);
    ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

    camInit();
    CAMU_SetSize(SELECT_OUT1_OUT2, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1_OUT2, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetNoiseFilter(SELECT_OUT1_OUT2, true);   
    CAMU_SetAutoExposure(SELECT_OUT1_OUT2, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetTrimming(PORT_CAM1, false);
    CAMU_SetTrimming(PORT_CAM2, false);

    consoleClear();//Limpiamos la consola y escribimos el mensaje de bienvenida junto con los controles

    printMenuScreen();	

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


        if (hidKeysDown() & KEY_L){

        idx = 4;
        bool memeGenMode = true, memeGenEditor = false;
               
        consoleClear();
        clearScreen();
        playAudio(idx);
        printMemeGenScreen();


        while (memeGenMode){
        
                hidScanInput();
                    
                if (hidKeysDown() & KEY_B){consoleClear(); printMenuScreen(); memeGenMode = false;}

                if (hidKeysDown() & KEY_DUP){

                    if (selectedMemeIdx != 0) --selectedMemeIdx;
                    
                    consoleClear();
                    printMemeGenScreen();


                }

                if (hidKeysDown() & KEY_DDOWN){

                    if (selectedMemeIdx != 3) ++selectedMemeIdx;
                    
                    consoleClear();
                    printMemeGenScreen();
                    

                }

                if (hidKeysDown() & KEY_A){
                        
                        
                        memeGenMode = false;
                        memeGenEditor = true;
                        bool showGuide = true;
                        consoleClear();
                        loadImage(selectedMemeIdx);
                        printMemeGenHelpScreen();

                        while(memeGenEditor){

                            hidScanInput();

                            if (hidKeysHeld() & KEY_B){

                                consoleClear();
                                clearScreen();
                                clearTopScreen();
                        
                                printMemeGenScreen();

                                memeGenMode = true; 
                                memeGenEditor = false;


                            }


                            if (hidKeysDown() & KEY_A){

                                if (showGuide) {consoleClear(); showGuide = false;}
                                else if (!showGuide) {printMemeGenHelpScreen(); showGuide = true;}
                                


                            }

                            


                            gfxFlushBuffers();
                            gspWaitForVBlank();
                            gfxSwapBuffers();
                            



                        }
                }
            }
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
      
    }

    //Se limpia todo lo necesario y se sale del programa
cleanup:
    clearScreen();
    consoleClear();
    ndspChnWaveBufClear(0);
    ndspExit();
    camExit();
    for (int i = 0; i < 5; i++) if (audios[i]) linearFree(audios[i]);
    acExit();
    romfsExit();
    gfxExit();
    return 0;
}   
