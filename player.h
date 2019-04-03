#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "rlgl.h"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum {
    IDLE,
    WALK,
    PLANT
} PlayerAnimation;

typedef struct Player {
    Vector3 position;
    Texture2D texture;
    Color color;
    
    // Animation variables
    Rectangle frameRec;
    int currentFrame;
    int currentSeq;
    
    char isMoving;
    Vector3 targetPos;
} Player;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
Player player;

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitPlayer(void);
void UpdatePlayer(float delta);
void DrawPlayer(void);
void UnloadPlayer(void);
void ResetPlayer(void);




#endif // SCREENS_H