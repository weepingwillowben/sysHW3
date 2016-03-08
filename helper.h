#pragma once
#include <stdlib.h>
#include "cache.h"

void c_delete(void ** ptr);
void * make_copy(const void * buffer,size_t buffsize);

struct val_item{
    void * value;
    uint32_t val_size;
};
typedef struct val_item val_s;
