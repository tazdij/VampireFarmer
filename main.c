/*******************************************************************************************
*
*   SKULLY ESCAPE [KING GAME JAM 2015]
*
*   This game has been created using raylib 1.6 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#define GRAPHICS_API_OPENGL_33

#include "raylib.h"
#include "rlgl.h"
#include "external/glad.h"
#include "screens/screens.h"
#include "player.h"

#include "screens/screen_splashmv.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
const int screenWidth = 1280;
const int screenHeight = 720;

// Required variables to manage screen transitions (fade-in, fade-out)
float transAlpha = 0;
bool onTransition = false;
bool transFadeOut = false;
int transFromScreen = -1;
int transToScreen = -1;

static int framesCounter = 0;

//Music music;
    
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
// void TransitionToScreen(int screen);
// void ChangeToScreen(int screen);    // No transition effect
// void UpdateTransition(void);
// void DrawTransition(void);

void UpdateDrawFrame(void);         // Update and Draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//---------------------------------------------------------
	const char windowTitle[30] = "VampireFarmer";

    InitWindow(screenWidth, screenHeight, windowTitle);

    // Global data loading (assets that must be available in all screens, i.e. fonts)
    InitAudioDevice();
    
    //music = LoadMusicStream("resources/audio/come_play_with_me.ogg");
    //PlayMusicStream(music);
    
    font = LoadFont("resources/textures/alagard.png");
    
    // Load Tile Textures
    dirt_tile = LoadTexture("resources/textures/brown_ground.png");
    grass_tile = LoadTexture("resources/textures/green_ground.png");
    
    fence_vtile = LoadTexture("resources/textures/grey_fence_01.png");
    
    //sndDoor = LoadSound("resources/audio/door.ogg");
    //sndScream = LoadSound("resources/audio/scream.ogg");
    
    
    // Initialize the Screen System
    Screens_Init();
	
	
	// Add all of the screens in the game to the Screens System
	Screens_AddScreen("splashmv", Screen_SplashMV_GetScreen());
	
    //InitPlayer();
    
    Screens_SetScreen("splashmv");
    printf("Screen Set\n");
    
    // Init the player object
    InitPlayer();
    
    // Setup and Init first screen
    //currentScreen = LOGO;
    //InitLogoScreen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
	// Unload and Free the Screens
	Screens_Free();

    // Unload all global loaded data (i.e. fonts) here!
	//UnloadPlayer();
    UnloadFont(font);
    //UnloadTexture(doors);
    //UnloadSound(sndDoor);
    //UnloadSound(sndScream);
    
    //UnloadMusicStream(music);
    
    CloseAudioDevice();
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
	
    return 0;
}



// Update and draw game frame
void UpdateDrawFrame(void)
{
    Screens_Update(GetFrameTime());
    
    //UpdateMusicStream(music);
    //----------------------------------------------------------------------------------
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
        ClearBackground(RAYWHITE);
        Screens_Draw();
    EndDrawing();
    //----------------------------------------------------------------------------------
}

