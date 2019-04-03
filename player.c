#include "player.h"

// Load player module data

// Texture

void InitPlayer(void) {
    player.position = (Vector3) {1.0f, 0.0f, 2.0f};
    player.texture = LoadTexture("resources/textures/vampire.png");
    player.color = RAYWHITE;
    
    // Animation variables
    player.frameRec = (Rectangle) {0.0f, 0.0f, 64.0f, 64.0f};
    player.currentFrame = 0;
    
    player.isMoving = 0;
    player.targetPos = player.position;
}

void UpdatePlayer(float delta) {
    
    
    if (IsKeyDown(KEY_S)) {
        player.position.z += 1.5f * delta;
    }
    
    if (IsKeyDown(KEY_W)) {
        player.position.z -= 1.5f * delta;
    }
    
    if (IsKeyDown(KEY_D)) {
        player.position.x += 1.5f * delta;
    }
    
    if (IsKeyDown(KEY_A)) {
        player.position.x -= 1.5f * delta;
    }

}
void DrawPlayer(void) {
    rlEnableTexture(player.texture.id);
    rlBegin(RL_QUADS);
    
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0.0f, 0.0f, 1.0f);
    
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(player.position.x, 0.0f, player.position.z + 0.5f);

    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(player.position.x + 0.75f, 0.0f, player.position.z + 0.5f);

    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(player.position.x + 0.75f, 0.75f, player.position.z + 0.5f);

    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(player.position.x, 0.75f, player.position.z + 0.5f);
    
    rlEnd();
    rlDisableTexture();
}

void UnloadPlayer(void) {
    
}

void ResetPlayer(void) {
    
}
