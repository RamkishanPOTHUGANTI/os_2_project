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
	Object * nextPointer;

};

struct Slab {

	int totalObj = 0 ; //4 bytes
	int freeObj = 0; //4 bytes
	size_t bucketSize = 0; //4 bytes
	std::string bitmap;
	void * bucket = NULL;
	Slab * nextSlab;
	Object * objPtr;

};
struct Bucket {
	size_t bucketSize;
	Slab * slab;
};

int firstZeroBitmap(std::string s){
    int n = s.length();
    for(int i = 0;i<n;i++){
    
    	if (s[i]=='0') return i;
    }
    return -1;
}

std::string make_n_length_string(int n){

	std::string s = "";
	for (int i = 0; i < n; i++) {
		s = s + "0";
	}
	return s;

}

void change_char_at(int pos , std::string * a ,std::string b){
	a->replace(pos,1,b);
	return ;
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

void * allocate_slab_chunk(){
	return mmap(NULL, SLAB_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
}

int deallocate_slab_chunk(void* p){

	return munmap(p,SLAB_SIZE);

}

int initializeSlab(Slab * s,Bucket b){

	// Function should allocate space for the objects
	
	s->bucket = (void *) &b;
	s->bucketSize = b.bucketSize;
	s->nextSlab = NULL;

	// Have to allocate space for the objects
	int count = 0;
	int numObjects = (SLAB_SIZE - (sizeof(struct Slab)))/ (sizeof(struct Object) + b.bucketSize);
	
	s->totalObj = numObjects;
	s->freeObj = s->totalObj;
	s->bitmap = make_n_length_string(s->totalObj);	

	s->objPtr = (Object *) (sizeof(struct Slab) + s);
	s->objPtr->nextPointer = NULL;
	s->objPtr->memory = (void *) (sizeof(Object) + s->objPtr);
	s->objPtr->parentSlab = s;
	count ++;
	Object * temp = s->objPtr;

	while (count < numObjects){

		temp->nextPointer = temp + sizeof(Object) + b.bucketSize;
		temp = temp->nextPointer;
		temp->memory= (void *) (sizeof(Object) + temp);
		temp->parentSlab= s;
		count++;
				
	}
	temp->nextPointer=NULL;
	

	return 0;

}

int initialize_bucket(int i){
	Table[i].bucketSize = (2^(i+2)); //use correct syntax for enumerator
	Table[i].slab =(Slab *) allocate_slab_chunk();

	// Initializing the slabs have to allocate space to the objects
	initializeSlab(Table[i].slab,Table[i]);	

	return 0;
}
/*int initialize_Object(Object * o , int i){

	//memory = (void*)o + size_of(o->parentSlab);
	//o->nextPointer = o + BucketSize[i];
	//return 0;
}
*/
Slab * createSlab(Slab * sl,Bucket b){
	// Function to create new slab
	Slab * newSlab =(Slab *) allocate_slab_chunk();
	initializeSlab(newSlab,b);
	sl->nextSlab = newSlab;
	return newSlab;
}


int initialize_all_Buckets(){
	
	for(int i = 0 ; i < N ; i++){
		initialize_bucket(i);

	}
	return 0;
}