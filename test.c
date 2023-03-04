#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define SIZE 4096


int main(){
    char buff[SIZE] = {"\0"};
    system("hostname -I >> ip.txt");
    FILE* fp = fopen("ip.txt", "r");
    fgets(buff, SIZE, fp);
    buff[strcspn(buff, "\n")] = '\0';
    printf("IP is: %s\n", buff);
    fclose(fp);
    remove("ip.txt");
    return 0;
}


