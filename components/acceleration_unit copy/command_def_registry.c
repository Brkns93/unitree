#include <command_def_registry.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/kernel.h>


struct command_def_registry_t cdr_create(){
    struct command_def_registry_t cbr;
    memset(&cbr, 0, sizeof(struct command_def_registry_t));
    return cbr;
}

int cdr_register_command_def(struct command_def_registry_t* cbr, const struct command_def_t* command){
    
	printk("Registering callback.\n");
	printk("cbr->cb_count = %d\n", cbr->cb_count);
    if(cbr->cb_count == 0){
        cbr->cbs = malloc(sizeof(struct command_def_t));
	    printk("cbr->cb_count = %d\n", cbr->cb_count);
    }
    else{
        cbr->cbs = realloc(cbr->cbs, sizeof(struct command_def_t)*(cbr->cb_count+1));
    }
    if(cbr->cbs == NULL){
        return -1;
    }
	printk("cbr->cb_count = %d\n", cbr->cb_count);
    memcpy(cbr->cbs+cbr->cb_count, command, sizeof(struct command_def_t));
    cbr->cb_count++;
	printk("cbr->cb_count = %d\n", cbr->cb_count);
    
    return 0;
}

struct command_def_t *cdr_get_command_def_by_id(struct command_def_registry_t* cbr, const uint16_t cmd_id)
{
    for (int i = 0; i < cbr->cb_count; ++i)
    {
        if (cbr->cbs[i].id == cmd_id)
        {
            return &(cbr->cbs[i]);
        }
    }

    return NULL;
}
