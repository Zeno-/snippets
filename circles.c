#include <stdio.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_main.h>

#define DEG2RAD(x) ((x) * 0.01745329251994329576923690768489)


struct fpsctx {
    uint32_t prevtick;
    uint16_t tickInterval;
};
void initfpstimer(struct fpsctx *t, int fpslimit);
void limitfps(struct fpsctx *t);

int init_gfx(void);
SDL_Surface *loadblob(const char *filename);
int processEvents(void);

#define OUT_WIDTH  640
#define OUT_HEIGHT 480
#define RIGHT_MARGIN (OUT_WIDTH - 32)
#define TARGET_FPS 60

int x_positions[128];
int y_positions[128];

SDL_Surface *surface = NULL;

void make_pos_tables(void)
{
   int i;

   for (i = 0; i < 128; i++) {
        double v = i / 128.0 * 360 ;
        x_positions[i] = (sin(DEG2RAD(v)) + 1) * 31 + 0.5    + 64;
        y_positions[i] = (cos(DEG2RAD(v)) + 1) * 31 + 0.5    + OUT_HEIGHT - OUT_HEIGHT/3;
   }
}

#define NUM_BLOBS     8

int main(void)
{
    struct fpsctx fpstimer;
    unsigned px;
    SDL_Surface *blob;
    SDL_Rect blob_rect = {0, 0, 16, 16};
    SDL_Rect blob_dest;
    SDL_Rect canvas_rect = {0, 0, OUT_WIDTH, OUT_HEIGHT};
    int i;
    /* Pairs of indices (one pair per blob) for x_positions
     * and y_positions
     */
    unsigned blob_pos_indices[NUM_BLOBS * 2];

    make_pos_tables();
    init_gfx();

    if (!(blob = loadblob("bldob.png"))) {
        printf("Blob didn't load\n");
        exit(1);
    }

    initfpstimer(&fpstimer, TARGET_FPS);

    p_idx = 0;

    /* Build blob position indices
     */
    for (i = 0; i < NUM_BLOBS * 2; i += 2) {
        blob_pos_indices[i + 0]  = p_idx;
        blob_pos_indices[i + 1] = 128 - p_idx;
        p_idx += 128 / NUM_BLOBS;
    }

    while(!processEvents()) {

        /* Clear canvas */
        SDL_FillRect(surface, &canvas_rect, SDL_MapRGB(surface->format, 0, 0, 0));

        for (i = 0; i < NUM_BLOBS * 2; i += 2) {
            /* At left hand side of screen
             */
            blob_dest.x = x_positions[blob_pos_indices[i + 0]];
            blob_dest.y = y_positions[blob_pos_indices[i + 1]];
            SDL_BlitSurface(blob, &blob_rect, surface, &blob_dest);

            /* At right hand side of screen
             */
            blob_dest.x = RIGHT_MARGIN - x_positions[blob_pos_indices[i + 0]];
            blob_dest.y = y_positions[blob_pos_indices[i + 1]];
            SDL_BlitSurface(blob, &blob_rect, surface, &blob_dest);
        }

        SDL_Flip(surface);
        limitfps(&fpstimer);

        /* Increment blob position indices
         */
        for (i = 0; i < NUM_BLOBS * 2; i++) {
            blob_pos_indices[i] += 1;
            blob_pos_indices[i] &= 0x7f;
        }
    }

    SDL_FreeSurface(blob);

    return 0;
}

void cleanup(void)
{
    SDL_Quit();
}

int init_gfx(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 0;
    atexit(cleanup);
    surface = SDL_SetVideoMode(OUT_WIDTH, OUT_HEIGHT,
                               32,
                               SDL_HWSURFACE | SDL_DOUBLEBUF);
    return surface != NULL;
}

void initfpstimer(struct fpsctx *t, int fpslimit)
{
    t->prevtick  = SDL_GetTicks();
    t->tickInterval = 1000/fpslimit;
}

void limitfps(struct fpsctx *t)
{
    uint32_t currtick, expected;

    currtick = SDL_GetTicks();
    expected = t->prevtick + t->tickInterval;

    if (currtick < expected) {
        SDL_Delay(expected - currtick);
        //printf("Delayed %d\n", expected - currtick);
    }

    t->prevtick = SDL_GetTicks();
}

SDL_Surface *loadblob(const char *filename)
{
    SDL_Surface *img;
    SDL_Surface *final;

    if (!(img = IMG_Load(filename)))
        return NULL;

    final = SDL_ConvertSurface(img, surface->format, SDL_HWSURFACE);
    SDL_FreeSurface(img);

    if (final) {    /* Set blit "mask" */
        SDL_SetColorKey(final, SDL_SRCCOLORKEY, SDL_MapRGB(final->format, 0, 0, 0));
    }

    return final;
}

int processEvents(void)
{
    int quit = 0;
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            quit = 1;
            break;
        case SDL_KEYDOWN:
            quit = 1;
            break;
        }
    }
    return quit;
}
