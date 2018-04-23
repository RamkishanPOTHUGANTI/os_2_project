#include <iostream>
#include <thread>
#include "libmymem.hpp"
#include <unistd.h>
#include <time.h>

using namespace std;
int nthreads , iter ;
const int m = 1 , M = 8192;//rand()


void myThreadFun()
{
  int n = iter;
  for(int i = 0 ; i < n ; i++){
   
      char * ptr = (char*)mymalloc(rand()%M + m);
      ptr[0]='h';
      ptr[1]='e';
      ptr[2]='l';
      usleep(rand()%10000);
      myfree((void*)ptr);
      
    }
    return ;
}
int main(int argc, char const *argv[]) {
    iter = atoi(argv[2]);
    nthreads = atoi(argv[4]);
    thread  a[nthreads];
    for (int i = 0; i < nthreads; i++) {
    
       a[i] = std::thread(myThreadFun);
    
    }
    for (int i = 0; i < nthreads; i++) {
    
      a[i].join();
    
    }
    return 0;
}