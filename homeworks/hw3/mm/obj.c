#include "obj.h"
#include "mm/frame_alloc.h"

void* object_alloc(obj_alloc_t* alloc) {
    return frame_alloc();
}

void object_free(obj_alloc_t* alloc, void* obj) {
    frame_free(obj);
}
