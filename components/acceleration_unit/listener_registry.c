
#include <listener_registry.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


struct listener_registry_t lr_create(){
    struct listener_registry_t elr;
    memset(&elr, 0, sizeof(struct listener_registry_t));
    return elr;
}

int lr_register_listener(struct listener_registry_t* elr, const struct listener_t* listener){
    if(elr->listener_count == 0){
        elr->listeners = malloc(sizeof(struct listener_t));
    }
    else{
        elr->listeners = realloc(elr->listeners, sizeof(struct listener_t)*(elr->listener_count+1));
    }
    if(elr->listeners == NULL){
        return -1;
    }
    memcpy(elr->listeners+elr->listener_count, listener, sizeof(struct listener_t));
    elr->listener_count++;
    
    return 0;
}
