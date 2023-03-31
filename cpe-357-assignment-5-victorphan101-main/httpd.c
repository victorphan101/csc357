#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define PORT 1560

int check_cgi(char* filename){
	char* token = strtok(filename, "/");
	if (token != NULL){
		token = strtok(NULL, "/");
		token = strtok(filename, "/");
		if(strcmp(token, "cgi-like")==0){
			token = strtok(NULL, "/");
			return 0;			
		}
	}
	token = strtok(NULL, "\\");
	return -1;
}

void exec_cgi(char* type, char* filename, int nfd){
	//chdir("/cgi-like");
	char* line = filename+10;
	char* execCommand;
	char* execArgs;	

	//find ? to start looking for arguments
	char* argsLine;
	if ((argsLine = strchr(line, '?'))==NULL){
		execCommand = line;
		execArgs = NULL;
	}
	else{ 
		//use index from start and until ? and place into a char* for execvp name
		char* args = ++argsLine;
		//new string, named execArgs
		char* token = strtok(args, "&");

		if (token==NULL){
			token = strtok(NULL, "&");
			execArgs = NULL;
		}
		else{
			execArgs = strdup(token);
			token = strtok(NULL, "&");
			while((token=strtok(args, "&"))!=NULL){
				strcat(execArgs, strdup(token));
				strcat(execArgs, "\0"); 	
				token = strtok(NULL, "&");
			}
		}

		//find command
		argsLine = "\0";
		execCommand = line;
	}

	//if -1 -> error 500 Internal error and exit(1)
	pid_t pid = fork();
	char fileName[20];
	sprintf(fileName, "%d", (int)pid);
	if (pid==-1){
		printf("HTTP 1.0 500 Internal Error\\r\\n\n");
		exit(1);
	}
	else if (pid==0){
		int fd = open(fileName, O_RDWR);
		dup2(fd, 1);
		dup2(fd, 2); 
		execlp(execCommand, execCommand, execArgs, NULL);
		perror("exec");
		exit(1); 		
	}
	else{
		wait(0);
		FILE* pidFile = fopen(fileName, "r");
		if (strcmp(type, "GET")==0){
	                char* replyLine = NULL;
        	        size_t fsize;
                	size_t lineSize = 0;
                	while(getline(&replyLine, &fsize, pidFile)>0){
                   		write(nfd, replyLine, lineSize);
                 	    	printf("%s\n", replyLine);
                	}
               		 free(replyLine);
        	}
		fclose(pidFile);			
	}
}


void handle_request(int nfd)
{
   chdir("0");
   FILE *network = fdopen(nfd, "r");
   char *line = NULL;
   size_t size;
   ssize_t num;
  

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }

   if ((num = getline(&line, &size, network)) >=0)
   { 
	//retrieve GET/HEAD command
	char* token = strtok(line, " ");
        char* type = strdup(token);

        token = strtok(NULL, " ");
        
        char* filename = strdup(token);

        token = strtok(NULL, " ");

	char* trimTok = strtok(token, "\n");
        char* http = strdup(trimTok);

        trimTok = strtok(NULL, "\n");
        token = strtok(NULL, " ");	

	//check and executre cgi if valid
	//change the filename into the pid
	char* checkName = strdup(filename);
	if (check_cgi(checkName)==0){ 
		exec_cgi(type, filename, nfd);
	}
        else{

	//check for file
	//read file for reply
        FILE* replyFile = fopen(filename, "r");
	int length = 0;
        struct stat lengthBuffer;
	stat(filename, &lengthBuffer);
	length = lengthBuffer.st_size;		

	char* header;
	char* contentType;
	char contentLength[50];
	char* blankLine;
	size_t replySize = 0;

        header = http;
	if (replyFile==NULL){
		strcat(header, " 404 Not Found");
	}
	else if (strcmp(type, "GET")!=0 && strcmp(type, "HEAD")!=0){
		strcat(header, " 400 Bad Request");
	}
	else{
		strcat(header, " 200 OK");
	}
	strcat(header, "\\r\\n\n");
	contentType = "Content-Type: text/html\\r\\n\n";
	
	sprintf(contentLength, "Content-Length: %d\\r\\n\n", length);
        blankLine = "\\r\\n\n";

	write(nfd, header, replySize);
	write(nfd, contentType, replySize);
	write(nfd, contentLength, replySize);
	write(nfd, blankLine, replySize);  	


	printf("%s",header);
	printf("%s", contentType); 
	printf("%s", contentLength);
	printf("%s",blankLine);

        if (strcmp(type, "GET")==0){
 	       char* replyLine = NULL;
               size_t fsize;
	       size_t lineSize = 0;
               while(getline(&replyLine, &fsize, replyFile)>0){
   	            write(nfd, replyLine, lineSize);
		    printf("%s\n", replyLine);
               }
		free(replyLine);
        }
	fclose(replyFile);
	free(checkName);
	}
			               
   }

   free(line);
   fclose(network);
}

void handle_action(int signo){
        printf("Finished Request\n");
}


void run_service(int fd)
{
   while (1)
   {
      int nfd = accept_connection(fd);
      if (nfd != -1){
		pid_t pid;
                if ((pid=fork())==-1){
                        perror("Error fork");
                        exit(1);
                }
                else if(pid == 0){
                        printf("Connection established\n");
        		handle_request(nfd);
        		printf("Connection closed\n");
			exit(0);	
                }
                else{
			int status;
			waitpid(pid, &status, 0);
                        
			struct sigaction action;
                        if (sigemptyset(&action.sa_mask)==-1){
                                perror("empty");
                                exit(1);
                        }
                        action.sa_handler = &handle_action;
                        action.sa_flags=0;
                        if(sigaction(SIGCHLD, &action, NULL)==-1){
                                perror("sigaction");
                                exit(1);
                        }	
       			
			kill(0, SIGCHLD);	
		        exit(0);
        	
	      }
         }
   }
}

int main(int argc, char* argv[])
{
   char* inputPort = argv[1]; 
   int numVerPort; 
   sscanf(inputPort, "%d", &numVerPort);
   
   int fd = create_service(numVerPort);

   if (fd != -1)
   {
      printf("listening on port: %d\n", numVerPort);

      run_service(fd);

      close(fd);
   }

   return 0;
}
