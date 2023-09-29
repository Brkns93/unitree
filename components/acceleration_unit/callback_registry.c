
#include <callback_registry.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


struct callback_registry_t cb_create(){
    struct callback_registry_t cb;
    memset(&cb, 0, sizeof(struct callback_registry_t));
    return cb;
}

int cb_register_callback(struct callback_registry_t* cb, const struct callback_t* callback){
    if(cb->count == 0){
        cb->cbs = malloc(sizeof(struct callback_t));
    }
    else{
        cb->cbs = realloc(cb->cbs, sizeof(struct callback_t)*(cb->count+1));
    }
    if(cb->cbs == NULL){
        return -1;
    }
    memcpy(cb->cbs+cb->count, callback, sizeof(struct callback_t));
    cb->count++;
    
    return 0;
}

struct callback_t *cb_get_callback_by_id(struct callback_registry_t* cbr, const uint16_t id)
{
    for (int i = 0; i < cbr->count; ++i)
    {
        if (cbr->cbs[i].id == id)
        {
            return &(cbr->cbs[i]);
        }
    }

    return NULL;
}
