#include "screen_splashmv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rlgl.h"

#include "external/glad.h"

#include "player.h"
#include "renderer.h"

// Create the screen struct for this screen and add it to the screens
Screen Screen_SplashMV;
static char Screen_SplashMV_Initialized = 0;

void Screen_SplashMV_Init(void);
void Screen_SplashMV_Update(float delta);
void Screen_SplashMV_Draw(void);
void Screen_SplashMV_Load(void);
void Screen_SplashMV_Unload(void);
void Screen_SplashMV_Finish(void);

static Camera camera;


// One of ground tiles
#define NONE_T  0b0000
#define DIRT_T  0b0001
#define GRASS_T 0b0010
#define BRICK_T 0b0011

//Verticle tiles (none or more) or'd with ground tile and more verticle
// Maybe this should be a UInt16 with a nibble (4bit) for direction,
// with and byte for selecting the tile to place with that direction (rotation)
#define FENCE_VT 0b000100000000

#define N_VT 0b00010000
#define S_VT 0b00100000
#define W_VT 0b01000000
#define E_VT 0b10000000

// TODO: ADD 2bit rotation for the ground tile

static char levelWidth = 15;
static unsigned int level[] = {
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | N_VT | FENCE_VT | DIRT_T,  N_VT | FENCE_VT | DIRT_T,  N_VT | FENCE_VT | DIRT_T,  N_VT | FENCE_VT | DIRT_T, N_VT | FENCE_VT | DIRT_T,  N_VT | E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | FENCE_VT | DIRT_T,  DIRT_T,  DIRT_T, DIRT_T, DIRT_T, E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | FENCE_VT | DIRT_T,  DIRT_T,  DIRT_T, DIRT_T, DIRT_T, E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | FENCE_VT | DIRT_T,  DIRT_T,  DIRT_T, DIRT_T, DIRT_T, E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | FENCE_VT | DIRT_T,  DIRT_T,  DIRT_T, DIRT_T, DIRT_T, E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | FENCE_VT | DIRT_T,  DIRT_T,  DIRT_T, DIRT_T, DIRT_T, E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, DIRT_T,  DIRT_T,  DIRT_T,  DIRT_T, DIRT_T, E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, W_VT | S_VT | FENCE_VT | DIRT_T,  S_VT | FENCE_VT | DIRT_T,  S_VT | FENCE_VT | DIRT_T,  S_VT | FENCE_VT | DIRT_T, S_VT | FENCE_VT | DIRT_T, S_VT | E_VT | FENCE_VT | DIRT_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T,
    GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T, GRASS_T
};


#define NONE_C 0b0000
#define ARM_C 0b0001

// Crop variables
static Texture2D crop_arm;

static RenderList* scene = 0;


//TEMP: Create struct for our tile objects
typedef struct TileRO {
    RenderObject ro;
    unsigned int info;
} TileRO;

typedef struct VTileRO {
    RenderObject ro;
    unsigned int info;
} VTileRO;

typedef struct CTileRO {
    RenderObject ro;
    unsigned int info;
} CTileRO;


// For time sake, for now we will have a single animation timer for crops
// later if time permits, it should be a crop struct which manages each
// peice of data about the crop piece

static unsigned int levelCropRow = 3;
static unsigned int levelCropCol = 3;

static unsigned int levelCropWidth = 6;
static unsigned int levelCrops[] = {
    NONE_C, ARM_C, NONE_C, NONE_C, NONE_C, NONE_C, 
    NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, 
    NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, 
    NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, 
    NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, 
    NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, 
    NONE_C, NONE_C, NONE_C, NONE_C, NONE_C, NONE_C
};

static unsigned char cropAnimFrame = 1;
static float cropAnimFrameTime = 0.0f;
static float cropAnimSpeed = 6.0f; // Frames per second
static Rectangle cropAnimRect;
static Vector3 cropOrigin = (Vector3) { 0.0f, 0.0f, 0.0f };



static void DrawTileRO(void* obj) {
    TileRO* tile = (TileRO*)obj;
    
    // Bitwise!!! and ops priority DOE....
    if ((tile->info & GRASS_T) == GRASS_T) {
        rlEnableTexture(grass_tile.id);
    } else if ((tile->info & DIRT_T) == DIRT_T) {
        rlEnableTexture(dirt_tile.id);
    }

    
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0.0f, 1.0f, 0.0f);

    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z + 1.0f);

    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(tile->ro.box.blb.x + 1.0f, 0.0f, tile->ro.box.blb.z + 1.0f);

    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(tile->ro.box.blb.x + 1.0f, 0.0f, tile->ro.box.blb.z);

    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z);

    //printf("render tile {%f, %f, %f}\n", tile->ro.box.blb.x, tile->ro.box.blb.y, tile->ro.box.blb.z);

    rlDisableTexture();
}

static void DrawVTileRO(void* obj) {
    VTileRO* tile = (VTileRO*)obj;
    
    glDisable(GL_CULL_FACE);
    
    if ((tile->info & FENCE_VT) == FENCE_VT) {
        rlEnableTexture(fence_vtile.id);
    }
    
    if ((tile->info & N_VT) == N_VT) {
        //rlBegin(RL_QUADS);
            rlColor4ub(255, 255, 255, 255);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlTexCoord2f(0.0f, 1.0f);
            rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z);

            rlTexCoord2f(1.0f, 1.0f);
            rlVertex3f(tile->ro.box.blb.x + 1.0f, 0.0f, tile->ro.box.blb.z);

            rlTexCoord2f(1.0f, 0.0f);
            rlVertex3f(tile->ro.box.blb.x + 1.0f, 1.0f, tile->ro.box.blb.z);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(tile->ro.box.blb.x, 1.0f, tile->ro.box.blb.z);
        //rlEnd();
    }
    
    if ((tile->info & E_VT) == E_VT) {
        //rlBegin(RL_QUADS);
            rlColor4ub(255, 255, 255, 255);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlTexCoord2f(0.0f, 1.0f);
            rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z + 1.0f);

            rlTexCoord2f(1.0f, 1.0f);
            rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z);

            rlTexCoord2f(1.0f, 0.0f);
            rlVertex3f(tile->ro.box.blb.x, 1.0f, tile->ro.box.blb.z);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(tile->ro.box.blb.x, 1.0f, tile->ro.box.blb.z + 1.0f);
        //rlEnd();
    }
    
    
    if ((tile->info & W_VT) == W_VT) {
        //rlBegin(RL_QUADS);
            rlColor4ub(255, 255, 255, 255);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlTexCoord2f(0.0f, 1.0f);
            rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z + 1.0f);

            rlTexCoord2f(1.0f, 1.0f);
            rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z);

            rlTexCoord2f(1.0f, 0.0f);
            rlVertex3f(tile->ro.box.blb.x, 1.0f, tile->ro.box.blb.z);

            rlTexCoord2f(0.0f, 0.0f);
            rlVertex3f(tile->ro.box.blb.x, 1.0f, tile->ro.box.blb.z + 1.0f);
        //rlEnd();
    }
   
    if ((tile->info & S_VT) == S_VT) {
        //rlBegin(RL_QUADS);
        rlColor4ub(255, 255, 255, 255);
        rlNormal3f(0.0f, 0.0f, 1.0f);
        
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(tile->ro.box.blb.x, 0.0f, tile->ro.box.blb.z);

        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(tile->ro.box.blb.x + 1.0f, 0.0f, tile->ro.box.blb.z);

        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(tile->ro.box.blb.x + 1.0f, 1.0f, tile->ro.box.blb.z);

        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(tile->ro.box.blb.x, 1.0f, tile->ro.box.blb.z);
        //rlEnd();
    }
    
    
    rlDisableTexture();
}

static void DrawCTileRO(void* obj) {
    CTileRO* tile = (CTileRO*)obj;
    
    //TODO: Figure out animation
    
    //Draw the crop
    
    
    cropAnimRect.x = 0;
    cropAnimRect.y = 0;
    cropAnimRect.width = 64;
    cropAnimRect.height = 64;
    
    rlEnableTexture(crop_arm.id);
    
    //rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0.0f, 0.0f, 1.0f);

    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * ((float)cropAnimFrame - 1), 1.0f);
    rlVertex3f(tile->ro.box.blb.x, tile->ro.box.blb.y, tile->ro.box.blb.z);

    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * (float)cropAnimFrame, 1.0f);
    rlVertex3f(tile->ro.box.trf.x, tile->ro.box.blb.y, tile->ro.box.blb.z);

    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * (float)cropAnimFrame, 0.0f);
    rlVertex3f(tile->ro.box.trf.x, tile->ro.box.trf.y, tile->ro.box.blb.z);

    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * ((float)cropAnimFrame - 1), 0.0f);
    rlVertex3f(tile->ro.box.blb.x, tile->ro.box.trf.y, tile->ro.box.blb.z);
    //rlEnd();
    
    rlDisableTexture();
}

static void FreeTileRO(void* obj) {
    TileRO* tile = (TileRO*)obj;
    
    // Free the tile (it doesn't contain any pointers, so it should free correctly)
    free(tile);
}

static void FreeVTileRO(void* obj) {
    VTileRO* tile = (VTileRO*)obj;
    
    // Free the tile (it doesn't contain any pointers, so it should free correctly)
    free(tile);
}

static void FreeCTileRO(void* obj) {
    CTileRO* tile = (CTileRO*)obj;
    
    // Free the tile (it doesn't contain any pointers, so it should free correctly)
    free(tile);
}

static void CreateTile(RenderList* scene, Vector3 pos, unsigned int info) {
    Vector3 blb = pos;
    Vector3 trf = (Vector3) {pos.x + 1.0f, pos.y, pos.z + 1.0f};
    
    TileRO* tile = (TileRO*)malloc(sizeof(TileRO));
    renderer_InitRenderObject((RenderObject*)tile, blb, trf, DrawTileRO, FreeTileRO);
    
    tile->info = info;
    
    renderer_Add(scene, (RenderObject*)tile);
}

static void CreateVTiles(RenderList* scene, Vector3 pos, unsigned int info) {
    Vector3 blb; // = pos;
    Vector3 trf; // = (Vector3) {pos.x + 1.0f, pos.y, pos.z + 1.0f};
    VTileRO* tile;
    
    if ((info & N_VT) == N_VT) {
        blb = pos;
        trf = (Vector3) {pos.x + 1.0f, pos.y + 1.0f, pos.z};
        
        tile = (VTileRO*)malloc(sizeof(VTileRO));
        renderer_InitRenderObject((RenderObject*)tile, blb, trf, DrawVTileRO, FreeVTileRO);
        
        tile->info = info & (~E_VT & ~S_VT & ~W_VT);
        
        renderer_Add(scene, (RenderObject*)tile);
    }
    
    if ((info & E_VT) == E_VT) {
        blb = (Vector3) {pos.x + 1.0f, pos.y, pos.z};
        trf = (Vector3) {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f};
        
        tile = (VTileRO*)malloc(sizeof(VTileRO));
        renderer_InitRenderObject((RenderObject*)tile, blb, trf, DrawVTileRO, FreeVTileRO);
        
        tile->info = info & (~N_VT & ~S_VT & ~W_VT);
        
        renderer_Add(scene, (RenderObject*)tile);
    }
    
    
    if ((info & W_VT) == W_VT) {
        blb = pos;
        trf = (Vector3) {pos.x, pos.y + 1.0f, pos.z + 1.0f};
        
        tile = (VTileRO*)malloc(sizeof(VTileRO));
        renderer_InitRenderObject((RenderObject*)tile, blb, trf, DrawVTileRO, FreeVTileRO);
        
        tile->info = info & (~N_VT & ~S_VT & ~E_VT);
        
        renderer_Add(scene, (RenderObject*)tile);
    }
    
    if ((info & S_VT) == S_VT) {
        blb = (Vector3) {pos.x, pos.y, pos.z + 1.0f};
        trf = (Vector3) {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f};
        
        tile = (VTileRO*)malloc(sizeof(VTileRO));
        renderer_InitRenderObject((RenderObject*)tile, blb, trf, DrawVTileRO, FreeVTileRO);
        
        tile->info = info & (~N_VT & ~E_VT & ~W_VT);
        
        renderer_Add(scene, (RenderObject*)tile);
    }
}

static void CreateCTile(RenderList* scene, Vector3 pos, unsigned int info) {
    Vector3 blb = (Vector3) {pos.x + 0.2f, pos.y, pos.z + 0.5f};
    Vector3 trf = (Vector3) {pos.x + 0.8f, pos.y + 0.6f, pos.z + 0.5f};
    
    CTileRO* tile = (CTileRO*)malloc(sizeof(CTileRO));
    renderer_InitRenderObject((RenderObject*)tile, blb, trf, DrawCTileRO, FreeCTileRO);
    
    tile->info = info;
    
    renderer_Add(scene, (RenderObject*)tile);
}

static void DrawLevel(void) {
    
    int row = 0;
    int col = 0;
    
    rlBegin(RL_QUADS);
    for (row = 0; row < (sizeof(level) / sizeof(int)) / levelWidth; row++) {
        for (col = 0; col < levelWidth; col++) {
            int idx = row * levelWidth + col;
            
            if (level[idx] > 0) {
                //printf("drawing %d, %d @ %d\n", row, col, idx);
                // We don't draw null squares
                
                // Bitwise!!! and ops priority DOE....
                if ((level[idx] & GRASS_T) == GRASS_T) {
                    rlEnableTexture(grass_tile.id);
                } else if ((level[idx] & DIRT_T) == DIRT_T) {
                    rlEnableTexture(dirt_tile.id);
                }

                
                    rlColor4ub(255, 255, 255, 255);
                    rlNormal3f(0.0f, 1.0f, 0.0f);

                    rlTexCoord2f(0.0f, 1.0f);
                    rlVertex3f((float)col, 0.0f, (float)row + 1.0f);

                    rlTexCoord2f(1.0f, 1.0f);
                    rlVertex3f((float)col + 1.0f, 0.0f, (float)row + 1.0f);

                    rlTexCoord2f(1.0f, 0.0f);
                    rlVertex3f((float)col + 1.0f, 0.0f, (float)row);

                    rlTexCoord2f(0.0f, 0.0f);
                    rlVertex3f((float)col, 0.0f, (float)row);
                

                rlDisableTexture();

            }
        }
    }
    
    //printf("%d\n", );
    int cropLastRow = levelCropRow + (sizeof(levelCrops) / sizeof(int)) / levelCropWidth;
    int cropLastCol = levelCropCol + levelCropWidth - 1;
    char playerDeferred = 0;
    // TEMP Render crops
    for (row = 0; row < (sizeof(level) / sizeof(int)) / levelWidth; row++) {
        for (col = 0; col < levelWidth; col++) {
            int idx = row * levelWidth + col;
            
            
            if (level[idx] > 0) {
                // We don't draw null squares
                
                // DRAW VERTICLE TILES?
                // Each direction N|E|W|S _VT might need to be a seprate pass (Or fix camera and only move along XZ Plane)
                //TODO: Allow for multiple tiles at once on a single coord
                glDisable(GL_CULL_FACE);
                char do_vert = 0;
                if ((level[idx] & FENCE_VT) == FENCE_VT) {
                    rlEnableTexture(fence_vtile.id);
                    do_vert = 1;
                }
                
                if (do_vert) {
                    if ((level[idx] & N_VT) == N_VT) {
                        //rlBegin(RL_QUADS);
                            rlColor4ub(255, 255, 255, 255);
                            rlNormal3f(0.0f, 0.0f, 1.0f);

                            rlTexCoord2f(0.0f, 1.0f);
                            rlVertex3f((float)col, 0.0f, (float)row);

                            rlTexCoord2f(1.0f, 1.0f);
                            rlVertex3f((float)col + 1.0f, 0.0f, (float)row);

                            rlTexCoord2f(1.0f, 0.0f);
                            rlVertex3f((float)col + 1.0f, 1.0f, (float)row);

                            rlTexCoord2f(0.0f, 0.0f);
                            rlVertex3f((float)col, 1.0f, (float)row);
                        //rlEnd();
                    }
                    
                    if ((level[idx] & W_VT) == W_VT) {
                        //rlBegin(RL_QUADS);
                            rlColor4ub(255, 255, 255, 255);
                            rlNormal3f(0.0f, 0.0f, 1.0f);

                            rlTexCoord2f(0.0f, 1.0f);
                            rlVertex3f((float)col, 0.0f, (float)row + 1.0f);

                            rlTexCoord2f(1.0f, 1.0f);
                            rlVertex3f((float)col, 0.0f, (float)row);

                            rlTexCoord2f(1.0f, 0.0f);
                            rlVertex3f((float)col, 1.0f, (float)row);

                            rlTexCoord2f(0.0f, 0.0f);
                            rlVertex3f((float)col, 1.0f, (float)row + 1.0f);
                        //rlEnd();
                    }
                    
                    if ((level[idx] & E_VT) == E_VT) {
                        //rlBegin(RL_QUADS);
                            rlColor4ub(255, 255, 255, 255);
                            rlNormal3f(0.0f, 0.0f, 1.0f);

                            rlTexCoord2f(0.0f, 1.0f);
                            rlVertex3f((float)col + 1.0f, 0.0f, (float)row + 1.0f);

                            rlTexCoord2f(1.0f, 1.0f);
                            rlVertex3f((float)col + 1.0f, 0.0f, (float)row);

                            rlTexCoord2f(1.0f, 0.0f);
                            rlVertex3f((float)col + 1.0f, 1.0f, (float)row);

                            rlTexCoord2f(0.0f, 0.0f);
                            rlVertex3f((float)col + 1.0f, 1.0f, (float)row + 1.0f);
                        //rlEnd();
                    }
                }
            }
            
            
            
            // Crops
            if ((row >= levelCropRow && col >= levelCropRow) && (row <= cropLastRow && col <= cropLastCol)) {
                // tmp draw arm as billboard
                cropOrigin.x = col;
                cropOrigin.y = 0.5f;
                cropOrigin.z = row;
                
                cropAnimRect.x = 0;
                cropAnimRect.y = 0;
                cropAnimRect.width = 64;
                cropAnimRect.height = 64;
                
                rlEnableTexture(crop_arm.id);
                
                //rlBegin(RL_QUADS);
                    rlColor4ub(255, 255, 255, 255);
                    rlNormal3f(0.0f, 0.0f, 1.0f);

                    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * ((float)cropAnimFrame - 1), 1.0f);
                    rlVertex3f((float)col + 0.25f, 0.0f, (float)row + 0.5f);

                    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * (float)cropAnimFrame, 1.0f);
                    rlVertex3f((float)col + 0.25f + 0.5f, 0.0f, (float)row + 0.5f);

                    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * (float)cropAnimFrame, 0.0f);
                    rlVertex3f((float)col + 0.25f + 0.5f, 0.5f, (float)row + 0.5f);

                    rlTexCoord2f((1.0f / (crop_arm.width / 64.0f)) * ((float)cropAnimFrame - 1), 0.0f);
                    rlVertex3f((float)col + 0.25f, 0.5f, (float)row + 0.5f);
                //rlEnd();
                
                rlDisableTexture();
                //DrawBillboardRec(camera, crop_arm, cropAnimRect, cropOrigin, 1.0f, RAYWHITE);
            }
            
            if ((player.position.x >= col + 1 && player.position.x <= col + 2) &&
                (player.position.z >= row && player.position.z <= row + 1)) {
                DrawPlayer();
            }
            
            
            // South Verticle Tiles
            if (level[idx] > 0) {
                // We don't draw null squares
                
                // DRAW VERTICLE TILES?
                // Each direction N|E|W|S _VT might need to be a seprate pass (Or fix camera and only move along XZ Plane)
                //TODO: Allow for multiple tiles at once on a single coord
                glDisable(GL_CULL_FACE);

                if ((level[idx] & FENCE_VT) == FENCE_VT) {
                    rlEnableTexture(fence_vtile.id);
                
                    if ((level[idx] & S_VT) == S_VT) {
                        //rlBegin(RL_QUADS);
                        rlColor4ub(255, 255, 255, 255);
                        rlNormal3f(0.0f, 0.0f, 1.0f);
                        
                        rlTexCoord2f(0.0f, 1.0f);
                        rlVertex3f((float)col, 0.0f, (float)row + 1.0f);

                        rlTexCoord2f(1.0f, 1.0f);
                        rlVertex3f((float)col + 1.0f, 0.0f, (float)row + 1.0f);

                        rlTexCoord2f(1.0f, 0.0f);
                        rlVertex3f((float)col + 1.0f, 1.0f, (float)row + 1.0f);

                        rlTexCoord2f(0.0f, 0.0f);
                        rlVertex3f((float)col, 1.0f, (float)row + 1.0f);
                        //rlEnd();
                    }

                    rlDisableTexture();
                }
            }
            
            
        }
    }
    
    
    rlEnd();
    
    
    
    
}


void Screen_SplashMV_Create(void) {
	printf("Screen_SplashMV_Create\n");
	strcpy(Screen_SplashMV.screenId, "splashmv");
	Screen_SplashMV.Init = &Screen_SplashMV_Init;
	Screen_SplashMV.Update = &Screen_SplashMV_Update;
	Screen_SplashMV.Draw = &Screen_SplashMV_Draw;
	Screen_SplashMV.Load = &Screen_SplashMV_Load;
	Screen_SplashMV.Unload = &Screen_SplashMV_Unload;
	Screen_SplashMV.Finish = &Screen_SplashMV_Finish;
	printf("Screen_SplashMV_Create ENDED\n");
}

Screen* Screen_SplashMV_GetScreen(void) {
	if (Screen_SplashMV_Initialized == 0) {
		Screen_SplashMV_Create();
        Screen_SplashMV_Init();
		Screen_SplashMV_Initialized = 1;
	}

	return &Screen_SplashMV;
}

void Screen_SplashMV_Init(void) {
	printf("Screen Initialized!\n");
	camera.fovy = 45.0;
	camera.position = (Vector3) {7.0f, 7.0f, 15.0f};
	camera.target = (Vector3) {7.0f, 0.0f, 7.0f};
	camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
	camera.type = CAMERA_PERSPECTIVE;
    
    SetCameraMode(camera, CAMERA_CUSTOM);
    
    
    
    // Load screen textures
    crop_arm = LoadTexture("resources/textures/arm_crop_anim.png");
    
    
    
    // Put Level in the renderlist
    scene = renderer_CreateRenderList();
    
    // Loop the tiles putting them into the scene
    int row = 0;
    int col = 0;
    
    int cropLastRow = levelCropRow + (sizeof(levelCrops) / sizeof(int)) / levelCropWidth;
    int cropLastCol = levelCropCol + levelCropWidth - 1;
    
    for (row = 0; row < (sizeof(level) / sizeof(int)) / levelWidth; row++) {
        for (col = 0; col < levelWidth; col++) {
            int idx = row * levelWidth + col;
            Vector3 pos = (Vector3) {(float)col, 0.0f, (float)row};
            
            if (level[idx] > 0) {
                CreateTile(scene, pos, level[idx]);
                CreateVTiles(scene, pos, level[idx]);
            }
            
            // Process Crops
            if ((row >= levelCropRow && col >= levelCropRow) 
                && (row <= cropLastRow && col <= cropLastCol)) {
                
                int crop_idx = (row - levelCropRow) * levelCropWidth + (col - levelCropCol);
                
                CreateCTile(scene, pos, levelCrops[crop_idx]);
                
            }
        }
    }
}

static Vector3 targetPos = (Vector3) { 7.0f, 7.0f, 15.0f };
static char camMoving = 0;

void Screen_SplashMV_Update(float delta) {
    UpdateCamera(&camera);
    
    UpdatePlayer(delta);
    
    // Snap to if close enough
    //printf("Distance: %f.2 \n", Vector3Distance(camera.position, targetPos));
    if (Vector3Distance(camera.position, targetPos) < 0.1f ) {
        camera.position = targetPos;
        camMoving = 0;
    }
    
    // Check for input to move the camera
    if (IsKeyDown(KEY_I)) {
        targetPos.x = 15.0f;
        camMoving = 1;
    }
    
    if (IsKeyDown(KEY_J)) {
        targetPos.x = 7.0f;
        camMoving = 1;
    }
    
    
    if (camMoving)
        camera.position = Vector3Lerp(camera.position, targetPos, delta);
    
    // Later camera will just follow player around
    
    cropAnimFrameTime += delta;
    if (cropAnimFrameTime >= 1.0f / cropAnimSpeed) {
        cropAnimFrameTime = 0.0f;
        cropAnimFrame++;
        
        if (cropAnimFrame > (int)crop_arm.width / 64.0f) {
            cropAnimFrame = 1;
        }
    }
}



void Screen_SplashMV_Draw(void) {


    BeginMode3D(camera);
    
        

        //DrawLevel();
        
        //DrawGrid(20, 1.0f);
        rlBegin(RL_QUADS);
        renderer_Render(scene);
        rlEnd();
    EndMode3D();

    DrawFPS(10, 10);

}

void Screen_SplashMV_Load(void) {

}

void Screen_SplashMV_Unload(void) {

}

void Screen_SplashMV_Finish(void) {

	return 1;
}
