#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define FILE_IDX 1
#define NUM_ARGS 2


int check_args(int argc){
    if (argc == 0 || argc>NUM_ARGS){
        fprintf(stderr, "%s", "No specified input");
        exit(1);
    }
    return argc;
}

FILE* open__file(char *name){
    FILE* file = fopen(name, "r");
    if (file== NULL){
        perror(name);
        exit(1);
    }
    return file;
}

void echo_contents(FILE *in){
    char* line = NULL; 
    char* prevLine = NULL;
    size_t size;

    while (getline(&line, &size, in) >0){ 
	if ((prevLine==NULL) || (prevLine !=NULL && strcmp(line, prevLine) !=0)){
	    printf("%s", line);  
        }
        free(prevLine);
	prevLine = strdup(line);	        
    }
    free(prevLine);
    free(line);
}


int main(int argc, char *argv[]){
    if (check_args(argc)==NUM_ARGS){
        FILE* in = open__file(argv[FILE_IDX]);
        echo_contents(in);
        fclose(in);
    }
    else{
        echo_contents(stdin);
    }
    return 0;
}
