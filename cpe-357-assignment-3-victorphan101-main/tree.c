#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "array_list.h"

void validate_arguments(int argc, char* argv[], int* checkSize, int* checkHidden, char** mainDir){
	if (strcmp(argv[0], "./tree")!=0){
		printf("%s\n", argv[0]);
		printf("Command not supported\n");
		exit(1);;
	}
	int ptr = 1;
	while (ptr!=argc){
		if (strcmp(argv[ptr],".")==0 || strcmp(argv[ptr], "..")==0){
			exit(1);
		}
		else if (strcmp(argv[ptr], "-s")==0){
			*checkSize = 1;
		} 		
		else if (strcmp(argv[ptr], "-a")==0){
			*checkHidden = 1;
		}
		else{
			if (*mainDir == NULL){
				*mainDir = argv[ptr];
			}
			else{
				printf("Error mainDir: %s\n", *mainDir);
				printf("invalid number of directory inputs\n");
			}
		}
		ptr++;
	}

}

int cmpName(const void *node1, const void *node2){
	struct node *stream1 = (struct node*)node1;
	struct node *stream2 = (struct node*)node2;
	int orderNum = strcmp(stream1->name, stream2->name);
	return orderNum; 
}

void tree_struct(char *directory, int *checkSize, int *checkHidden, int numSpaces){
	if (strcmp(directory, ".")==0 || strcmp(directory,"..")==0 || chdir(directory)==-1){
		printf("directory no exist\n");
		return;
	}
	DIR *dir;
	struct dirent *stream;
	struct stat statbuf;

	if ((dir = opendir(".")) == NULL){
		closedir(dir);
		printf("Cannot open directory\n");
		exit(1);
	}

	struct list *fList = array_list_new(100);
	while((stream=readdir(dir)) != NULL){
		char* name = strdup(stream->d_name);
		if (strcmp(name, ".")!=0 && strcmp(name, "..")!=0){
			if (*checkHidden==0 && name[1]!='.'){ 
				unsigned char type = stream->d_type;
				array_list_add_to_end(fList, name, type);
			}
		}
	}
	free(stream->d_name);
	free(stream);
	closedir(dir);

	//qsort(fList, fList->length, sizeof(struct node), cmpName);
	
	struct node* ptr = fList->head;
	while (ptr!=NULL){
                int i;
                for(i=0; i<numSpaces; i++){
                        printf("|   ");
                }
		stat(ptr->name, &statbuf);

		if (*checkSize==1){
			printf("|--%s [size: %d]\n", ptr->name, (int)statbuf.st_size);
		}else{
                        printf("|-- %s\n", ptr->name);
                }
                if (((int)statbuf.st_size)!=0 && ptr->type==DT_DIR){
                        int newNumSpaces = numSpaces+1;
                        tree_struct(ptr->name, checkSize, checkHidden, newNumSpaces);
                        chdir("..");
                }
        	
		ptr = ptr->next;
	}
	free_list(fList);
}


int main(int argc, char* argv[]){
	int checkSize = 0;
	int checkHidden = 0;

	char* mainDir = NULL;
	validate_arguments(argc, argv, &checkSize, &checkHidden, &mainDir);
	
	tree_struct(mainDir, &checkSize, &checkHidden, 0);
	free(mainDir);
}
