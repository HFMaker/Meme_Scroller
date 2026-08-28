#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <citro2d.h>

static u32 next_pow2(u32 n) { n--; n|=n>>1; n|=n>>2; n|=n>>4; n|=n>>8; n|=n>>16; return n+1; }
static u32 rgba_to_abgr(u32 px) {
    u8 r = (px & 0xff000000) >> 24;
    u8 g = (px & 0x00ff0000) >> 16;
    u8 b = (px & 0x0000ff00) >> 8;
    u8 a = px & 0x000000ff;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

static C2D_Image load_img(const char *path, u32 w, u32 h) {
    FILE *f = fopen(path,"rb"); if(!f) return (C2D_Image){0};
    u32 *rgba = malloc(w*h*4);
    fread(rgba,4,w*h,f); 
    fclose(f);

    C2D_Image img;
    C3D_Tex *tex = malloc(sizeof(C3D_Tex));
    img.tex = tex;
    tex->width = next_pow2(w);
    tex->height = next_pow2(h);

    Tex3DS_SubTexture *st = malloc(sizeof(Tex3DS_SubTexture));
    img.subtex = st;
    st->width=w; 
    st->height=h;
    st->left   = 0.0f;
    st->top    = 1.0f;
    st->right  = (float)w / (float)tex->width;
    st->bottom = 1.0f - ((float)h / (float)tex->height);

    C3D_TexInit(tex, tex->width, tex->height, GPU_RGBA8);
    C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);

    for(u32 y=0;y<h;y++) for(u32 x=0;x<w;x++){
        u32 s=rgba[y*w+x];
        u32 d = (((y>>3)*(tex->width>>3)+(x>>3))<<6) +
        ((y&1) | ((x&1)<<1) | ((y&2)<<1) | ((x&2)<<2) | ((y&4)<<2) | ((x&4)<<3));   
        ((u32*)tex->data)[d]=rgba_to_abgr(s);
    }
    free(rgba);
    return img;
}

int main(void) {
    gfxInitDefault();
    romfsInit();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget *bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    C2D_Image img = load_img("romfs:/meme_raw.bin", 320, 240);
    consoleInit(GFX_TOP, NULL);
	printf("\x1b[16;15HTIENES 14?? ACTIVA CAM!!");

	printf("\x1b[30;16HPress Start to exit.");
    // --- Load audio from RomFS ---
    u8 *audio = NULL;
    long audioSize = 0;
    FILE *audFile = fopen("romfs:/meme_pcm.bin", "rb");
    if (audFile) {
        fseek(audFile, 0, SEEK_END);
        audioSize = ftell(audFile);   // ← determine size FIRST
        rewind(audFile);
        audio = linearAlloc(audioSize); // ← then allocate
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
   // --- Main loop ---
    while (aptMainLoop()) {
        hidScanInput();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bot, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(bot);
        C2D_DrawImageAt(img, 0,0,0, NULL, 1.0f, 1.0f);  
        C3D_FrameEnd(0);
        if (hidKeysDown() & KEY_START) break;

      
    }

    // --- Cleanup ---
    C3D_TexDelete(img.tex); 
    free((void*)img.tex); 
    free((void*)img.subtex);
    C2D_Fini();
    C3D_Fini(); 
    ndspChnWaveBufClear(0);
    ndspExit();
    linearFree(audio);
    romfsExit();
    gfxExit();
    return 0;
}   
