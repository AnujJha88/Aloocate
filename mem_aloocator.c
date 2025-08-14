#include <unistd.h>
#include<stdbool.h>
typedef struct block_meta{
size_t size;
struct block_meta *next;
    bool is_free;

}block_meta_t;

#define META_SIZE sizeof(block_meta_t)

void *global_base=NULL;

//first fit strat-> what you do is you find the first block large enough for
//your purposes and then you just allocate that

block_meta_t find_first_free(block_meta_t **last, size_t size){
    block_meta_t *current=*last;
    while(current){
        if(current->is_free && current->size>= size){
            return current;
        }
        current=current->next;
    }
    return NULL;
}

void* my_malloc(int size){

    if(size<=0) return NULL;
    block_meta_t *new_mem;
    if(!global_base){
        //TODO when we have no base we need to ask OS for memory to then use for
        //our purposes
    }
    else{
        new_mem=find_first_free(&global_base,size);
        if(new_mem){
            //TODO when we have a valid block available
            new_mem->is_free=0;// will do splitting stuff later
        }
        else{
            //TODO when we have no block available
            //need bigger heap
        }

    }
    return new_mem+1;
}

