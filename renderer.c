#include "renderer.h"

void renderer_InitRenderObject(RenderObject* ro, Vector3 blb, Vector3 trf, fpRenderObjectDraw draw_cb, fpRenderObjectFree free_cb) {
    ro->Draw = draw_cb;
    ro->Free = free_cb;
    ro->box.blb = blb;
    ro->box.trf = trf;
}

char renderer_AABBZTest(AABB* box1, AABB* box2) {
    
    // For now, let's just check the BLB
    //  TODO: real AABB Depth Test
    
    //printf("Compare AABB:\n");
    //printf("\t (%.2f, %.2f, %.2f)\n", box1->blb.x, box1->blb.y, box1->blb.z);
    //printf("\t (%.2f, %.2f, %.2f)\n", box2->blb.x, box2->blb.y, box2->blb.z);
    
    // Check X first
    if (box1->blb.z < box2->blb.z) {
        return -1;
    } else if (box1->blb.z == box2->blb.z) {
        // Z is the same, we need to test x
        if (box1->blb.x < box2->blb.x) {
            return -1;
        } else if (box1->blb.x == box2->blb.x) {
            // ZX are the same, we need to test Y
            if (box1->blb.y < box2->blb.y) {
                return -1;
            } else if (box1->blb.y == box2->blb.y) {
                // All the same
                
                // Now lets test Height
                //if (box1->trf.y > )
                
                return 0;
            }
        }
    }
    
    return 1;
}

void renderer_Add(RenderList* rl, RenderObject* obj) {
    //TODO: Figure out how to sort?
    
    // For now, add to end?
    
    RenderList* new_node = (RenderList*)malloc(sizeof(RenderList));
    new_node->obj = obj;
    
    
    if (rl->prev == 0 && rl->next == 0) {
        // This is a new list
        printf("adding first RenderObject\n");
        rl->next = new_node;
        rl->prev = new_node;
        
        new_node->prev = rl;
        new_node->next = rl;
    } else {
        // This is not new
        
        
        RenderList* tmp = rl;
        int idx = 0;
        char res = renderer_AABBZTest(&obj->box, &tmp->prev->obj->box);
        while ((tmp->next != rl) && (renderer_AABBZTest(&obj->box, &tmp->next->obj->box) > 0)) {
            tmp = tmp->next;
            idx++;
        }
        
        if (tmp->next != rl) {
            tmp = tmp->next;
        }
        
        printf("adding another RenderObject (SORTED) [%d]\n", idx);
        
        // Point the End node to our new node
        
        tmp->prev->next = new_node;
        
        new_node->prev = tmp->prev; // EndNode->prev points to old EndNode
        new_node->next = tmp; // End node points to start
        
        tmp->prev = new_node; // StartNode-prev points to newly appended node
        
    }
}

RenderList* renderer_CreateRenderList(void) {
    RenderList* rl_node = (RenderList*)malloc(sizeof(RenderList));
    rl_node->obj = 0;
    rl_node->next = 0;
    rl_node->prev = 0;
    
    return rl_node;
}

void renderer_FreeRenderList(RenderList* rl) {
    printf("renderer_FreeRenderList NOT IMPLEMENTED YET. (LEAKING MEMORY LIKE A BOSS)\n");
}


// Simply Loop the sorted List calling Draw
void renderer_Render(RenderList* rl) {
    RenderList* cur = rl;
    
    //printf("%d -> %d [NEXT] = %d\n", cur, cur->obj, cur->next);
    
    cur = cur->next;
    while (1) {
        //printf("%d -> %d [NEXT] = %d\n", cur, cur->obj, cur->next);
        if (cur->obj == 0) 
            break;
        
        cur->obj->Draw((void*)cur->obj);
        cur = cur->next;
    }
}