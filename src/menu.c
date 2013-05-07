

#include <SDL/SDL.h>

// bool hack ... 
// TODO find better solution
#define bool int
#define true 1
#define false 0



extern int state;
extern SDL_Surface *screen;

SDL_Surface *menuBG;

void initMenu( void )
{
    menuBG = SDL_LoadBMP("../data/title2.bmp");
}

void handleKeyEvent( SDL_Event *event )
{
    //printf("Key pressed event recognized!\n");
    switch ( event->key.keysym.sym )
    {
        case SDLK_END:
        case SDLK_ESCAPE:
            state = -1; // quit
            break;
        case SDLK_HOME:
        case SDLK_SPACE:
        case SDLK_RETURN:
            state = 1; // start game
        default:
            break;
    }
}

void menu( void )
{
    static bool firstrun = true;
    SDL_Rect SrcR, DstR;
    SDL_Event event;

    if ( firstrun )
    {
        initMenu();
        firstrun = false;
    }
    
    // clear screen
    SDL_FillRect( screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

    // draw bg
    SrcR.x = 0; SrcR.y = 0; SrcR.w = 800; SrcR.h = 480;
    DstR.x = 0; DstR.y = 0; DstR.w = 800; DstR.h = 480;

    SDL_BlitSurface( menuBG, &SrcR, screen, &DstR );

    while(SDL_PollEvent(&event)) {  /* Loop until there are no events left on the queue */
        switch(event.type) { /* Process the appropriate event type */
            case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
                handleKeyEvent( &event );
                break;
            //case SDL_MOUSEMOTION: // maybe i will use the mouse in future ... atm it seems not like a good idea thogh
            default:
                break; // nothing to do
  }
}
}
