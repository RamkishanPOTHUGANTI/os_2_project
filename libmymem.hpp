#include <iostream>
#include <sys/mman.h>
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

	Slab * nextSlab;

};


struct Object {
	Slab * parentSlab;
	void * memory;
	
};

struct Bucket {
	size_t bucketSize;
	Slab * slab;
};


Bucket Table[N];		// Table of type bucket 

void myfree(void *ptr);
void* mymalloc(unsigned size);

