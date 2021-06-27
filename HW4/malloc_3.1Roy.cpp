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
    MallocMetadata* next_free;
    MallocMetadata* prev_free;
}* MMD;




MMD bins[128]={nullptr};
MMD lastBlock = nullptr;
MMD mallocPtr = nullptr;
MMD largeAlloc = nullptr;


static void releaseNode(MMD node,int size)
	{
		if(!node->next_free && !node->prev_free) return; 
		if(node->next_free)node->next_free->prev_free =node ->prev_free;
		if(node->prev_free)node->prev_free->next_free = node->next_free;
		else{
			 bins[((int)(size*0.001))] = node->next_free;
			}
		node-> next_free = nullptr;
		node-> prev_free = nullptr;	
				
	}

static void insertNode(MMD* new_node)
{
	
	MMD temp = bins[((int)((*new_node)->size*0.001))];
	if(!temp){ 
		bins[((int)((*new_node)->size*0.001))] = (*new_node);
		}
	else if(temp->size >= (*new_node)->size)
	{
		bins[((int)((*new_node)->size*0.001))] = (*new_node);
		temp->prev_free = *new_node;
		(*new_node)->next_free = temp;
	}
	
	else{
		
	while(temp){
		if(temp->next_free && temp->size <= (*new_node)->size && temp->next_free->size >= (*new_node)->size)
		    {
				(*new_node)->next_free = temp->next_free;
				temp->next_free->prev_free = (*new_node);
				temp->next_free = *new_node;
				(*new_node)->prev_free = temp;
				break;
			} 
			if(!temp->next_free)
			{
			temp->next_free = *new_node;
			(*new_node)->prev_free = temp;
			break;
			} 
		temp = temp->next_free;
		}
	
	}
}

static void* splitBlocks(size_t size , MMD node, int sizeBytes){
		MMD new_node = (MallocMetadata*) ((char*)node->address +size);
		new_node->is_free = true;
		int ress = node->size - size-sizeof(MallocMetadata);
		new_node->size = node->size - size-sizeof(MallocMetadata);
		new_node->next_free = nullptr;
		new_node->prev_free = nullptr;
		node->size = size;
		node->is_free = false;
		if(node == lastBlock) lastBlock = new_node; 
		new_node->address = (char*)new_node + sizeof(MallocMetadata);
		if(node->next) node->next->prev = new_node;
		new_node -> next = node->next;
		node->next = new_node;
		new_node->prev = node;
		// case 1: we need to reassign new node into a new linkedList in bins 
		if(!(((int)(new_node->size*0.001)) == sizeBytes && (!node->prev_free || new_node->size >= node->prev_free->size )))
		{
		releaseNode(node,node->size);
		insertNode(&new_node);
		}
		// case 2: let's keep new_node at the same list where the node was before.
		else{
		new_node->next_free = node->next_free;
		new_node->prev_free = node->prev_free;
		if(node->next_free) node->next_free->prev_free = new_node;
		if(node->prev_free) node->prev_free->next_free = new_node;
		else bins[sizeBytes] = new_node;
			}
		node->next_free = nullptr;
		node->prev_free = nullptr;
		return node->address;
}




static void mergeBlocks(MMD node){

	   MMD temp2 = nullptr;
	   int size = node->size;
       if(node->next && node->next->is_free)//merging with right block
        {
	   if(lastBlock == node->next) lastBlock = node; 
	   releaseNode(node->next,node->size);
	   node->size += (node->next->size+sizeof(MallocMetadata));
	   if(node->next->next)node->next->next->prev = node;
       node->next = node->next->next;
	   if(!node->prev || node->prev->is_free == false){
			insertNode(&node);
			return;
			 }
		}
    size = 0;
    if(node->prev && node->prev->is_free) {
		if(lastBlock == node) lastBlock = node->prev;
        size += node->prev->size ;
        node->prev->size += node->size+sizeof(MallocMetadata);
        releaseNode(node,node->size);
        if(node->next)node->next->prev = node->prev;
       node->prev->next = node->next;
       node = node->prev;
       releaseNode(node,size);//release prev node from bins      
		}
		insertNode(&node);
}




void* smalloc(size_t size){
    if(size == 0 || size>pow(10,8)) return nullptr;
    MMD temp;// = mallocPtr;
    void* res = nullptr;
    int sizeBytes = size *0.001 ;// converting size to bytes
    //first trial - let's find a free block in bins which satisfies the request 
    if (size < 128* pow(10,3)) 
    {     
		for(int i = sizeBytes; i < 128; i++){			
		temp = bins[i];
        while (temp) {
			
            if (temp->size >= size) 
            {
                if ((int)(temp->size - size  -sizeof(MallocMetadata)) >= 128) {
                    return splitBlocks(size, temp, i);
                }
                temp->is_free = false;
                releaseNode(temp,temp->size);
                return temp->address;
            }
            temp = temp->next_free;
        }
	}
	//2nd trial - let's check whether the wilderness chunk is free
	if (lastBlock &&  lastBlock->is_free) 
			{
                sbrk(size - lastBlock->size );
                if (errno == ENOMEM) return nullptr;
                lastBlock->is_free = false;
                releaseNode(lastBlock,lastBlock->size);
                lastBlock->size += size - lastBlock->size;
                return lastBlock->address;
            }
    }
    //case 1 - munmap request
    if(size >= 128* pow(10,3)) 
		{
			res =  mmap(nullptr,size+sizeof(MallocMetadata),PROT_WRITE | PROT_READ,MAP_ANONYMOUS |
			MAP_PRIVATE,-1,0);
			if(res == MAP_FAILED) return nullptr;
			temp =(MallocMetadata*) res;
			temp->address = ((char*) temp) + sizeof(MallocMetadata);
		}
		////case 2 -sbrk request
    else
		{
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
    temp->next_free = nullptr;
    temp->prev_free = nullptr;
    //inserting element into a linked list(sbrk alloc Blocks list).  
    if(size < 128* pow(10,3)) 
    {
		MMD temp2; 
        if (!mallocPtr || (mallocPtr->address > temp->address) ||
            (mallocPtr->address < temp->address && !mallocPtr->next)) 
            {
            if (!mallocPtr) {
                mallocPtr = temp;
                lastBlock = temp;
                return res;
            }
            if (mallocPtr->address > temp->address)
             {
                temp->next = mallocPtr;
                mallocPtr->prev = temp;
                mallocPtr = temp;
                return res;
            }
            temp->prev = mallocPtr;
            mallocPtr->next = temp;
            lastBlock = temp;
            return res;
			}
        temp2 = mallocPtr->next;
        while (temp2) 
			{
            if (temp2->address > temp->address && temp->address > temp2->prev->address) 
				{
                temp->next = temp2;
                temp->prev = temp2->prev;
                temp2->prev->next = temp;
                temp2->prev = temp;
                return res;
				}
            if (!temp2->next && temp2->address < temp->address)
				{
                temp->prev = temp2;
                temp2->next = temp;
                lastBlock = temp;
                return res;
				}
            temp2 = temp2->next;
        }
        return res;
    }
    // in case we are dealing with munmap request
    else{
		if(!largeAlloc)
			{ 
			largeAlloc = temp;
			return temp->address;
			}
			temp->next =largeAlloc;
			largeAlloc->prev = temp;
			largeAlloc = temp;
		return temp->address;
		}
    }


void* scalloc(size_t num, size_t size){

    void* temp = smalloc(num*size);
    if(!temp) return nullptr;
    return memset(temp,0,num*size);
}



void sfree(void* p)
{
    if(!p)return;
    MMD temp = mallocPtr;
    while (temp)
    {
        if(temp->address == p)
			{
				if(temp->is_free == true) return;
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
	
	int totalsize = 0;
    if(size == 0) return nullptr;
    if(!oldp)return smalloc(size);	
    MMD temp = nullptr;
    MMD old = nullptr;
    if(size < 128* pow(10,3)){
		temp = mallocPtr;
    while(temp){
        if(temp->address == oldp ){
            if(temp->size >=size) {
                temp->is_free = false;
                releaseNode(temp,temp->size);
                return oldp;
            }
            break;
        }
        temp = temp->next;
    }
    //old =temp; 
    if(temp) {
		int size2 = 0;
        if (temp->prev &&  temp->prev->is_free && temp->prev->size + temp->size + sizeof(MallocMetadata) >= size) {
			size2 = temp->prev->size;
			temp->prev->size+=(temp->size+ sizeof(MallocMetadata));
            temp->prev->is_free = false;
            temp->prev->next = temp->next;
            if(temp->next) temp->next->prev = temp->prev;
            else{
				lastBlock = temp->prev; 
				}
            releaseNode(temp,temp->size);
            temp = temp->prev;
            
            totalsize = ((int)((int)(temp->size - size )) - sizeof(MallocMetadata));
            if ( totalsize >= 128) {
                return splitBlocks(size, temp,((int)(size2*0.001)));
            }
            releaseNode(temp,size2);
            return temp->address;
        }
        if (temp->next && temp->next->is_free && temp->next->size + temp->size + sizeof(MallocMetadata)>= size) {
			size2 = temp->size;
            releaseNode(temp->next,temp->next->size);
            if(temp->next->next == nullptr)lastBlock = temp;
            temp->size += (temp->next->size+ sizeof(MallocMetadata));
            temp->is_free = false;
            temp->next = temp->next->next;       
            if (temp->next)temp->next->prev = temp;
                totalsize = ((int)((int)(temp->size - size )) - sizeof(MallocMetadata));
            if (totalsize >= 128) {
                return splitBlocks(size, temp,((int)(size2*0.001)));
            }
            releaseNode(temp,size2);
            return temp->address;
        }
        if (temp->next && temp->prev && temp->next->is_free &&  temp->prev->is_free && temp->next->size +
                                        temp->prev->size + temp->size + (2*sizeof(MallocMetadata))>= size) 
            {
			size2 = temp->prev->size;
			if(temp->next->next == nullptr)lastBlock = temp->prev; 
			releaseNode(temp,temp->size);
            releaseNode(temp->next,temp->next->size);								
            temp->prev->size +=( temp->size + temp->next->size+ (2*sizeof(MallocMetadata)));
            temp->prev->is_free = false;
            temp->prev->next = temp->next->next;
            if (temp->next->next) temp->next->next->prev = temp->prev;
            temp = temp-> prev;
            totalsize = ((int)((int)(temp->size - size )) - sizeof(MallocMetadata));
            if (totalsize >= 128) {
                return splitBlocks(size, temp,((int)(size2*0.001)));
            }
            
            releaseNode(temp->prev,size2);
            return temp->address;
        }
    }
                if (!temp->next && temp->is_free) {
                sbrk(size - temp->size );
                if (errno == ENOMEM) return nullptr;
                temp->size += (size - temp->size);
                temp->is_free = false;
                return temp->address;
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


