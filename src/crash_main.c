
#include "datamgr.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>


#define SCREEN_RES 800, 480, 32


void game( void );
void menu( void );

int state;

SDL_Surface *screen;

void initMain( void )
{
    state = 0;
    
    // SDL initialisation
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO );
    screen = SDL_SetVideoMode( SCREEN_RES, SDL_HWSURFACE | SDL_DOUBLEBUF );

}

int main( void )
{
    
    initMain();

    while ( state >= 0  &&  state < 2 )
    {
        // simple state management
        switch ( state )
        {
            case 0:
                menu();
                break;
            case 1:
                game();
                break;
            default:
                break;
        }
        SDL_Flip(screen);
    }

    return 0;
}

//void game( void ){}
//void menu( void ){}

