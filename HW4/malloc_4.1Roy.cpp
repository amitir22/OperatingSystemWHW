#include <unistd.h>
#include <assert.h>
#include <cstdlib>
#include <stdio.h>
#include <sys/wait.h>
#include <iostream>
#include <cmath>
#include <string.h>
#include <sys/mman.h>
typedef struct MallocMetadata{
    size_t  size;
    bool is_free;
    void* address;
    MallocMetadata* next;
    MallocMetadata* prev;
}* MMD;
MMD mallocPtr = nullptr;
MMD largeAlloc = nullptr;
void* splitBlocks(size_t size , MMD node){
    MMD new_node = (MallocMetadata*) ((char*)node->address +size);
    new_node ->is_free = true;
    new_node->size = node->size - size-sizeof(MallocMetadata);
    node->size = size;
    node->is_free = false;
    if(node->next) node->next->prev = new_node;
    new_node->next = node->next;
    new_node->prev = node;
    node->next = new_node;
    new_node->address = (char*)new_node + sizeof(MallocMetadata);
    return node->address;
}
void* smalloc(size_t size){
    if(size == 0 || size>pow(10,8)) return nullptr;
    MMD temp = mallocPtr;
    void* res = nullptr;
      size_t rem = size%8;
    if(rem)size+=(8-rem);
    if (size < 128* pow(10,3)) {
        while (temp) {	  
            if (temp->is_free && temp->size >= size) {
                if ((int)(temp->size - size  -sizeof(MallocMetadata)) >= 128) {
                    return splitBlocks(size, temp);
                }
                temp->is_free = false;
                return temp->address;
            }
            if (!temp->next && temp->is_free) {
                sbrk(size - temp->size );
                if (errno == ENOMEM) return nullptr;
                temp->size += size - temp->size;
                temp->is_free = false;
                return temp->address;
            }
            temp = temp->next;
        }
    }
    if(size >= 128* pow(10,3)) {
       res =  mmap(nullptr,size+sizeof(MallocMetadata),PROT_WRITE | PROT_READ,MAP_ANONYMOUS |
        MAP_PRIVATE,-1,0);
        if(res == MAP_FAILED) return nullptr;
        temp =(MallocMetadata*) res;
        temp->address = ((char*) temp) + sizeof(MallocMetadata);
    }
    else{
    temp = (MallocMetadata*) sbrk(sizeof(MallocMetadata));
    if(errno == ENOMEM) return nullptr;
        res =  sbrk(size);
        if(errno == ENOMEM) return nullptr;
        temp->address = res;
    }
    temp->is_free = false;
    temp->size = size;
    temp->next = nullptr;
    temp->prev = nullptr;
    if(size < 128* pow(10,3)) {
        if (!mallocPtr || (mallocPtr->address > temp->address) ||
            (mallocPtr->address < temp->address && !mallocPtr->next)) {
            if (!mallocPtr) {
                mallocPtr = temp;
                return res;
            }
            if (mallocPtr->address > temp->address) {
                temp->next = mallocPtr;
                mallocPtr->prev = temp;
                mallocPtr = temp;
                return res;
            }
            temp->prev = mallocPtr;
            mallocPtr->next = temp;
            return res;
        }
        MMD temp2 = mallocPtr->next;
        while (temp2) {
            if (temp2->address > temp->address && temp->address > temp2->prev->address) {
                temp->next = temp2;
                temp->prev = temp2->prev;
                temp2->prev->next = temp;
                temp2->prev = temp;
                return res;
            }
            if (!temp2->next && temp2->address < temp->address) {
                temp->prev = temp2;
                temp2->next = temp;
                return res;
            }

            temp2 = temp2->next;
        }
        temp2 = mallocPtr;
        while (temp2) {
            temp2 = temp2->next;
        }
        return res;
    }
    else{
			       MMD temp2 = largeAlloc;
                while(temp2){	
			temp2 = temp2->next;
			}
		if(!largeAlloc){ 
			largeAlloc = temp;
			return temp->address;}
			temp->next =largeAlloc;
			largeAlloc->prev = temp;
			largeAlloc = temp;
			        temp2 = largeAlloc;
                while(temp2){
			temp2 = temp2->next;
}
    return temp->address;}}



void* scalloc(size_t num, size_t size){

    void* temp = smalloc(num*size);
    if(!temp) return nullptr;
    return memset(temp,0,num*size);
}


void* mergeBlocks(MMD node){
	   MMD temp = mallocPtr;
    while(temp){
		temp = temp->next;
		}
    if(node->next && node->next->is_free) {
        node->size += (node->next->size+sizeof(MallocMetadata));
        if(node->next->next)node->next->next->prev = node;
          node->next = node->next->next;
    }
         temp = mallocPtr;
    while(temp){
		temp = temp->next;
		}
    if(node->prev && node->prev->is_free) {
        node->address = node->prev->address;
        node->prev->size += node->size+sizeof(MallocMetadata);
        if(node->next)node->next->prev = node->prev;
          node->prev->next = node->next;
    }
    if(!node->prev) mallocPtr =node;
     temp = mallocPtr;
    while(temp){
		temp = temp->next;
		}
    return node;
}


void sfree(void* p)
{
    if(!p)return;
    MMD temp = mallocPtr;
    while (temp){
        if(temp->address == p)
        {
            temp->is_free = true;
            mergeBlocks(temp);
            return;
        }
        temp = temp->next;
    }
    temp =largeAlloc;
    while (temp){
		
        if(temp->address == p)
        {
			if(temp->prev) temp->prev->next = temp-> next;
			else largeAlloc = temp->next;
			if(temp->next) temp->next->prev = temp->prev;
			munmap((char*)temp->address -sizeof(MallocMetadata),temp->size+sizeof(MallocMetadata));
            return;
        }
        temp = temp->next;
    }
}


void* srealloc(void* oldp, size_t size)
{
    if(size == 0) return nullptr;
    if(!oldp)return smalloc(size);	
    MMD temp = nullptr;
    MMD old = nullptr;
    size_t rem = size%8;
    if(rem)size+=(8-rem);
    if(size < 128* pow(10,3)){
		temp = mallocPtr;
    while(temp){
        if(temp->address == oldp ){
            if(temp->size >=size) {	
                temp->is_free = false;
                return oldp;
            }
            break;
        }
        temp = temp->next;
    }
    old =temp; 
    if(temp) {
        if (temp->prev &&  temp->prev->is_free && temp->prev->size + temp->size + sizeof(MallocMetadata) >= size) {
			temp->prev->size+=(temp->size+ sizeof(MallocMetadata));
            temp->prev->is_free = false;
            temp->prev->next = temp->next;
            if(temp->next) temp->next->prev = temp->prev;
               temp = temp->prev;
            if ((int)((temp->size - size ) - sizeof(MallocMetadata)) >= 128) {
                return splitBlocks(size, temp);
            }
            return temp->address;
        }
        if (temp->next && temp->next->is_free && temp->next->size + temp->size + sizeof(MallocMetadata)>= size) {
            temp->size += (temp->next->size+ sizeof(MallocMetadata));
            temp->is_free = false;
            temp->next = temp->next->next;
            if (temp->next)temp->next->prev = temp;
            if ((int)temp->size - size - sizeof(MallocMetadata)>= 128) {
                return splitBlocks(size, temp);
            }
            return temp->address;
        }
        if (temp->next && temp->prev && temp->next->is_free &&  temp->prev->is_free && temp->next->size +
                                        temp->prev->size + temp->size + (2*sizeof(MallocMetadata))>= size) {
            temp->prev->size +=( temp->size + temp->next->size+ (2*sizeof(MallocMetadata)));
            temp->prev->is_free = false;
            temp->prev->next = temp->next->next;
            if (temp->next->next) temp->next->next->prev = temp->prev;
            temp = temp-> prev;
            if ((int)temp->size - size -sizeof(MallocMetadata)>= 128) {
                return splitBlocks(size, temp);
            }
            return temp->address;
        }
    }
    void* temp2 = smalloc(size);
    if(!temp2) return nullptr;
    void* res = memcpy(temp2,oldp,size);
    if(!res) {
        sfree(temp2);
        return nullptr;}
    sfree(oldp);
    return res;
}
   else{
	   temp = largeAlloc;
	    while(temp){
	          if(temp->address == oldp ) break;      
        temp = temp->next;
    }
    if(temp->prev) temp->prev->next = temp-> next;
	else largeAlloc = temp->next;
	if(temp->next) temp->next->prev = temp->prev;
    void* temp2 = smalloc(size);
    if(!temp2) return nullptr;
    void* res = memcpy(temp2,oldp,size);
    munmap((char*)temp->address -sizeof(MallocMetadata),temp->size+sizeof(MallocMetadata));
    if(!res) {
        sfree(temp2);
        return nullptr;}
    sfree(oldp);
    return res;
	   }
}



size_t _num_free_blocks(){

   size_t  counter = 0;
    MMD temp = mallocPtr;
    if(temp == nullptr) return 0;
    while (temp){
        if(temp->is_free) counter++;
        temp = temp->next;
    }
    return counter;
}


size_t _num_free_bytes(){
    size_t counter = 0;
    MMD temp = mallocPtr;
    while (temp){
        if(temp->is_free) counter+=temp->size;
        temp = temp->next;
    }
    temp = largeAlloc;
    while (temp){
        if(temp->is_free) counter+=temp->size;
        temp = temp->next;
    }
    return counter;
}

size_t   _num_allocated_bytes(){

    size_t counter = 0;
    MMD temp = mallocPtr;
    while (temp){
        counter+=temp->size;

     
        temp = temp->next;
    }
    temp = largeAlloc;
    while (temp){
       if(!temp->is_free){counter+=temp->size;
        }
        temp = temp->next;
    }
    return counter;
}

size_t _num_allocated_blocks(){
    size_t counter = 0;
    MMD temp = mallocPtr;
    while (temp){
        counter++;
        temp = temp->next;
    }
    temp = largeAlloc;
    while (temp){
        if(!temp->is_free){
			counter++;}
        temp = temp->next;
    }
    return counter;
}



size_t _num_meta_data_bytes(){

    int counter = _num_allocated_blocks();
    return counter * sizeof(MallocMetadata);
}

size_t _size_meta_data(){
    return sizeof (MallocMetadata);
}


















