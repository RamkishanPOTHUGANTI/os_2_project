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
	Slab * slab=NULL;
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

Bucket Table[N]={
	{4,NULL},{8,NULL},{16,NULL},{32,NULL},{64,NULL},{128,NULL},{256,NULL},{512,NULL},{1024,NULL},{2048,NULL},
	{4096,NULL},{8192,NULL}
};		// Table of type bucket

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
	printf("%d %d %d numObjects =%d\n",b.bucketSize,sizeof(struct Object),sizeof(struct Slab),numObjects);
	s->totalObj = numObjects;
	s->freeObj = s->totalObj;
	s->bitmap = make_n_length_string(s->totalObj);

	s->objPtr = (Object *) &s[sizeof(Slab)];
	s->objPtr->nextPointer = NULL;

	s->objPtr->memory = (void *) &s->objPtr[sizeof(Object)];
	s->objPtr->parentSlab = s;
	count ++;
	Object * temp = s->objPtr;
	printf("%p\n",s[64*1024]);
	printf("slab %p obj %p memor %p next %p \n",s,s->objPtr,s->objPtr->memory,s->objPtr->memory + s->bucketSize);
	while (count < numObjects){
		printf("%p\n",temp);
		temp->nextPointer =(Object *) (temp ->memory + b.bucketSize);
		printf("%p\n",temp->nextPointer);
		temp = temp->nextPointer;
		printf("%p\n",temp);
		if (temp==NULL){
			printf("usNULL");
			return 0;
		}
		temp->memory= (void *) &temp[sizeof(Object)];
		temp->parentSlab=s;
		printf("%d \n",count);//,temp->memory,temp->parentSlab);
		count++;

	}
	temp->nextPointer=NULL;


	return 0;

}

int initialize_bucket(int i){
//	Table[i].bucketSize = (2^(i+2)); //use correct syntax for enumerator
	Table[i].slab =(Slab *) allocate_slab_chunk();
	printf("allocated slab\n");
	// Initializing the slabs have to allocate space to the objects
	initializeSlab(Table[i].slab,Table[i]);
	printf("initialized slab\n");
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


int initialize_all_Buckets(int index){


		initialize_bucket(index);


	return 0;
}
//----------------------
void * searchInBucket(int bucketIndex){

	Slab * currentSlab;

	currentSlab = Table[bucketIndex].slab;
  if (currentSlab == NULL) {
		printf("Initializing\n");
		initialize_bucket(bucketIndex);

	}
	currentSlab = Table[bucketIndex].slab;
	if (currentSlab==NULL){
		printf("isNULL\n");
	}
	while (currentSlab->nextSlab!=NULL && currentSlab->freeObj==0){
		currentSlab = currentSlab -> nextSlab;
	}
	if (currentSlab -> freeObj == 0){
		currentSlab = createSlab(currentSlab,Table[bucketIndex]);
		Object * obj = currentSlab -> objPtr;
		currentSlab->freeObj--;
		// Change the bitmap value
		int position = 0;
		change_char_at(position,&currentSlab->bitmap,"1");

		return obj->memory;
	}
	else{

		int position=0,count=0;
		// search in bitmap update position;
		position = firstZeroBitmap(currentSlab->bitmap);

		Object * obj = currentSlab->objPtr;

		while (count<position){

			obj = obj->nextPointer;
			count++;
		}

		return obj->memory;


	}





}

void * mymalloc(unsigned size){

	int min=1000;
	int bucketIndex=-1;
	for (int i=0 ; i<12;i++){
		int diff = abs(size - Table[i].bucketSize);

		if (diff < min){
			min = diff;
			bucketIndex=i;
		}
	}
	printf("%d : size , bucket %d\n",size,bucketIndex);
	void * memory = searchInBucket(bucketIndex);

	return memory;
}



void myfree(void * ptr){

	if (ptr==NULL) return;
	Object * o = (Object*)ptr - sizeof(Object);
	Object *temp;
    Slab * p = get_parent_slab_address(o);
    temp = p->objPtr;
    int position = 0 ;
    while(temp != o){

    	temp = temp->nextPointer;
    	position++;
    }
    p->freeObj++;
    change_char_at(position,&p->bitmap,"0");
    ptr = NULL;
    return ;


}
