#include "libmymem.hpp"
#include <unistd.h>
#include <time.h>
const int m = 1 , M = 8192;//rand()
int n;
int main(){

    srand(time(0));
    std::cout<<"How many time you want to test?\n";
    std::cin>>n;
    while(n-- > 0){
    char * ptr = (char*)mymalloc((rand()/M)+m);
    usleep(rand()%1000 + 100);
    myfree((void*)ptr);
    }
    return 0;
}
