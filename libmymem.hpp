#include <iostream>
#include <sys/mman.h>
#define SLAB_SIZE (64*1024)
#define N 12
#define X
enum BucketSize{  // Enum for indexing into the correct size
	B4,
	B8,
	B16,
	B32,
	B64,
	B128,
	B256,
	B512,
	B1024,
	B2048,
	B4096,
	B8192,
	LAST
};


struct Slab {

	int totalObj;
	int freeObj;
	bool bitmap[4096];
	int initialize(){
		totalObj=0;
		freeObj=0;
	}
//	Bucket * bucket;

	Slab * nextpointer;
	Object * start_obj;

};


struct Object {
	Slab * parentSlab;
	void * memory;
	Object * nextpointer;

};

struct Bucket {
	size_t bucketSize;
	Slab * slab;
};


Bucket Table[N];		// Table of type bucket

void myfree(void *ptr);
void* mymalloc(unsigned size);

void * allococate_slab_chunk(){
	return mmap(NULL, SLAB_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
}
int deallocate_slab_chunk(void* p){
	return munmap(p,SLAB_SIZE);
}

template <typename nodepointer>
int insert(nodepointer a , nodepointer b){
	if(a == nullptr){
		//allocate space to b
		a = b;
		return 0;
	}
	else{
		nodepointer temp = a;
		while(temp->nextpointer == nullptr)temp = temp->nextpointer;
		//allocate space to b
		temp = b;
		return 0;
	}
	return 1;
}

