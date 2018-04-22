#include "libmymem.hpp"


void * searchInBucket(int bucketIndex){

	Slab * currentSlab;

	currentSlab = Table[bucketIndex].slab;
  if (currentSlab == NULL) initialize_bucket(bucketIndex);
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
