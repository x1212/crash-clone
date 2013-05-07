

#include <SDL/SDL.h>

// bool hack ... 
// TODO find better solution
#define bool int
#define true 1
#define false 0


#define ARENA_H 250
#define ARENA_W 250
#define ARENA_BORDER 60

#define BORDER_TILE_STATE 0
#define FLOOR_TILE_STATE 1
#define PLAYER_CAR_TILE_STATE 2
#define CPU_RED_CAR_TILE_STATE 3
#define CPU_BLUE_CAR_TILE_STATE 4
#define CPU_GREEN_CAR_TILE_STATE 5
#define CPU_YELLOW_CAR_TILE_STATE 6
#define RED_WALL_TILE_STATE 7
#define BLUE_WALL_TILE_STATE 8
#define GREEN_WALL_TILE_STATE 9
#define YELLOW_WALL_TILE_STATE 10
#define HOLE_TILE_STATE 11


// movement directions
#define DIR_U 0
#define DIR_R 2
#define DIR_D 4
#define DIR_L 6





extern int state;
extern SDL_Surface *screen;

SDL_Surface *tileset, *looser, *winner;
Uint8 *keystate;
int camX, camY;
int botdir[4], botcooldown[4]; // player counts as bot as well ...
int gamestate;
#define NOT_ENDED_STATE 0
#define WINNER_STATE 1
#define LOOSER_STATE 2
int timeout; // number of turns until state is set to menu
int activebots; // number of bots still in the game

void initGame( int* data1, int* data2 )
{
    int x, y;
    camX = 0; camY = 0;
    timeout = -1;
    gamestate = NOT_ENDED_STATE;
    activebots = 3;
    srand(time(NULL) + SDL_GetTicks());
    
    printf("\nInit new Game.\n");
    if ( tileset == NULL)
    {
        tileset = SDL_LoadBMP("../data/tiles.bmp");
    }
    if ( looser == NULL)
    {
        looser = SDL_LoadBMP("../data/looser.bmp");
        SDL_SetAlpha(looser, SDL_SRCALPHA /*| SDL_RLEACCEL*/, SDL_ALPHA_OPAQUE);
        looser->format->Amask = 0xFF000000;
        looser->format->Ashift = 24; 
    }
    if ( winner == NULL)
    {
        winner = SDL_LoadBMP("../data/winner.bmp");
        SDL_SetAlpha(winner, SDL_SRCALPHA /*| SDL_RLEACCEL*/, SDL_ALPHA_OPAQUE);
        winner->format->Amask = 0xFF000000;
        winner->format->Ashift = 24; 
    }

    
    // generate initial arena
    for ( x = 0; x < ARENA_W; x++ )
    {
        for ( y = 0; y < ARENA_H; y++ )
        {
            // calculate initial tilestate from its coordinates

            // borders
            if ( y < ARENA_BORDER || x < ARENA_BORDER || y >= ARENA_H - ARENA_BORDER || x >= ARENA_W - ARENA_BORDER )
            {
                *(data1+(y*ARENA_W + x)) = BORDER_TILE_STATE;
            }
            else
            {
                *(data1+(y*ARENA_W + x)) = FLOOR_TILE_STATE;
            }
        }
    }

    // startpositions
    *(data1+(((ARENA_H-2*ARENA_BORDER)/4 + ARENA_BORDER)*ARENA_W + ((ARENA_W-2*ARENA_BORDER)/4 + ARENA_BORDER))) = PLAYER_CAR_TILE_STATE;
    *(data1+(((ARENA_H-2*ARENA_BORDER)/4*3 + ARENA_BORDER)*ARENA_W + ((ARENA_W-2*ARENA_BORDER)/4 + ARENA_BORDER))) = CPU_BLUE_CAR_TILE_STATE;
    *(data1+(((ARENA_H-2*ARENA_BORDER)/4 + ARENA_BORDER)*ARENA_W + ((ARENA_W-2*ARENA_BORDER)/4*3 + ARENA_BORDER))) = CPU_GREEN_CAR_TILE_STATE;
    *(data1+(((ARENA_H-2*ARENA_BORDER)/4*3 + ARENA_BORDER)*ARENA_W + ((ARENA_W-2*ARENA_BORDER)/4*3 + ARENA_BORDER))) = CPU_YELLOW_CAR_TILE_STATE;

    // copy initial arena
    for ( x = 0; x < ARENA_W; x++ )
    {
        for ( y = 0; y < ARENA_H; y++ )
        {
            *(data2+(y*ARENA_W + x)) = *(data1+(y*ARENA_W + x));
        }
    }


    keystate = SDL_GetKeyState( NULL );
}


void handleKeyStates( void );
void drawTile( int tileID, int x, int y );
void drawGrid( int* data, int w, int h ); // data is pointer to array with tileIDs, w and h define the size of that array
void logic( int* data, int* dst, int w, int h ); // dst is pointer to the array that shoud hold the next state of all tiles

void game( void )
{
    static bool firstrun = true;
    static int data1[ARENA_W][ARENA_H], data2[ARENA_W][ARENA_H];
    static int *currentState, *nextState, *tmp;

    if ( firstrun )
    {
        currentState = data1;
        nextState = data2;
        initGame( data1, data2);

        firstrun = false;
    }

    SDL_PumpEvents(); // update keystates
    
    handleKeyStates();
    
    // calculate next state
    logic( currentState, nextState, ARENA_W, ARENA_H );
    // swap states
    tmp = currentState;
    currentState = nextState;
    nextState = tmp;

    // draw stuff

    // clear screen
    SDL_FillRect( screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
    // draw our data
    drawGrid( currentState, ARENA_W, ARENA_H );

    if ( state != 1 ) // we ended the game
    {
        firstrun = true; // we want a fresh new game next time
    }

    SDL_Delay(100);
    
    // gamestate stuff
    if ( timeout > 0 )
    {
        timeout--;
    } else if ( timeout == 0 )
    {
        SDL_Event event;
        state = 0;
        while ( SDL_PollEvent( &event ) )
        { /* do nothing, we only want all events to be thrown away before returning to menu */}
        firstrun = true;
    }

    if ( gamestate == WINNER_STATE )
    {

        SDL_Rect src, dst;
        src.w = 800; src.h = 480;
        dst.w = 800; dst.h = 480;

        src.x = 0; src.y = 0;
        dst.x = 0; dst.y = 0;
    
        SDL_BlitSurface( winner, &src, screen, &dst );


    }
    else if ( gamestate == LOOSER_STATE )
    {

        SDL_Rect src, dst;
        src.w = 800; src.h = 480;
        dst.w = 800; dst.h = 480;

        src.x = 0; src.y = 0;
        dst.x = 0; dst.y = 0;
    
        SDL_BlitSurface( looser, &src, screen, &dst );


    }
}


void handleKeyStates( void )
{
    if ( keystate[SDLK_ESCAPE] | keystate[SDLK_END] )
    {
        SDL_Event event;
        state = 0;
        while ( SDL_PollEvent( &event ) )
        { /* do nothing, we only want all events to be thrown away before returning to menu */}


        return;
    }
    if ( keystate[SDLK_RIGHT] )
    {
        //*if (SDL_GetTicks()%50 == 0)*/ camX++;
        if ( botdir[0] != DIR_L ) botdir[0] = DIR_R;
    }
    if ( keystate[SDLK_LEFT] )
    {
        //*if (SDL_GetTicks()%50 == 0)*/ camX--;
        if ( botdir[0] != DIR_R ) botdir[0] = DIR_L;
    }
    if ( keystate[SDLK_UP] )
    {
        //*if (SDL_GetTicks()%50 == 0)*/ camY--;
        if ( botdir[0] != DIR_D ) botdir[0] = DIR_U;
    }
    if ( keystate[SDLK_DOWN] )
    {
        //*if (SDL_GetTicks()%50 == 0)*/ camY++;
        if ( botdir[0] != DIR_U ) botdir[0] = DIR_D;
    }


}

void drawTile( int tileID, int x, int y )
{
    SDL_Rect src, dst;
    src.w = 16; src.h = 16;
    dst.w = 16; dst.h = 16;

    src.x = (tileID % 16) * 16; src.y = (tileID / 16) * 16;
    dst.x = (x - camX)*16; dst.y = (y - camY)*16;
    
    SDL_BlitSurface( tileset, &src, screen, &dst );
}

// TODO:
int getWallTileOffset( int *data, int x, int y );

void drawGrid( int* data, int w, int h )
{
    int x, y;

    for ( x = camX; x < camX + 800/16; x++ )
    {
        for ( y = camY; y < camY + 480/16; y++ )
        {
            if (y < ARENA_H && y >= 0  &&  x < ARENA_W && x >= 0 )
            {
                int tile2draw = 0;

                switch (*(data+(y*ARENA_W + x))) {
                    case BORDER_TILE_STATE:
                        tile2draw = 16;
                        if ( *(data+(y*ARENA_W + x - 1)) != BORDER_TILE_STATE && (y - ARENA_BORDER)%4 != 0 ) tile2draw = 6;
                        else if ( *(data+(y*ARENA_W + x - 1)) != BORDER_TILE_STATE && (y - ARENA_BORDER)%4 == 0 ) tile2draw = 7;
                        else if ( *(data+((y + 1)*ARENA_W + x)) != BORDER_TILE_STATE && (x - ARENA_BORDER)%4 != 0 ) tile2draw = 4;
                        else if ( *(data+((y + 1)*ARENA_W + x)) != BORDER_TILE_STATE && (x - ARENA_BORDER)%4 == 0 ) tile2draw = 5;
                        else if ( *(data+(y*ARENA_W + x + 1)) != BORDER_TILE_STATE && (y - ARENA_BORDER)%4 != 0 ) tile2draw = 10;
                        else if ( *(data+(y*ARENA_W + x + 1)) != BORDER_TILE_STATE && (y - ARENA_BORDER)%4 == 0 ) tile2draw = 11;
                        else if ( *(data+((y - 1)*ARENA_W + x)) != BORDER_TILE_STATE && (x - ARENA_BORDER)%4 != 0 ) tile2draw = 8;
                        else if ( *(data+((y - 1)*ARENA_W + x)) != BORDER_TILE_STATE && (x - ARENA_BORDER)%4 == 0 ) tile2draw = 9;
                        else if ( *(data+((y + 1)*ARENA_W + x + 1)) != BORDER_TILE_STATE ) tile2draw = 12;
                        else if ( *(data+((y + 1)*ARENA_W + x - 1)) != BORDER_TILE_STATE ) tile2draw = 13;
                        else if ( *(data+((y - 1)*ARENA_W + x + 1)) != BORDER_TILE_STATE ) tile2draw = 14;
                        else if ( *(data+((y - 1)*ARENA_W + x - 1)) != BORDER_TILE_STATE ) tile2draw = 15;


                        break;
                    case FLOOR_TILE_STATE:
                        if ( (x - ARENA_BORDER)%4 == 0 ) tile2draw = 1;
                        if ( tile2draw == 1 && (y - ARENA_BORDER)%4 == 0) tile2draw = 3;
                        else if ( (y - ARENA_BORDER)%4 == 0 ) tile2draw = 2;
                        break;
                    case PLAYER_CAR_TILE_STATE:
                    case CPU_RED_CAR_TILE_STATE:
                        tile2draw = 93 + botdir[0];
                        break;
                    case CPU_GREEN_CAR_TILE_STATE:
                        tile2draw = 101 + botdir[1];
                        break;
                    case CPU_BLUE_CAR_TILE_STATE:
                        tile2draw = 109 + botdir[2];
                        break;
                    case CPU_YELLOW_CAR_TILE_STATE:
                        tile2draw = 117 + botdir[3];
                        break;
                    case RED_WALL_TILE_STATE:
                        tile2draw = 29;
                        if ( *(data+(y*ARENA_W + x - 1)) == RED_WALL_TILE_STATE ) // left wall?
                        {
                            if ( *(data+(y*ARENA_W + x + 1)) == RED_WALL_TILE_STATE ) // right wall?
                            {
                                if ( *(data+((y-1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // upper wall?
                                {
                                    if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                                    {
                                        tile2draw = 44;
                                    }
                                    else tile2draw = 40;
                                }
                                else if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 41;
                                }
                                else tile2draw = 34;
                            }
                            else if ( *(data+((y-1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 42;
                                }
                                else tile2draw = 36;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 38;
                            }
                            else tile2draw = 31;

                        }
                        else if ( *(data+(y*ARENA_W + x + 1)) == RED_WALL_TILE_STATE ) // right wall?
                        {
                            if ( *(data+((y-1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 43;
                                }
                                else tile2draw = 37;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 39;
                            }
                            else tile2draw = 30;

                        }
                        else if ( *(data+((y-1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // upper wall?
                        {
                            if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 35;
                            }
                            else tile2draw = 33;

                        }
                        else if ( *(data+((y+1)*ARENA_W + x)) == RED_WALL_TILE_STATE ) // wall bellow?
                        {
                            tile2draw = 32;
                        }
                        break;




                    case GREEN_WALL_TILE_STATE:
                        tile2draw = 45;
                        if ( *(data+(y*ARENA_W + x - 1)) == GREEN_WALL_TILE_STATE ) // left wall?
                        {
                            if ( *(data+(y*ARENA_W + x + 1)) == GREEN_WALL_TILE_STATE ) // right wall?
                            {
                                if ( *(data+((y-1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // upper wall?
                                {
                                    if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                                    {
                                        tile2draw = 60;
                                    }
                                    else tile2draw = 56;
                                }
                                else if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 57;
                                }
                                else tile2draw = 50;
                            }
                            else if ( *(data+((y-1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 58;
                                }
                                else tile2draw = 52;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 54;
                            }
                            else tile2draw = 47;

                        }
                        else if ( *(data+(y*ARENA_W + x + 1)) == GREEN_WALL_TILE_STATE ) // right wall?
                        {
                            if ( *(data+((y-1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 59;
                                }
                                else tile2draw = 53;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 55;
                            }
                            else tile2draw = 46;

                        }
                        else if ( *(data+((y-1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // upper wall?
                        {
                            if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 51;
                            }
                            else tile2draw = 49;

                        }
                        else if ( *(data+((y+1)*ARENA_W + x)) == GREEN_WALL_TILE_STATE ) // wall bellow?
                        {
                            tile2draw = 48;
                        }
                        break;


                    case BLUE_WALL_TILE_STATE:
                        tile2draw = 61;
                        if ( *(data+(y*ARENA_W + x - 1)) == BLUE_WALL_TILE_STATE ) // left wall?
                        {
                            if ( *(data+(y*ARENA_W + x + 1)) == BLUE_WALL_TILE_STATE ) // right wall?
                            {
                                if ( *(data+((y-1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // upper wall?
                                {
                                    if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                                    {
                                        tile2draw = 76;
                                    }
                                    else tile2draw = 72;
                                }
                                else if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 73;
                                }
                                else tile2draw = 66;
                            }
                            else if ( *(data+((y-1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 74;
                                }
                                else tile2draw = 68;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 70;
                            }
                            else tile2draw = 63;

                        }
                        else if ( *(data+(y*ARENA_W + x + 1)) == BLUE_WALL_TILE_STATE ) // right wall?
                        {
                            if ( *(data+((y-1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 75;
                                }
                                else tile2draw = 69;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 71;
                            }
                            else tile2draw = 62;

                        }
                        else if ( *(data+((y-1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // upper wall?
                        {
                            if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 67;
                            }
                            else tile2draw = 65;

                        }
                        else if ( *(data+((y+1)*ARENA_W + x)) == BLUE_WALL_TILE_STATE ) // wall bellow?
                        {
                            tile2draw = 64;
                        }
                        break;




                    case YELLOW_WALL_TILE_STATE:
                        tile2draw = 77;
                        if ( *(data+(y*ARENA_W + x - 1)) == YELLOW_WALL_TILE_STATE ) // left wall?
                        {
                            if ( *(data+(y*ARENA_W + x + 1)) == YELLOW_WALL_TILE_STATE ) // right wall?
                            {
                                if ( *(data+((y-1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // upper wall?
                                {
                                    if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                                    {
                                        tile2draw = 92;
                                    }
                                    else tile2draw = 88;
                                }
                                else if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 89;
                                }
                                else tile2draw = 82;
                            }
                            else if ( *(data+((y-1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 90;
                                }
                                else tile2draw = 84;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 86;
                            }
                            else tile2draw = 79;

                        }
                        else if ( *(data+(y*ARENA_W + x + 1)) == YELLOW_WALL_TILE_STATE ) // right wall?
                        {
                            if ( *(data+((y-1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // upper wall?
                            {
                                if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                                {
                                    tile2draw = 91;
                                }
                                else tile2draw = 85;
                            }
                            else if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 87;
                            }
                            else tile2draw = 78;

                        }
                        else if ( *(data+((y-1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // upper wall?
                        {
                            if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                            {
                                tile2draw = 83;
                            }
                            else tile2draw = 81;

                        }
                        else if ( *(data+((y+1)*ARENA_W + x)) == YELLOW_WALL_TILE_STATE ) // wall bellow?
                        {
                            tile2draw = 80;
                        }
                        break;




                    case HOLE_TILE_STATE:
                        tile2draw = 28;
                        break;
                    default:
                        tile2draw = 93;
                        break;
                }

                drawTile( tile2draw, x, y );
            }
        }
    }
}

int getDist( int *data, int dir, int x, int y)
{
    int i, currentDist = 0;
    for ( i = 1; i < 100; i++ )
    {
        switch (dir)
        {
            case DIR_U:
                if ( *(data+((y-i)*ARENA_W + x)) != FLOOR_TILE_STATE )
                {
                    return i;
                }
                break;

            case DIR_D:
                if ( *(data+((y+i)*ARENA_W + x)) != FLOOR_TILE_STATE )
                {
                    return i;
                }
                break;

            case DIR_L:
                if ( *(data+(y*ARENA_W + x - i)) != FLOOR_TILE_STATE )
                {
                    return i;
                }
                break;

            case DIR_R:
                if ( *(data+(y*ARENA_W + x + i)) != FLOOR_TILE_STATE )
                {
                    return i;
                }
                break;
            default:
                break;
        }
    }
    return currentDist;
}

void ai( int *data, int x, int y )
{
    int botID;
    int dice;
    int currentDist; // distance to next obstracle in current dir

    switch ( *(data+(y*ARENA_W + x)) )
    {
        case CPU_GREEN_CAR_TILE_STATE:
            botID = 1;
            break;

        case CPU_BLUE_CAR_TILE_STATE:
            botID = 2;
            break;

        case CPU_YELLOW_CAR_TILE_STATE:
            botID = 3;
            break;

        default:
            return;
    }
    
    currentDist = getDist( data, botdir[botID], x, y );
    
    dice = rand()%100;
    if ( currentDist < 5 && dice > 50 )
    {
        //if (botID==2) printf( "%d:%d->%d\n", botID, botdir[botID], dice%4 );
        if (dice%4*2 != (botdir[botID]+4)%8) botdir[botID] = dice%4*2;
        if ( getDist( data, botdir[botID], x, y) < 5 ) // checks current dir for beeing 'free'
        {
            botdir[botID] += 2;
            if ( getDist( data, botdir[botID], x, y) < 5 ) 
            {
                botdir[botID] += 2;
                if ( getDist( data, botdir[botID], x, y) < 5 )
                {
                    botdir[botID] += 2;
                    if ( getDist( data, botdir[botID], x, y) < 5 ) botdir[botID] += 2;
                }
            }
        }
    }
    else if ( currentDist < 3 && dice > 10 )
    {
        //if (botID==2) printf( "%d:%d->%d\n", botID, botdir[botID], dice%4 );
        if (dice%4*2 != (botdir[botID]+4)%8) botdir[botID] = dice%4*2;
        if ( getDist( data, botdir[botID], x, y) < 3 ) // checks current dir for beeing 'free'
        {
            botdir[botID] += 2;
            if ( getDist( data, botdir[botID], x, y) < 3 ) 
            {
                botdir[botID] += 2;
                if ( getDist( data, botdir[botID], x, y) < 3 )
                {
                    botdir[botID] += 2;
                    if ( getDist( data, botdir[botID], x, y) < 3 ) botdir[botID] += 2;
                }
            }
        }
    }
    else if ( currentDist < 2 && dice > 0 )
    {
        printf( "%d:%d->%d\n", botID, botdir[botID], dice%4 );
        if (dice%4*2 != (botdir[botID]+4)%8) botdir[botID] = dice%4*2;
        if ( getDist( data, botdir[botID], x, y) < 2 ) // checks current dir for beeing 'free'
        {
            botdir[botID] += 2;
            if ( getDist( data, botdir[botID], x, y) < 2 ) 
            {
                botdir[botID] += 2;
                if ( getDist( data, botdir[botID], x, y) < 2 )
                {
                    botdir[botID] += 2;
                    if ( getDist( data, botdir[botID], x, y) < 2 ) botdir[botID] += 2;
                }
            }
        }
        //printf( "%d:%d  dist:%d\n", botID, botdir[botID], getDist(data,botdir[botID],x,y) );
    }
    else if ( dice > 75 )
    {
        int distu, distd, distr, distl;
        distu = getDist(data, DIR_U, x, y);
        distd = getDist(data, DIR_D, x, y);
        distr = getDist(data, DIR_R, x, y);
        distl = getDist(data, DIR_L, x, y);
        if ( distu > distd && distu > distl && distu > distr ) botdir[botID] = DIR_U;
        if ( distd > distu && distd > distl && distd > distr ) botdir[botID] = DIR_D;
        if ( distl > distd && distl > distu && distl > distr ) botdir[botID] = DIR_L;
        if ( distr > distd && distr > distl && distr > distu ) botdir[botID] = DIR_R;
        if ( getDist( data, botdir[botID], x, y) < 1 ) // checks current dir for beeing 'free'
        {
            botdir[botID] += 2;
            if ( getDist( data, botdir[botID], x, y) < 1 ) 
            {
                botdir[botID] += 2;
                if ( getDist( data, botdir[botID], x, y) < 1 )
                {
                    botdir[botID] += 2;
                    if ( getDist( data, botdir[botID], x, y) < 1 ) botdir[botID] += 2;
                }
            }
        }

    }
    botdir[botID] = botdir[botID]%8;
    //else printf( "%d \n", dice ); 
}

// TODO:
void destroy( int *data, int *dst, int x, int y, int destroyedTile ); // x and y are the new coordinates of the object beeing destroyed, destroyed Tile tells us what is destroyed (at x|y is only the obstracle)
void moveCar( int *data, int *dst, int x, int y, int dir, int wall, int car );
#define TILE( x, y) (*(data+((y)*ARENA_W + (x))))
#define DEST_TILE( x, y) (*(dst+((y)*ARENA_W + (x))))

void logic( int *data, int *dst, int w, int h )
{
    int x, y;
    
    // syncronize old and new state before aplying logic
    for ( x = 0; x < ARENA_W; x++ )
    {
        for ( y = 0; y < ARENA_H; y++ )
        {
            *(dst+(y*ARENA_W + x)) = *(data+(y*ARENA_W + x));
        }
    }


    for ( x = ARENA_BORDER; x < ARENA_W; x++ )
    {
        for ( y = ARENA_BORDER; y < ARENA_H; y++ )
        {
            int dir, wall, car;
            switch ( TILE( x, y) )
            {
                case PLAYER_CAR_TILE_STATE:
                    camX = x - 800/16/2;
                    camY = y - 480/16/2;
                    dir = botdir[0];
                    wall = RED_WALL_TILE_STATE;
                    car = PLAYER_CAR_TILE_STATE;
                    moveCar( data, dst, x, y, dir, wall, car );
                    break;




                case CPU_GREEN_CAR_TILE_STATE:
                    ai(data,x,y);
                    if ( botdir[1] == DIR_R )
                    {
                        if ( *(data+(y*ARENA_W + x + 1)) == FLOOR_TILE_STATE && *(dst+(y*ARENA_W + x + 1)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            *(dst+(y*ARENA_W + x + 1)) = CPU_GREEN_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+(y*ARENA_W + x + 1)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            if (*(data+(y*ARENA_W + x + 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x + 1)) = HOLE_TILE_STATE;
                        }
                    }
                    else if ( botdir[1] == DIR_L )
                    {
                        if ( *(data+(y*ARENA_W + x - 1)) == FLOOR_TILE_STATE && *(dst+(y*ARENA_W + x - 1)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            *(dst+(y*ARENA_W + x - 1)) = CPU_GREEN_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+(y*ARENA_W + x - 1)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            if (*(data+(y*ARENA_W + x - 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x - 1)) = HOLE_TILE_STATE;
                        }

                    }
                    else if ( botdir[1] == DIR_U )
                    {
                        if ( *(data+((y-1)*ARENA_W + x)) == FLOOR_TILE_STATE && *(dst+((y-1)*ARENA_W + x)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            *(dst+((y-1)*ARENA_W + x)) = CPU_GREEN_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+((y-1)*ARENA_W + x)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            if (*(data+((y-1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y-1)*ARENA_W + x)) = HOLE_TILE_STATE;
                        }

                    }
                    else if ( botdir[1] == DIR_D )
                    {
                        if ( *(data+((y+1)*ARENA_W + x)) == FLOOR_TILE_STATE && *(dst+((y+1)*ARENA_W + x)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            *(dst+((y+1)*ARENA_W + x)) = CPU_GREEN_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+((y+1)*ARENA_W + x)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = GREEN_WALL_TILE_STATE;
                            if (*(data+((y+1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y+1)*ARENA_W + x)) = HOLE_TILE_STATE;
                        }

                    }
                    break;


                case CPU_BLUE_CAR_TILE_STATE:
                    ai(data,x,y);
                    if ( botdir[2] == DIR_R )
                    {
                        if ( *(data+(y*ARENA_W + x + 1)) == FLOOR_TILE_STATE && *(dst+(y*ARENA_W + x + 1)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            *(dst+(y*ARENA_W + x + 1)) = CPU_BLUE_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+(y*ARENA_W + x + 1)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            if (*(data+(y*ARENA_W + x + 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x + 1)) = HOLE_TILE_STATE;
                        }
                    }
                    else if ( botdir[2] == DIR_L )
                    {
                        if ( *(data+(y*ARENA_W + x - 1)) == FLOOR_TILE_STATE && *(dst+(y*ARENA_W + x - 1)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            *(dst+(y*ARENA_W + x - 1)) = CPU_BLUE_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+(y*ARENA_W + x -1)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            if (*(data+(y*ARENA_W + x - 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x - 1)) = HOLE_TILE_STATE;
                        }

                    }
                    else if ( botdir[2] == DIR_U )
                    {
                        if ( *(data+((y-1)*ARENA_W + x)) == FLOOR_TILE_STATE && *(dst+((y-1)*ARENA_W + x)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            *(dst+((y-1)*ARENA_W + x)) = CPU_BLUE_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+((y-1)*ARENA_W + x)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            if (*(data+((y-1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y-1)*ARENA_W + x)) = HOLE_TILE_STATE;
                        }

                    }
                    else if ( botdir[2] == DIR_D )
                    {
                        if ( *(data+((y+1)*ARENA_W + x)) == FLOOR_TILE_STATE && *(dst+((y+1)*ARENA_W + x)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            *(dst+((y+1)*ARENA_W + x)) = CPU_BLUE_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+((y+1)*ARENA_W + x)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }
                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = BLUE_WALL_TILE_STATE;
                            if (*(data+((y+1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y+1)*ARENA_W + x)) = HOLE_TILE_STATE;
                        }

                    }
                    break;



                case CPU_YELLOW_CAR_TILE_STATE:
                    ai(data,x,y);
                    if ( botdir[3] == DIR_R )
                    {
                        if ( *(data+(y*ARENA_W + x + 1)) == FLOOR_TILE_STATE && *(dst+(y*ARENA_W + x + 1)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            *(dst+(y*ARENA_W + x + 1)) = CPU_YELLOW_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+(y*ARENA_W + x + 1)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            if (*(data+(y*ARENA_W + x + 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x + 1)) = HOLE_TILE_STATE;
                        }
                    }
                    else if ( botdir[3] == DIR_L )
                    {
                        if ( *(data+(y*ARENA_W + x - 1)) == FLOOR_TILE_STATE && *(dst+(y*ARENA_W + x - 1)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            *(dst+(y*ARENA_W + x - 1)) = CPU_YELLOW_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+(y*ARENA_W + x - 1)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            if (*(data+(y*ARENA_W + x - 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x - 1)) = HOLE_TILE_STATE;
                        }

                    }
                    else if ( botdir[3] == DIR_U )
                    {
                        if ( *(data+((y-1)*ARENA_W + x)) == FLOOR_TILE_STATE && *(dst+((y-1)*ARENA_W + x)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            *(dst+((y-1)*ARENA_W + x)) = CPU_YELLOW_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+((y-1)*ARENA_W + x)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            if (*(data+((y-1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y-1)*ARENA_W + x)) = HOLE_TILE_STATE;
                        }

                    }
                    else if ( botdir[3] == DIR_D )
                    {
                        if ( *(data+((y+1)*ARENA_W + x)) == FLOOR_TILE_STATE && *(dst+((y+1)*ARENA_W + x)) == FLOOR_TILE_STATE )
                        {
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            *(dst+((y+1)*ARENA_W + x)) = CPU_YELLOW_CAR_TILE_STATE;
                        }
                        else { // destroy
                            switch ( *(dst+((y+1)*ARENA_W + x)) )
                            {
                                case PLAYER_CAR_TILE_STATE:
                                    gamestate = LOOSER_STATE;
                                    timeout = 15;
                                    break;

                                case CPU_GREEN_CAR_TILE_STATE:
                                case CPU_BLUE_CAR_TILE_STATE:
                                case CPU_YELLOW_CAR_TILE_STATE:
                                    activebots--;
                                    break;

                                default:
                                    break;
                            }

                            activebots--;
                            if ( gamestate != LOOSER_STATE && activebots == 0 )
                            {
                                gamestate = WINNER_STATE;
                                timeout = 15;
                            }
                            *(dst+(y*ARENA_W + x)) = YELLOW_WALL_TILE_STATE;
                            if (*(data+((y+1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y+1)*ARENA_W + x)) = HOLE_TILE_STATE;
                        }

                    }
                    break;



            }
        }
    }
}


void destroy( int *data, int *dst, int x, int y, int destroyedTile )
{
    switch ( *(dst+(y*ARENA_W + x)) )
    {
        case PLAYER_CAR_TILE_STATE:
            gamestate = LOOSER_STATE;
            timeout = 15;
            break;

        case CPU_GREEN_CAR_TILE_STATE:
        case CPU_BLUE_CAR_TILE_STATE:
        case CPU_YELLOW_CAR_TILE_STATE:
            activebots--;
            break;

        default:
            break;
    }

    switch ( destroyedTile )
    {
        case PLAYER_CAR_TILE_STATE:
            gamestate = LOOSER_STATE;
            timeout = 15;
            break;

        case CPU_GREEN_CAR_TILE_STATE:
        case CPU_BLUE_CAR_TILE_STATE:
        case CPU_YELLOW_CAR_TILE_STATE:
            activebots--;
            break;

        default:
            break;
    }

    if ( gamestate != LOOSER_STATE && activebots == 0 )
    {
        gamestate = WINNER_STATE;
        timeout = 15;
    }
    if (*(data+(y*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x)) = HOLE_TILE_STATE;
    if (*(data+((y-1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y-1)*ARENA_W + x)) = HOLE_TILE_STATE;
    if (*(data+(y*ARENA_W + x + 1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x + 1)) = HOLE_TILE_STATE;
    if (*(data+((y+1)*ARENA_W + x)) != BORDER_TILE_STATE ) *(dst+((y+1)*ARENA_W + x)) = HOLE_TILE_STATE;
    if (*(data+(y*ARENA_W + x -1)) != BORDER_TILE_STATE ) *(dst+(y*ARENA_W + x - 1)) = HOLE_TILE_STATE;
}


void moveCar( int *data, int *dst, int x, int y, int dir, int wall, int car )
{
    if ( dir == DIR_R )
    {
        if ( TILE( x + 1, y ) == FLOOR_TILE_STATE && DEST_TILE( x + 1, y ) == FLOOR_TILE_STATE )
        {
            DEST_TILE( x, y ) = wall;
            DEST_TILE( x + 1, y ) = car;
        }
        else
        { // destroy
            destroy( data, dst, x + 1, y, TILE( x, y ) );
        }
    }
    else if ( dir == DIR_L )
    {
        if ( TILE( x - 1, y ) == FLOOR_TILE_STATE && DEST_TILE( x - 1, y ) == FLOOR_TILE_STATE )
        {
            DEST_TILE( x, y ) = wall;
            DEST_TILE( x - 1, y ) = car;
        }
        else
        { // destroy
            destroy( data, dst, x - 1, y, TILE( x, y ));
        }

    }
    else if ( dir == DIR_U )
    {
        if ( TILE( x, y - 1 ) == FLOOR_TILE_STATE && DEST_TILE( x, y - 1 ) == FLOOR_TILE_STATE )
        {
            DEST_TILE( x, y ) = wall;
            DEST_TILE( x, y - 1 ) = car;
        }
        else
        { // destroy
            destroy( data, dst, x, y - 1, TILE( x, y ));
        }

    }
    else if ( dir == DIR_D )
    {
        if ( TILE( x, y + 1 ) == FLOOR_TILE_STATE && DEST_TILE( x, y + 1 ) == FLOOR_TILE_STATE )
        {
            DEST_TILE( x, y ) = wall;
            DEST_TILE( x, y + 1 ) = car;
        }
        else
        { // destroy
            destroy( data, dst, x, y + 1, TILE( x, y ));
        }
    }

}
