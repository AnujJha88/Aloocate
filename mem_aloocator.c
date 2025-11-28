

#include <stddef.h>   // For size_t and NULL
#include <stdbool.h>
#include <unistd.h>   // For sbrk()
#include <string.h>   // For memset()
#define ALIGNMENT 8
size_t aling8(size_t s){
    return (s+7)&~7;//rounds up to a multiple of 8
}

typedef struct block_meta{
size_t size;
struct block_meta *next;
struct block_meta *prev;
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
    block->prev=last;


    return block;
}

//first fit strat-> what you do is you find the first block large enough for
//your purposes and then you just allocate that

block_meta_t* find_first_free( size_t size){
    block_meta_t *current=(block_meta_t*)global_base;
    while(current){
        if(current->is_free && current->size>= size){
            return current;
        }
        current=current->next;
    }
    return NULL;
}

//best fit this tries to be very very efficient
block_meta_t *find_best_fit(size_t size){

}

block_meta_t* find_last_block(){
    if(!global_base) return NULL;//what else will you do with a null global base

    block_meta_t* current=(block_meta_t*)global_base;

    while(current->next){
        current=current->next;
    }
    return current;
}

void split_block(block_meta_t *block,size_t size){

    if(block-> size >= size+META_SIZE+16){
         block_meta_t *new_block = (block_meta_t*)((char*)(block + 1) + size);
        // this finds the start of current block and then finds where to end the current one and start the new one
        new_block->size=block->size -size-META_SIZE;
        new_block->is_free=true;
        new_block->next=block-> next;
        block-> next=new_block;
        block->size=size;


    }
}


void* my_malloc(int size){

    if(size<=0) return NULL;
    size_t aligned_size=aling8(size);
    block_meta_t *new_mem;
    if(!global_base){
        // so this is our first allocation
        new_mem=request_space(NULL,aligned_size);
        if(!new_mem)return NULL;
        global_base=new_mem;


    }
    else{
        new_mem=find_first_free(aligned_size);
            new_mem->is_free=0;

        }

        else{
            block_meta_t* last=find_last_block();
            new_mem=request_space(last,aligned_size);
            if(!new_mem) return NULL;
        }

    }
    return new_mem+1;
}

block_meta_t* get_block_ptr(void* ptr){
    return (block_meta_t*)ptr -1;
}

void my_free(void* ptr){
 if(!ptr)return;
    block_meta_t* block=get_block_ptr(ptr);
    block->is_free=1;

    if(block->next && block->next->is_free){
        block->size+=block->next->size+META_SIZE;
        block->next=block->next->next;
    }
    if(block->prev && block->prev->is_free){
        block->size=+=block->prev->size+META_SIZE;
        block->prev=block->prev->prev;
    }
}

void* my_calloc(size_t elesize,size_t numelem){
int  size=elesize*numelem;

    void *ptr=my_malloc(size);
    if(ptr){
        memset(ptr,0,size);
    }
    return ptr;
}

void* my_realloc(void* ptr, size_t size){
    if(!ptr)my_malloc(size);
    if(size==0)my_free(ptr);
    //now we do the actual function of realloc which is resizing
    block_meta_t* block=get_block_ptr(ptr);

    if(block->size>=size)return ptr;



}

int main(){

}
