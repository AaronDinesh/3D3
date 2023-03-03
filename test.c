#include <stdlib.h>
#include <stdio.h>

#define SIZE 4096
int test2(int j, int k);

int test(void);
int main(){
    int ret = test();
    printf("Return val is: %d\n", ret);
    return 0;   
}


int test(void){
    test2(20, 30);
}

int test2(int j, int k){
    int i = j;
    int l = k;
    return j;
}