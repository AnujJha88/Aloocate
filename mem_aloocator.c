#include <unistd.h>
#include<stdbool.h>
typedef struct block_meta{
size_t size;
struct block_meta *next;
    bool is_free;

}block_meta_t;

#define META_SIZE sizeof(block_meta_t)

void *global_base=NULL;

block_meta_t *request_space( block_meta_t* last,size_t size){
    // we ask the OS for some space here
    // this is where we need the unistd header cuz we need to call the sbrk
    // method to beg for memory from the oS
    block_meta_t* block;
    block=(block_meta_t *)sbrk(0);// gives current end of the heap

    void* request=sbrk(META_SIZE+size);
    if ((void*)-1 ==request)return NULL;// heap ended so we dont got any space

    if(last){
        (last)->next=block;

    }

    block->size=size;
    block->is_free=false;
    block->next=NULL;


    return block;
}

//first fit strat-> what you do is you find the first block large enough for
//your purposes and then you just allocate that

block_meta_t* find_first_free( size_t size){
    block_meta_t *current=global_base;
    while(current){
        if(current->is_free && current->size>= size){
            return current;
        }
        current=current->next;
    }
    return NULL;
}

block_meta_t* find_last_block(){
    if(!global_base) return NULL;//what else will you do with a null global base

    block_meta_t* current=global_base;

    while(current->next){
        current=current->next;
    }
    return current;
}

void* my_malloc(int size){

    if(size<=0) return NULL;
    block_meta_t *new_mem;
    if(!global_base){
        // so this is our first allocation
        new_mem=request_space(NULL,size);
        if(!new_mem)return NULL;
        global_base=new_mem;


    }
    else{
        new_mem=find_first_free(size);
        if(new_mem){
            //TODO when we have a valid block available
            new_mem->is_free=0;// will do splitting stuff later

        }

        else{
            block_meta_t* last=find_last_block();
            new_mem=request_space(last,size);
            if(!new_mem) return NULL;
        }

    }
    return new_mem+1;
}

