#include "obj.h"
#include "common.h"
#include "mm/frame_alloc.h"

void* object_alloc(obj_alloc_t* alloc) {
    UNUSED(alloc);
    return frame_alloc();
}

void object_free(obj_alloc_t* alloc, void* obj) {
    UNUSED(alloc);
    frame_free(obj);
}
