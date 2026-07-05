// SDL3 template from https://examples.libsdl.org/SDL3/renderer/04-points/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "header.hpp"

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// 7/6 ratio
#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 300
int iters_n = 35;

/* This function runs once at startup. */
static Uint64 strt = 0;
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

    SDL_SetAppMetadata("Mandelbrot zoom", "1.0", "com.example.brh");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Mandelbrot zoom", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    strt = SDL_GetTicks();

    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
long double xx = -2.5, xy = 1.0, yx = -1.5, yy = 1.5;
int good(long double x, long double y) {
    long double zx = 0., zy = 0.;
    for(int j = 0 ; j < iters_n ; j++) {
        long double zzx = zx;
        zx = zx*zx - zy*zy + x;
        zy = 2*zzx*zy + y;
        if(zx*zx + zy*zy > 25.) return j;
    }

    return -1;
}

int gsb[WINDOW_WIDTH][WINDOW_HEIGHT][3];
int l = 0;
SDL_AppResult SDL_AppIterate(void *appstate) {
    Uint64 curt = SDL_GetTicks() - strt;
    if(curt/3000 != l) {
        l = curt/3000;
        iters_n++;
    }

    cout << curt << endl;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  /* white, full alpha */

    // guassian blur.
    memset(gsb, 0, sizeof gsb);
    for(int k = 0 ; k < WINDOW_WIDTH ; k++) {
        for(int j = 0 ; j < WINDOW_HEIGHT ; j++) {
            long double x = xx + ((k+1)*(xy-xx))/WINDOW_WIDTH;
            long double y = yx + ((j+1)*(yy-yx))/WINDOW_HEIGHT;
            // cout << yay << endl;
            int v = good(x, y);
            int r, g, b;
            if(v == -1) {
                // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            else {
                float yo = (float)v/iters_n;
                float r = yo*yo*yo, g = yo*yo, b = yo;
                // SDL_SetRenderDrawColor(renderer, v, v, v, 255);
                // for(int q = 0 ; q < 3 ; q++) gsb[k][j][q] = v;
                gsb[k][j][0] = (int)(255*r);
                gsb[k][j][1] = (int)(255*g);
                gsb[k][j][2] = (int)(255*b);
            }

            // SDL_RenderPoint(renderer, (long double)k, (long double)j);
        }
    }

    int stab = 30;
    for(int k = 1 ; k < WINDOW_WIDTH-1 ; k++) {
        for(int j = 1 ; j < WINDOW_HEIGHT-1 ; j++) {
            int rr[3];
            for(int q = 0 ; q < 3 ; q++) {
                rr[q] = (stab*gsb[k][j][q]+gsb[k+1][j][q]+gsb[k-1][j][q]+gsb[k+1][j+1][q]+gsb[k-1][j+1][q]+gsb[k+1][j-1][q]+gsb[k-1][j-1][q]+gsb[k][j+1][q]+gsb[k][j-1][q])/(stab+8);
            }

            SDL_SetRenderDrawColor(renderer, rr[0], rr[1], rr[2], 255);
            SDL_RenderPoint(renderer, (float)k, (float)j);
        }
    }

    // xx = (xx*999 + xy)/1000;
    // xy = (xy*999 + xx)/1000;
    // yx = (yx*999 + yy)/1000;
    // yy = (yy*999 + yx)/1000;
    
    xx = zom_ptx + (xx - zom_ptx) * 0.99f;
    xy = zom_ptx + (xy - zom_ptx) * 0.99f;
    yx = zom_pty + (yx - zom_pty) * 0.99f;
    yy = zom_pty + (yy - zom_pty) * 0.99f;

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {

}