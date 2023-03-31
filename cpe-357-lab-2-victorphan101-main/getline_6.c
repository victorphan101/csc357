#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILE_IDX 1
#define NUM_ARGS 2
#define LAST_TWO 2


void validate_args(int argc, char *argv[]){
    if (argc != NUM_ARGS){
        fprintf(stderr, "usage:%s file\n", argv[0]);
        exit(1);
    }
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
	size_t size;
    char* first_line = NULL;
    char* second_line = NULL;
	while(getline(&line, &size, in)>0){
        free(first_line);
        first_line = second_line;
        second_line = strdup(line);
	}
    printf("%s", first_line);
    printf("%s", "\n");
    printf("%s", second_line);
    free(first_line);
    free(second_line);
    free(line);
}

int main(int argc, char *argv[]){
    validate_args(argc, argv);
    FILE* in = open__file(argv[FILE_IDX]);
    in = stdin;
    echo_contents(in);
    fclose(in);
    return 0;
}
