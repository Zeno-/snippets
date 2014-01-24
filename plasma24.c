/* A plasma-like effect.
 * By CDR - September 2013
 * Rot13 Email: xqr.cflpu ng tznvy.pbz
 *
 * gcc -O3 -lm -lSDL filename.c
 */
#include <SDL/SDL.h>
#include <SDL/SDL_main.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI_OVER_180 (0.01745329252)
#define DEG_TO_RAD(d) ((d)*PI_OVER_180)

#define OUT_WIDTH  800
#define OUT_HEIGHT 600

#define MAX_SHIFT 256

#define OFFSET_MAG  (MAX_SHIFT/2)

#define PALETTE_SIZE (OUT_HEIGHT + MAX_SHIFT)

#define INTER_WIDTH  (OUT_WIDTH + MAX_SHIFT)
#define INTER_HEIGHT (OUT_HEIGHT)

#define TARGET_FPS 50

struct fpsctx {
    uint32_t prevtick;
    uint16_t tickInterval;
};

static SDL_Surface* surface;
static SDL_Surface* logo;

static uint8_t palette1[PALETTE_SIZE];

static uint8_t *intermediateR;
static uint8_t *intermediateG;
static uint8_t *intermediateB;

static int16_t offsetTable[512];
static struct fpsctx fpstimer;

bool init(void);
void cleanup(void);
bool processEvents(void);
void drawPlasma(SDL_Surface *surface);
void drawLogo(SDL_Surface *surface, const SDL_Surface *logo);
void initfpstimer(struct fpsctx* t, int fpslimit);
void limitfps(struct fpsctx* t);


/*
 * TODO: Clean this mess up
 */


int main (void)
{
    if (init()) {
        while(!processEvents()) {
            drawPlasma(surface);
            SDL_Flip(surface);
            limitfps(&fpstimer);
        }
    }

    cleanup();

    return 0;
}

bool init(void)
{
    if (OUT_HEIGHT < OFFSET_MAG) {
        puts("Output screen/window size is too small.");
        return false;
    }

    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    atexit(SDL_Quit);
    surface = SDL_SetVideoMode(OUT_WIDTH, OUT_HEIGHT, 32,
                               SDL_HWSURFACE | SDL_DOUBLEBUF);
                               //| SDL_FULLSCREEN);
    if (!surface) return false;
    SDL_LockSurface(surface);

    // Intermediate pixel destinations
    intermediateR = malloc(INTER_WIDTH * INTER_HEIGHT * sizeof(*intermediateR));
    if (!intermediateR) return false;

    intermediateG = malloc(INTER_WIDTH * INTER_HEIGHT * sizeof(*intermediateG));
    if (!intermediateG) return false;
    
    intermediateB = malloc(INTER_WIDTH * INTER_HEIGHT * sizeof(*intermediateB));
    if (!intermediateR) return false;
    
    unsigned i;

    // init palettes

    const int max_colour = 255;
    const int min_colour = 0;
    double step = (double)(max_colour - min_colour) / (PALETTE_SIZE / 2);

    for (i = 0; i < PALETTE_SIZE / 2; i++) {

        int b = min_colour + ceil(i * step);

        if (b > max_colour) b = max_colour;

        palette1[i] = b;
        palette1[PALETTE_SIZE - i - 1] = b;

    }

    // void init_offsetTable(void)
    unsigned len = sizeof offsetTable / sizeof offsetTable[0];
    for (i = 0; i < len; i++) {
        offsetTable[i] = sin(DEG_TO_RAD((double)i / len * 360.0)) * OFFSET_MAG;
    }

    // set target fps
    initfpstimer(&fpstimer, TARGET_FPS);

    return true;
}

void cleanup(void)
{
    free(intermediateR);
    free(intermediateG);
    free(intermediateB);
    
    SDL_Quit();
}

bool processEvents(void)
{
    bool quit = false;
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            quit = true;
            break;
        }
    }
    return quit;
}

void drawPlasma(SDL_Surface *surface)
{
    int x, y;

    static uint16_t p1_xoff = 0xf000,
                    p1_yoff = 0xe000,
                    p2_xoff = 0x0001,
                    p2_yoff = 0x0003,
                    p3_xoff = 0x0000,
                    p3_yoff = 0x0000;
                    
    static uint16_t p1_fade = 0x0000,
                    p2_fade = 0x0000,
                    p3_fade = 0x0000;
    
    uint16_t    p1_sinpos_start_x = p1_xoff;
    uint16_t    p1_sinpos_start_y = p1_yoff;
    uint16_t    p1_sinposy;
    uint16_t    p1_sinposx;
    int         p1_palettePos = OFFSET_MAG;

    uint16_t    p2_sinpos_start_x = p2_xoff;
    uint16_t    p2_sinpos_start_y = p2_yoff;
    uint16_t    p2_sinposy;
    uint16_t    p2_sinposx;
    int         p2_palettePos = OFFSET_MAG;
    
    uint16_t    p3_sinpos_start_x = p3_xoff;
    uint16_t    p3_sinpos_start_y = p3_yoff;
    uint16_t    p3_sinposy;
    uint16_t    p3_sinposx;
    int         p3_palettePos = OFFSET_MAG;
        

    uint32_t *dest = surface->pixels;

    p1_sinposx = p1_sinpos_start_x;
    p2_sinposx = p2_sinpos_start_x;
    p3_sinposx = p3_sinpos_start_x;

    unsigned ypos = 0;

    for (y = 0; y < INTER_HEIGHT; y++) {

        p1_sinposy = p1_sinpos_start_y;
        p2_sinposy = p2_sinpos_start_y;
        p3_sinposy = p3_sinpos_start_y;

        for (x = 0; x < INTER_WIDTH; x++) {
            uint32_t colour;

            p1_sinposy += 61;
            colour = palette1[p1_palettePos - offsetTable[p1_sinposy>>7]];
            *(intermediateR + x + ypos) = colour;

            p2_sinposy += 47;
            colour = palette1[p2_palettePos - offsetTable[p2_sinposy>>7]];
            //colour >>= 1;
            *(intermediateG + x + ypos) = colour;
            
            p3_sinposy += 67;
            colour = 255-palette1[p3_palettePos - offsetTable[p3_sinposy>>7]];
            
            *(intermediateB + x + ypos) = colour;

        }
        /* if (y & 2) */ {
            for (x = 0; x < OUT_WIDTH; x++) {
                int srcpos;
                uint32_t colour;
                
                /* if (x & 2) */ {
                    // copy to row y
                    srcpos = OFFSET_MAG + x
                                + ypos - (offsetTable[p1_sinposx>>7]>>1);
                    
                    /* FIXME: SDL_MapRGB() is very likely not a very fast way to do this
                    */
                    colour = SDL_MapRGB(surface->format,
                                    intermediateG[srcpos],
                                    intermediateR[srcpos],
                                    intermediateB[srcpos]);
                                
                    *(dest + x) = colour;
                }
            }
        }

        p1_palettePos++;
        p1_sinposx += 263;
        p1_fade++;
        
        p2_palettePos++;
        p2_sinposx += 907;
                
        p3_palettePos++;
        p3_sinposx += 397;

        // advance to next output row
        dest += surface->w;
        ypos += INTER_WIDTH;

    }

    p1_xoff += 1559;    // Lots of magic values. They're all prime numbers
    p1_yoff += 307;     // because I figure that will make them more magical.
    
    p2_xoff += 521;
    p2_yoff += 179;
    
    p3_xoff += 131;
    p3_yoff += 89;
    
}

void initfpstimer(struct fpsctx* t, int fpslimit)
{
    t->prevtick  = SDL_GetTicks();
    t->tickInterval = 1000/fpslimit;
}

void limitfps(struct fpsctx* t)
{
    uint32_t currtick, expected;

    currtick = SDL_GetTicks();
    expected = t->prevtick + t->tickInterval;

    if (currtick < expected) {
        SDL_Delay(expected - currtick);
    }

    t->prevtick = SDL_GetTicks();
}
