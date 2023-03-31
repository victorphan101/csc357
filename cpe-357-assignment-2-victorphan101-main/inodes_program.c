#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#define MAX_CHAR 32
#define MAX_INODE 1024

char* checked_malloc(size_t size){
        char* p;
        p = malloc(size);
        if (p==NULL){
                perror("malloc");
                exit(1);
        }
	return p;
}

char *uint32_to_str(uint32_t i){
	int length = snprintf(NULL, 0, "%lu", (unsigned long)i);       // pretend to print to a string to get length
   	char* str = checked_malloc(length + 1);                        // allocate space for the actual string
   	snprintf(str, length + 1, "%lu", (unsigned long)i);            // print to string

   	return str;
}

void validate_inputs(int count){
	//check for too fewer too many  words for command
	//'mkdir *name*'
	if(count==0 || count>2){
		printf("Out of bound command input(s) or arguments");
	}	
}

char *remove_leading(char str[]){
	size_t idx;
	char* newStr;
	newStr = strdup(str);
	for (idx = 0; str[idx]!='\0' && isspace(str[idx]); idx++){
		newStr[idx] = '\0';
	}
	return newStr;
}	


int inodes_available(){
	FILE* inodeFile;
	uint32_t buffer;
	int count;
	
	inodeFile = fopen("inodes_list", "r"); 
	while (fread(&buffer, sizeof(uint32_t), 1, inodeFile)){
		if (strcmp(uint32_to_str(buffer), "f")==0 || strcmp(uint32_to_str(buffer), "d")==0){
			count = count+1;
		}
	}
	return count;
}

int check_exist(char* type, char directory[], char* curDir){	
	FILE* curDirFile;
	FILE* inode_list;
	uint32_t prevBuffer;
	uint32_t bufferDir;
	curDirFile = fopen(curDir, "r");
	inode_list = fopen("inodes_list", "r");

	while(fread(&bufferDir, sizeof(uint32_t), 1, curDirFile)){
		if (strcmp(uint32_to_str(bufferDir),directory)==0){
			//use check-type variable to know when to read if the input is a file or directory
			//from looking through inodes_list
 			uint32_t bufferInode;
			int check_type = 0;
			while(fread(&bufferInode, sizeof(uint32_t),1, inode_list)){
				if(check_type==1){
					return (strcmp(type, uint32_to_str(bufferInode)));
				}
				else if (bufferInode == prevBuffer){
					check_type = 1;
				}
			}			
		}
		prevBuffer = bufferDir;				
	
	}
	return -1;			
}

void cd_command(char directory[], char *curDir, char *prevDir){
	//check if directory exists
	char type[MAX_CHAR];
	type[MAX_CHAR-2] = 'd';
	char* typeChar = remove_leading(type);  
	if (check_exist(typeChar, directory, curDir)==0){
		printf("Input not a directory or does not exist");
	}
	//check for ., go to root directory
	if (strcmp(directory, ".")==0){
		curDir = uint32_to_str(0);
	}	
	//check for .., go to prevDir
	else if (strcmp(directory, "..")==0){
		if (prevDir == NULL){
			prevDir = "0";
		}
		curDir = prevDir;
	}
	// if dir exist, set curDir to it
	else{
		prevDir = curDir;
		curDir = directory; 	
	}
}

void ls_command(char *curDir){
  	FILE* curFile;
	uint32_t buffer = 0;

	//search for file of curDir
	curFile = fopen(curDir, "r"); 

 	while(fread(&buffer, sizeof(uint32_t),1, curFile)){
		char* output = uint32_to_str(buffer);
        	printf("%s\n", output);
        }
	fclose(curFile);
}

void add_command(char directory[], char *type, char *curDir){
	//check if inode available, check inodes_list??
	if (inodes_available()==MAX_INODE){
		printf("No inodes avaiable");
	}	
	FILE *newDir;
	FILE *curDirFile;
	FILE *inodesFile;

	if (strcmp(type, "d")==0){
		//check if directory exists
		if (check_exist(type, directory, curDir)==0){
			printf("Directory already exists");
		}
		newDir = fopen(directory, "a+");
		char* fileInput;
		char rootChar[MAX_CHAR];
		char prevChar[MAX_CHAR];

		rootChar[31] = '.';
		prevChar[30] = '.';
		prevChar[31] = '.';
	 
		char* rootDir;
		char* prevDir;
		rootDir = remove_leading(rootChar);
		prevDir = remove_leading(prevChar);
		
		fileInput = rootDir;
		//generate string to add to file
		strcat(fileInput, rootDir);
		strcat(fileInput, prevDir);
		strcat(fileInput, prevDir); 

        	fwrite(fileInput, 1, sizeof(uint32_t), newDir);
        	fclose(newDir);	
	}
	else if (strcmp(type, "f")==0){
	 	//check if file exists	
	 	if (check_exist(type, directory, curDir)==0){
	        	perror("File already exists\n");
		}
		newDir = fopen(directory, "a+");
		fputs(directory, newDir);
		fclose(newDir);	
	}

	//open currDir and add new directory/file names and inode to it
	curDirFile = fopen(curDir, "w");
	
	char* curFileInput = directory;
	strcat(curFileInput, directory);
	fwrite(curFileInput, 1, sizeof(uint32_t), curDirFile);
	fclose(curDirFile);	 
	//add to inodes_list
	char* inodeFileInput = directory;
	char type32Char[MAX_CHAR];
	type32Char[MAX_CHAR-2] = *type;

	strcat(inodeFileInput, type32Char);
	inodesFile = fopen("inodes_list", "w");
	fwrite(inodeFileInput, 1, sizeof(uint32_t), inodesFile);
	fclose(inodesFile);		
}

int check_command(char* command, char dirInput[], char* startDir){ 
	//check if the argumnt is 32 chars 
	if (strlen(dirInput) > MAX_CHAR){ 
		dirInput[strlen(dirInput)-MAX_CHAR] = '\0'; 
	} 
	//remove leading space chars 
	char* directory; 
	directory = remove_leading(dirInput); 

	//set current directory and initialize previous directory 
	char* curDir = startDir; 
	char* prevDir = NULL; 
	if (command!=NULL){
		if (directory!=NULL){ 
			//check commands 
			if (strcmp(command, "cd")==0){
				cd_command(directory, curDir, prevDir); 
			} 
			else if(strcmp(command,"mkdir")==0){ 
				char* typeInput = "d";
				add_command(directory, typeInput, curDir); 
			} 
			else if(strcmp(command, "touch")==0){ 
				char* typeInput = "f";
				add_command(directory, typeInput, curDir); 
			}  
		}
		else{ 
			//if exit or ls 
			if (strcmp(command, "ls")==0){ 
				ls_command(curDir); 
			} 
			else if (strcmp(command, "exit")==0){ 
				exit(0); 
			}
		}
	} 
	else{ 
		printf("Invalid command"); 
	} 
	return 1; 
}

int main(int argc, const char *argv[]){
	if (argc>2){
		printf("Invalid number of arguments");
		exit(1);
	}
	//load file system dir
	if (chdir(argv[1])==-1){
		printf("File system directory does not exist or not in use");
                exit(1);
	}

	//initialize starting directory
	char* startDir = "0";
	
	//take stdin
	int inputCount = 0;	
	char* command;
	char dirInput[MAX_CHAR];

	char* line = NULL;
   	size_t size;
   	const char s[2] = " ";
   	char* token = NULL;

   	while(getline(&line, &size, stdin)>0){
		token = strtok(line, s);
		if (token==NULL){
			printf("No command or argument stated");		
		}
        	while (token!=NULL){
			inputCount++;
			if (inputCount == 1){
				command = token;
				if (strcmp(command, "ls")==0 || strcmp(command, "exit")==0){
					check_command(command,  dirInput, startDir);;
				}
			}
			else if (inputCount == 2){
				dirInput[MAX_CHAR-strlen(token)-1] = *token;
			}
			else if(inputCount == 3){
				if (strcmp(token, "\n")){
					check_command(command, dirInput, startDir);
					inputCount = 0;			
				}
				else{
					printf("Invalid number of arguments");
				}
			}
                	token = strtok(NULL, s);
             	}	
   	}
   	free(line);
	return 1;
}
