#include <iostream>		
#include <sys/mman.h>
#include <mutex>
#include <condition_variable>
#define SLAB_SIZE (64*1024)
#define N 12

/* Thread safety is achieved by using semaphores at Bucket level granularity */

class Semaphore {
	/* Class for semaphore */
public:
	Semaphore();
	Semaphore(int); // constructor taking the int value indicating the number of shared resources
	void waiting();  // used for decrementing the count variable using locks
	void signal();	// used for incrementing the count variable using locks

private:
	std::mutex mutex;  // standard mutex for the
	int count;			// indicating the number of shared resources
	std::condition_variable cv; // condition variable

};
Semaphore::Semaphore(){
	count = 1;
}
Semaphore::Semaphore(int n) {
	count = n;
}
void Semaphore::waiting() {
	std::unique_lock<std::mutex> lock(mutex);
	while (count == 0) {
		cv.wait(lock);
	}
	count--;

}
void Semaphore::signal() {
	std::unique_lock<std::mutex> lock(mutex);
	count++;
	cv.notify_one();

}

struct Object {  // Structure for objects

	void* parentSlab = NULL;
	void * memory = NULL;
	Object * nextPointer;	// Object pointing to next object in the linked list

};

struct Slab {	

	int totalObj = 0 ; 
	int freeObj = 0; 
	size_t bucketSize = 0; // to get the bucket size
	std::string bitmap;		// Storing which 0 if objects are free and 1 if objects are empty
	void * bucket = NULL;
	Slab * nextSlab;
	Object * objPtr;	
	int bucketIndex=0;

};
struct Bucket {

	size_t bucketSize;
	int index;		// storing the bucket size
	Slab * slab = NULL;
//	Semaphore bucketMutex;
			// storing the pointer to slab
};
Semaphore BucketSemaphore[N]{{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1},{1}}; //array of semaphores used for thread safety
// Functions defined in libmymem.cpp
int initializeSlab(Slab * ,Bucket);
void* searchInBucket(int );
void myfree(void *ptr);
void* mymalloc(unsigned size);

int firstZeroBitmap(std::string s){ 
	// Searches for the first occurrance of 0 in bitmap i.e returns the position of free object
    int n = s.length();
    for(int i = 0;i<n;i++){
    
    	if (s[i]=='0') return i;
    }
    return -1;
}

std::string make_n_length_string(int n){
	// Function used to make n length bitmap representing the free/empty objects
	std::string s = "";
	for (int i = 0; i < n; i++) {
		s = s + "0";
	}
	return s;

}

void change_char_at(int pos , std::string * a ,std::string b){
	// Used to change the bitmap value of string a at position pos with character b
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

Bucket Table[N]={ {4,0,NULL},{8,1,NULL},{16,2,NULL},{32,3,NULL},{64,4,NULL},{128,5,NULL},{256,6,NULL},{512,7,NULL},{1024,8,NULL},{2048,9,NULL},{4096,10,NULL},{8192,11,NULL}};
		// Table of type bucket


void * allocate_slab_chunk(){
	// Used to allocate the 64 KB of slab
	return mmap(NULL, SLAB_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
}

int deallocate_slab_chunk(void* p){
	// Used to deallocate the slab
	return munmap(p,SLAB_SIZE);

}


int initialize_bucket(int i){
	Table[i].slab =(Slab *) allocate_slab_chunk();
	printf("allocated slab\n");
	// Initializing the slabs have to allocate space to the objects
	initializeSlab(Table[i].slab,Table[i]);	
	printf("initializedSlab\n");
	return 0;
}

Slab * createSlab(Slab * sl,Bucket b){
	// Function to create new slab
	Slab * newSlab =(Slab *) allocate_slab_chunk();
	initializeSlab(newSlab,b);
	sl->nextSlab = newSlab;
	return newSlab;
}


int initialize_all_Buckets(int index){
	
	
		initialize_bucket(index);

	

	return 0;
}
