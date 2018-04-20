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


struct Object {
	void* parentSlab = NULL;
	void * memory = NULL;
	Object * nextpointer;
};

struct Slab {
	int totalObj = 0 ; //4 bytes
	int freeObj = 0; //4 bytes
	int BucketSize = 0; //4 bytes
	bool bitmap[4096]; //4096 bytes
	void * bucket = NULL;
	Slab * nextpointer;
};
struct Bucket {
	size_t bucketSize;
	Slab * slab;
};
void intialise_slab(Slab * s , int bs){
	s->bucketSize = bs;
	int used_space = 10;
	s->totalObj = (64*1024 - B)/sizeof(Object);
	s->freeObj = s->totalObj;
	update_parent_bucket_in_Slab(s,&Bucket[bs]); 
}
void update_parent_slab_in_Object(Slab * s , Object * o){
	o->parentSlab = (void *)s;
}
Slab * get_parent_slab_address(Object* o){
	return (Slab *)(o->parentSlab);
}
void update_parent_bucket_in_Slab(Bucket * b , Slab * s){
	s->bucket = (void *)b;
}
Bucket * get_parent_bucket_address(Slab* s){
	return (Bucket *)(s->bucket);
}

Bucket Table[N];		// Table of type bucket

void myfree(void *ptr);
void* mymalloc(unsigned size);

void * allococate_slab_chunk(){
	return mmap(NULL, SLAB_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
}
int deallocate_slab_chunk(void* p){
	return munmap(p,SLAB_SIZE);
}


Slab* insert_slab_in_bucket(Slab * first_slab_in_bucket,int bucket_size){
	if(first_slab_in_bucket == NULL){
		Slab * slab_ ;
		void * tempptr = allococate_slab_chunk();
		if(tempptr == MAP_FAILED){
			return nullptr;
		}
		slab_ = (Slab*)tempptr;  //use some dynmaci cast or static cast
		intialise_slab(slab_,bucketSize);
		first_slab_in_bucket = slab_;
		return first_slab_in_bucket;
	}
	else{
	  Slab * slab_ ;
	  void * tempptr = allococate_slab_chunk();
	  if(tempptr == MAP_FAILED){
		     return nullptr;
	  }
	  slab_ = (Slab*)tempptr;  //use some dynmaci cast or static cast
		Slab * temp = first_slab_in_bucket;
		while(temp->nextpointer == nullptr)temp = temp->nextpointer;
		intialise_slab(slab_,bucketSize);
		temp->nextpointer = slab_;
		return temp->nextpointer;
	}
	return nullptr;
}
