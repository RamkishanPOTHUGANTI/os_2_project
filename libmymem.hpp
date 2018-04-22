#include <iostream>
#include <cstdlib>
#include <string.h>
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
	size_t bucketSize = 0; //4 bytes
	std::string bitmap ;//#zeros = 64*1024/samlles bucket
	void * bucket = NULL;
	Slab * nextSlab;
	Object * objPtr;
};
struct Bucket {
	size_t bucketSize;
	Slab * slab;
};

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
std::string make_n_length_string(int n){
	std::string s = "";
	for (int i = 0; i < n; i++) {
		s = s + "0";
	}
	return s;
}
std::string change_char_at(int pos , std::string a ,char b){
	std::string s = "";
	int n = a.length();
	for (int i = 0; i < pos-1; i++) {
		s = s + a[i];
	}
	s = s + b;
	for (int i = pos; i < n; i++) {
		s = s + a[i];
	}
	return s;
}
void myfree(void *ptr){
	if(ptr != nullptr){
	Object *o = (Object*)(static_cast<char*>(ptr)-sizeof(Object)),*temp;
	Slab * p = get_parent_slab_address(o);
	temp = p->objPtr;
	int position = 1 ;
	while(temp != o){temp = temp->nextpointer;position++;}
	p->freeObj++;
	p->bitmap = change_char_at(position,p->bitmap,0);
	ptr = NULL;
	}
	return ;
}
void* mymalloc(unsigned size);

Slab * allocate_slab_chunk(){
	return (Slab*)mmap(NULL, SLAB_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
}

int deallocate_slab_chunk(void* p){
	return munmap(p,SLAB_SIZE);
}

int initializeSlab(Slab * s,Bucket b){
	// Function should allocate space for the objects
	s->totalObj = ((64*1024) - sizeof(Slab)) /b.bucketSize ;
	s->bucket = (void *) &b;
	s->bucketSize = b.bucketSize;
	s->nextSlab = NULL;
	s->bitmap = make_n_length_string(s->totalObj);

	// Have to allocate space for the objects

}

int initialize_bucket(int i){
	Table[i].bucketSize = (2^(i+2)); //use correct syntax for enumerator
	Table[i].slab =(Slab *) allocate_slab_chunk();

	// Initializing the slabs have to allocate space to the objects
	initializeSlab(Table[i].slab,Table[i]);

	return 0;
}
int initialize_Object(Object * o , int i){

	o->memory = (void*)((char*)o + sizeof(o->parentSlab));
	o->nextpointer = o + Table[i].bucketSize;
	return 0;
}

Slab * create_slab(Slab * sl,Bucket b){
	// Function to create new slab
	Slab * newSlab = allocate_slab_chunk();
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
