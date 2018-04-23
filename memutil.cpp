#include <time.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include "libmymem.hpp"

const int mv = 1 , Mv = 8192 , ms = 1 , Ms = 100000 ;

int main(int argc, char const *argv[])
{
	srand(time(0));

	int n = atoi(argv[2]);
	printf("%d %s\n",n,argv[2]);
	for (int i = 0; i < n; ++i)
	{	
		char * ptr = (char *)mymalloc(rand()%Mv + mv);
		if (ptr==NULL){
			printf("Yes NULL\n");
		}
		ptr[0]='h';
		ptr[1]='e';
		ptr[2]='l';
		ptr[3]='l';
		ptr[4]='o';
		usleep(rand()%Ms + ms);
		printf("%s\n",ptr);
		myfree(ptr);
	}

	return 0;
}