/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"

#define PLAYER_ANIM_FRAMES     7
#define PLAYER_ANIM_SEQ        2

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef void (*fpScreenUpdate)(float);
typedef void (*fpScreenDraw)(void);
typedef void (*fpScreenInit)(void);
typedef void (*fpScreenLoad)(void);
typedef void (*fpScreenUnload)(void);
typedef void (*fpScreenFinish)(void);

typedef struct Screen {
	char screenId[65];			// The id of the screen, used as a unique index
	fpScreenUpdate Update;		// Called every loop on CurrentScreen
	fpScreenDraw Draw;			// Called every frame on CurrentScreen
	fpScreenInit Init;			// Called the first time the screen is Used (SetCurrent)
	fpScreenLoad Load;			// Called every time the screen is Used (SetCurrent)
	fpScreenUnload Unload;		// Called every time the screen is Unused (NotCurrent)
	fpScreenFinish Finish;		// Called one time when the game is exiting, if the 
								//     screen was ever Used (SetCurrent)
} Screen;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
Font font;

Texture2D dirt_tile;
Texture2D grass_tile;
Texture2D fence_vtile;

//Sound sndDoor;
//Sound sndScream;

void Screens_Init(void);
void Screens_Update(float delta);
void Screens_Draw(void);

/**
	Screens_AddScreen(const char* screenId, Screen* screen)
**/
void Screens_AddScreen(const char* screenId, Screen* screen);

/**
	Screens_Free(void);
	Finish and free each screen
**/
void Screens_Free(void);

void Screens_SetScreen(const char* screenId);



#endif // SCREENS_H
