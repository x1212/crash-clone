

#include <SDL/SDL.h>

// bool hack ... 
// TODO find better solution
#define bool int
#define true 1
#define false 0


#define START_GAME_SELECT 0
#define DIFFICULTY_SELECT 1
#define SPEED_SELECT 2
#define SLOW_SPEED 0
#define AVERAGE_SPEED 1
#define FAST_SPEED 2
#define INSANE_SPEED 3
#define EASY_DIFFICULTY 0
#define STANDARD_DIFFICULTY 1
#define HARD_DIFFICULTY 2


extern int state;
extern SDL_Surface *screen;

int speed, difficulty; // these will be used from within the game
int selection;

SDL_Surface *menuBG, *startButton, *easyMode, *standardMode, *hardMode, *fast, *average, *slow, *insane;

void activateAlpha( SDL_Surface *surf )
{
    SDL_SetAlpha(surf, SDL_SRCALPHA /*| SDL_RLEACCEL*/, SDL_ALPHA_OPAQUE);
    surf->format->Amask = 0xFF000000;
    surf->format->Ashift = 24; 
}

void initMenu( void )
{
    menuBG = SDL_LoadBMP("../data/title2.bmp");
    startButton = SDL_LoadBMP("../data/start.bmp");
    easyMode = SDL_LoadBMP("../data/easy.bmp");
    standardMode = SDL_LoadBMP("../data/standard.bmp");
    hardMode = SDL_LoadBMP("../data/hard.bmp");
    fast = SDL_LoadBMP("../data/fast.bmp");
    slow = SDL_LoadBMP("../data/slow.bmp");
    average = SDL_LoadBMP("../data/average.bmp");
    insane = SDL_LoadBMP("../data/insane.bmp");

    activateAlpha( startButton );
    activateAlpha( easyMode );
    activateAlpha( standardMode );
    activateAlpha( hardMode );
    activateAlpha( fast );
    activateAlpha( slow );
    activateAlpha( average );
    activateAlpha( insane );

    selection = START_GAME_SELECT;
    speed = SLOW_SPEED;
    difficulty = STANDARD_DIFFICULTY;
}

void handleKeyEvent( SDL_Event *event )
{
    int selectionChange = 0;
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
            if ( selection == START_GAME_SELECT ) state = 1; // start game
            break;
        case SDLK_DOWN:
            selection = (selection + 1) % 3;
            break;
        case SDLK_UP:
            selection--;
            if ( selection < 0 ) selection = 2;
            break;
        case SDLK_LEFT:
            selectionChange = -1;
            break;
        case SDLK_RIGHT:
            selectionChange = 1;
            break;
        default:
            break;
    }
    if ( selectionChange != 0 )
    {
        switch ( selection )
        {
            case START_GAME_SELECT:
                break;
            case DIFFICULTY_SELECT:
                difficulty += selectionChange;
                if ( difficulty < 0 ) difficulty = 0;
                if ( difficulty > 2 ) difficulty = 2;
                break;
            case SPEED_SELECT:
                speed += selectionChange;
                if ( speed < 0 ) speed = 0;
                if ( speed > 3 ) speed = 3;
                break;
        }

    }
}

void menu( void )
{
    static bool firstrun = true;
    SDL_Rect SrcR, DstR;
    SDL_Event event;
    SDL_Surface *modeSelector, *speedSelector;

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

    switch ( difficulty )
    {
        case EASY_DIFFICULTY:
            modeSelector = easyMode;
            break;
        case STANDARD_DIFFICULTY:
            modeSelector = standardMode;
            break;
        case HARD_DIFFICULTY:
            modeSelector = hardMode;
            break;
        default:
            break;
    }

    switch ( speed )
    {
        case FAST_SPEED:
            speedSelector = fast;
            break;
        case SLOW_SPEED:
            speedSelector = slow;
            break;
        case AVERAGE_SPEED:
            speedSelector = average;
            break;
        case INSANE_SPEED:
            speedSelector = insane;
            break;
        default:
            break;
    }

    // draw menu
    if ( selection == START_GAME_SELECT ) DstR.x = 2*sin(SDL_GetTicks());
    else DstR.x = 0;
    DstR.y = 250;
    SDL_BlitSurface( startButton, &SrcR, screen, &DstR );
    if ( selection == DIFFICULTY_SELECT ) DstR.x = 2*sin(SDL_GetTicks());
    else DstR.x = 0;
    DstR.y = 300;
    SDL_BlitSurface( modeSelector, &SrcR, screen, &DstR );
    if ( selection == SPEED_SELECT ) DstR.x = 2*sin(SDL_GetTicks());
    else DstR.x = 0;
    DstR.y = 350;
    SDL_BlitSurface( speedSelector, &SrcR, screen, &DstR );

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
