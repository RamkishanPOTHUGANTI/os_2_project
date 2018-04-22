#include "libmymem.hpp"


void * searchInBucket(int bucketIndex){
	Slab * currentSlab;

	currentSlab = Table[bucketIndex].slab;
	while (currentSlab->nextSlab!=NULL && currentSlab->freeObj==0){
		currentSlab = currentSlab -> nextSlab;
	}
	if (currentSlab -> freeObj == 0){
		currentSlab = createSlab(currentSlab,Table[bucketIndex]);
		Object * obj = currentSlab -> objPtr;
		currentSlab->freeObj--;
		// Change the bitmap value

		return obj->memory;
	}
	else{
		
		int position=0,count=0;
		// search in bitmap update position;
		
		Object * obj = currentSlab->objPtr;
		while (count<position){
			obj = obj->nextPoiter;
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

	void * memory = searchInBucket(bucketIndex);

}

int main(){

}

void myfree(void * ptr){

	Object * o = (Oject*)(ptr - sizeof(Object)),*temp;
    Slab * p = get_parent_slab_address(o);
    temp = p->objPtr;
    int position = 1 ;
    while(temp != o){temp = temp->nextpointer;position++;}
    p->freeObj++;
    p->bitmap[position-1] = 0 ;
    ptr = NULL;
    return ;

	
}
