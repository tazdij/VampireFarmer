#ifndef __RENDERER_H_
#define __RENDERER_H_

#include "raylib.h"
#include "rlgl.h"

                 
typedef struct { 
    Vector3 blb; // Bottom Left Back (Origin for this game)
    Vector3 trf; // Top Right Front
} AABB;


typedef void (*fpRenderObjectDraw)(void* object);
typedef void (*fpRenderObjectFree)(void* object);

typedef struct {
    AABB box;
    fpRenderObjectDraw Draw;
    fpRenderObjectFree Free;
} RenderObject;


// DoublyLinkedList (performance doesn't matter for now) (NO LOOPING?)
//  sorted on insert ()
typedef struct RenderList {
    struct RenderList* prev;
    struct RenderList* next;
    RenderObject* obj;
} RenderList;

void renderer_InitRenderObject(RenderObject* ro, Vector3 blb, Vector3 trf, fpRenderObjectDraw draw_cb, fpRenderObjectFree free_cb);

char renderer_AABBZTest(AABB* box1, AABB* box2); // returns -1, 0, 1 (box1's relation to box2)
void renderer_Add(RenderList* rl, RenderObject* obj); // Add RenderObject to the RenderList
void renderer_Update(RenderList* rl, RenderObject* obj); // Update the position of the object in the RenderList


RenderList* renderer_CreateRenderList(void);
void renderer_FreeRenderList(RenderList* rl); // Loop freeing all RenderObject and then each RenderList
void renderer_Render(RenderList* rl);

#endif