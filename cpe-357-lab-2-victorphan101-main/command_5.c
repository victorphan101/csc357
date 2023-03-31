#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int index = 0;
    while(index < argc){
        if (argv[index][0]=='-'){
            printf(argv[index]);
            printf("\n");
        }
        index++;
    }
    return 0;
}

