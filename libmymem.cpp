#include "libmymem.hpp"

int initializeSlab(Slab * s,Bucket b){
	/* Function used to initialize the slab according to the bucket size
	*/
	s->bucket = (void *) &b;
	s->bucketSize = b.bucketSize;
	s->nextSlab = NULL;
	s->bucketIndex = b.index;
	// Have to allocate space for the objects
	int count = 0;
	int numObjects = (SLAB_SIZE - (sizeof(struct Slab)))/ (sizeof(struct Object) + b.bucketSize); // Calculates the number of objects
	
	s->totalObj = numObjects;
	s->freeObj = s->totalObj;
	s->bitmap = make_n_length_string(s->totalObj);  // creating the bitmap

	s->objPtr = (Object *) ((char *)s + sizeof(Slab)); // Shifting the pointer to starting address of object pointer
	s->objPtr->nextPointer = NULL;

	s->objPtr->memory = (void *)((char *)s->objPtr + sizeof(Object)); // Shifting the obj ptr to the first address of the memory location
	s->objPtr->parentSlab = s;
	count ++;

	Object * temp = s->objPtr;
	while (count < numObjects){
		// Loop to create 'numObjects' number of objects in the form of linked list

		temp->nextPointer =(Object *) ((char *)temp ->memory + b.bucketSize); // setting the next pointer to point to next object
		temp = temp->nextPointer;
		if (temp==NULL){
			return 0;
		}
		temp->memory= (void *)((char *)temp + sizeof(Object)); // setting the memory
		temp->parentSlab=s;		// setting the parent slab pointer
		count++;

	}

	temp->nextPointer=NULL;		// The last object points to NULL


	return 0;

}

void *searchInBucket(int bucketIndex){
	/*	Search in bucket function searches in the Table indexed at bucket index looks for the slab pointer 
		using the slab pointer searches for the free objects if all the slabs are filled then creates a new 
		slab and initializes it.

	*/
	BucketSemaphore[bucketIndex].waiting();

	Slab * currentSlab;
	Object * obj;
	currentSlab = Table[bucketIndex].slab;
	
	if (currentSlab == NULL){  
	       // Initially if not slabs are present then initialize the slab
		printf("initializing bucket\n");
		initialize_bucket(bucketIndex);
	
	}
	currentSlab = Table[bucketIndex].slab;

	while (currentSlab->nextSlab!=NULL && currentSlab->freeObj==0){
		// Checking for a slab which has free objects
		currentSlab = currentSlab -> nextSlab;
	}

	if (currentSlab -> freeObj == 0){ // if no free objects are found then creates a new slab and initializes it

		currentSlab = createSlab(currentSlab,Table[bucketIndex]);
		obj = currentSlab -> objPtr;
		currentSlab->freeObj--;
		// Change the bitmap value
		int position = 0;
		change_char_at(position,&currentSlab->bitmap,"1");
		std::cout<<currentSlab->bitmap<<"\n";
	}
	else{ // if a slab is found with free objects then
		
		int position=0,count=0;
		// searches in bitmap to get the position of free object;

		position = firstZeroBitmap(currentSlab->bitmap); 
		currentSlab->freeObj--;
		change_char_at(position,&currentSlab->bitmap,"1");
		printf("\nfree objects after allocating %d\n",currentSlab->freeObj);
		obj = currentSlab->objPtr;

		while (count<position){
			// traverses the objects until the desired position is reached
			obj = obj->nextPointer;
			count++;
		}
		
		

	}


	BucketSemaphore[bucketIndex].signal();
//	printf("hoo\n");
	return obj->memory;


}

void * mymalloc(unsigned size){
	// mymalloc is user defined memory allocation function 
	int min=100000;
	int bucketIndex=-1;
//	printf("%d %d\n",min,bucketIndex);
	for (int i=0 ; i<12;i++){  // Loop for searching the index in the Table according to the best fit algorithm

		int diff = abs(size - Table[i].bucketSize);
		if (diff < min){
			min = diff;
			bucketIndex=i; 
		}
	}
	if (size > Table[bucketIndex].bucketSize){ // To adjust the bucket index in edge cases
		bucketIndex++;
	}
	printf("\n%d : size , bucket %d\n",size,bucketIndex);
	void * memory = searchInBucket(bucketIndex);    // search in bucket returns the memory pointer 

	return memory;
}



void myfree(void * ptr){
	// myfree is user defined functino to free the memory taken by the void*
	if (ptr==NULL) return;  

	Object * o = (Object*)((char *)ptr - sizeof(Object)); // getting the object pointer from the memory pointer
	Object *temp;
    Slab * p = get_parent_slab_address(o);		// to get parent slab address
//    Bucket * b = get_parent_bucket_address(p);
    printf("%d hee\n",p->bucketIndex);
    BucketSemaphore[p->bucketIndex].waiting();
 //   printf("how\n");
    temp = p->objPtr;
    int position = 0 ;
    while(temp != o){
    	// Traversing to the desired object to get the position of the object so that bitmap can be changed
    	temp = temp->nextPointer;
    	position++;
    }
    for (int i=0;p->bitmap[i]!='\0';i++){
    	printf("%c",p->bitmap[i]);
    }
    printf("\n");
//    std::cout<<p->bitmap<<"\n";
    p->freeObj++;

    printf("\nnum Free Objects after freeing %d",p->freeObj);
    change_char_at(position,&p->bitmap,"0"); // changing the bitmap to 0 to indicate free object
    ptr = NULL;
    printf("\nhoooo freed\n");
    BucketSemaphore[p->bucketIndex].signal();
    return ;

	
}
