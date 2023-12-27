#include "SDL.h"

// Application Struct for better typing and type checking
typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
} App;