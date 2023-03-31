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
    int charCount=0; 
    int wordCount = 0;
    int lineCount = 0;
    int prevChar = 0;
    int curr = 0;
    while ((curr=fgetc(in)) != EOF){
        charCount++;
        //user adds at least one character and start new line
	if (prevChar=='\n' && (!isspace(curr) && curr!='\n')){
             lineCount++;
    	}	
        if (isspace(curr)  && !isspace(prevChar) && prevChar!='\n'){
             wordCount++;
        }
        prevChar = curr;
    }
    //user ends at a character
    if (!isspace(prevChar) && prevChar!='\n'){
	lineCount++;
	wordCount++;
    }
    else if (prevChar=='\n'){
	lineCount++;
    }
    printf("Character Count is %d\n", charCount);
    printf("Word Count is %d\n", wordCount);
    printf("Line Count is %d\n", lineCount);
    

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
