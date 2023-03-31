#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_ARGS 3

#define NUM_OBJS 16
#define COUNTY_IDX 0
#define STATE_IDX 1
#define ED_BACH_IDX 2
#define ED_IDX_HIGH_IDX 3
#define ETH_INDIAN_IDX 4
#define ETH_ASIAN_IDX 5
#define ETH_BLACK_IDX 6
#define ETH_HIS_IDX 7
#define ETH_NATIVE_IDX 8
#define ETH_TWO_IDX 9
#define ETH_WHITE_IDX 10
#define ETH_WHITE_NOT_IDX 11
#define INCOME_MED_IDX 12
#define INCOME_CAP_IDX 13
#define INCOME_POV_IDX 14
#define POP_IDX 15
struct Demo;

struct Demo
{
	struct Demo *next;
	char* objs[NUM_OBJS];
};

struct DemoList
{ 
	struct Demo *demoHead;	
};

int validate_args(int argc, char *argv[]){
	if (argc != NUM_ARGS){
		printf("Invalid number of args\n");
		exit(1);
	}
	FILE* demoFile;
	FILE* opFile;
	if ((demoFile=fopen(argv[1], "r") )== NULL){
		printf("County Demo file cannot open or do not exist");
		exit(1);
	}
	else if ((opFile=fopen(argv[2], "r"))==NULL){
		printf("Operations file cannot open or do not exist");
		fclose(demoFile);
                exit(1);
	}
	else{
		fclose(demoFile);
		fclose(opFile);
	}
	return 0;
}

int total_pop(struct DemoList *list){
	struct Demo *ptr = list->demoHead;
	int count = 0;
	while(ptr!=NULL){
		int pop;
		sscanf((ptr->objs)[POP_IDX], "%d", &pop);
		ptr = ptr->next;
		count += pop;
	}
	printf("2014 population: %d\n", count);
	return count;
}

void filter_state(struct DemoList *list, char* target){
	struct Demo *ptr = list->demoHead;
	int count = 0;
	while(ptr!=NULL){
		if(strcmp((ptr->objs)[STATE_IDX], target)==0){
			count++;
		}
		ptr = ptr->next;
	}
	printf("Filter: state == %s (%d entries)\n", target, count);

}

void demoPopPer(struct DemoList *list, char* line, char* operation, char* filterState){
	struct Demo *ptr = list->demoHead;
	int idx =0;
	if (strcmp(line, "Education.Bachelor's Degree or Higher\n")==0){
		idx = 2;
	}
	else if (strcmp(line, "Education.High School or Higher\n")==0){
		idx = 3;
	}
	else if (strcmp(line, "Ethnicities.American Indian and Alaska Native Alone\n")==0){
		idx =4;
	}
	else if (strcmp(line, "Ethnicities.Asian Alone\n")==0){
		idx = 5;
	}
	else if (strcmp(line, "Ethnicities.Black Along\n")==0){
		idx = 6;
	}
	else if (strcmp(line, "Ethnicities.Hispanic or Latino\n")==0){
		idx = 7;
	}
	else if (strcmp(line, "Ethnicities.Native Hawaiian and Other Pacific Islander Alone\n")==0){
		idx = 8;
	}
	else if (strcmp(line, "Ethnicities.Two or More Races\n")==0){
		idx = 9;
	}
	else if (strcmp(line, "Ethnicities.White Alone\n")==0){
		idx = 10;
	}
	else if (strcmp(line, "Ethnicities.White Alone, not Hispanic or Latino\n")==0){
		idx = 11;
	} 
	else if (strcmp(line, "Income,Persons Below Poverty Level\n")==0){
		idx = 13;
	}
	float pop=0;
	while (ptr!=NULL){
		if (filterState== NULL || (strcmp(filterState, ((ptr->objs)[STATE_IDX]))==0)){
			float num;
			sscanf((ptr->objs)[idx], "%f", &num);
			pop += num;
			ptr=ptr->next;
		}			
	}
	char* linePrint = strtok(line, "\n");
	if (strcmp(operation, "percent")==0){
		struct Demo *newPtr = list->demoHead;
		float totalPer = 0;
		while (ptr!=NULL){
			float percent;
			if (filterState==NULL || (strcmp(filterState, ((ptr->objs)[STATE_IDX]))==0)){
				sscanf((ptr->objs)[idx], "%f", &percent);
				float population;
				sscanf((ptr->objs)[idx], "%f", &population);
				totalPer+=(percent/100.0)*population;
				newPtr=newPtr->next;
			}
		}
		int totalPop = total_pop(list); 
		float avgPercent = totalPer/totalPop;

		printf("2014 %s percentage: %f\n", linePrint, avgPercent); 
	}else{
		printf("2014 %s population: %f\n", linePrint, pop);
	}
	linePrint = strtok(NULL, "\n");
}
int demo_program(FILE* demoFile, FILE* operationFile){
	struct DemoList *demoList = malloc(sizeof(struct DemoList*));
	demoList->demoHead = NULL;
	
	int recordsNum = 0;	
	struct Demo *curDemo= malloc(sizeof(struct Demo *));
	char* line = NULL;
	size_t size;
	int c = getline(&line, &size, demoFile);
	while((c=getline(&line, &size, demoFile)) >=0){
		struct Demo *newDemo = malloc(sizeof(struct Demo *));
		*(newDemo->objs) = malloc(16*sizeof(char *));
		newDemo->next = NULL;
		if(demoList-> demoHead ==NULL){
			demoList->demoHead=newDemo;
			curDemo = demoList->demoHead;
		}
		else{
			curDemo->next = newDemo;
			curDemo = curDemo->next;
		}
		char* token= strtok(line, "\"");
		int idx = 0;
		int demoIdx=0;
		while(demoIdx<2){
			if (token==NULL && idx!=0){
				printf("Not enougth information for line %d\n", recordsNum);
				continue;
			}
			if(idx%2==0){
				((curDemo->objs)[demoIdx]) = strdup(token);
				demoIdx++;
			
			}
			token = strtok(NULL, "\"");
			idx++;
		}
		
		for (int i=0; i<6; i++){
			if (token==NULL){
				printf("Not enought information for line %d", recordsNum);
				continue;			
	
			}
			token = strtok(NULL, "\"");
		}
		while(demoIdx < 4){
			if(token == NULL){
				printf("Not enough information for line %d", recordsNum);
				continue;				
			}
			if(idx%2==0){
				((curDemo->objs)[demoIdx]) = strdup(token);
				demoIdx++;
				
			}
			token = strtok(NULL, "\"");
			idx++;
		}
		for (int j=0; j<8; j++){
			if(token==NULL){
				printf("Not enough information for line %d", recordsNum);
				continue;
			}
			token = strtok(NULL, "\"");
		}
		while(demoIdx<12){
			if(token ==NULL){
				printf("Not enough information for line %d", recordsNum);
				continue;
			}
			if(idx%2==0){
				((curDemo->objs)[demoIdx]) = strdup(token);
				demoIdx++;
		
			}
			token = strtok(NULL, "\"");
			idx++;
		}
		for(int k=0; k<12; k++){
			if(token==NULL){
				printf("Not enough information for line %d", recordsNum);
				continue;
			}
			token = strtok(NULL, "\"");
		}
		while(demoIdx<15){
			if(token==NULL){
				printf("Not enough information for line %d", recordsNum);
				continue;
			}
			if(idx%2==0){
				((curDemo->objs)[demoIdx]) = strdup(token);
				demoIdx++;
		
			}
			token = strtok(NULL, "\"");		
			idx++;
		}
		for(int l=0; l<21; l++){
			if (token == NULL){
				printf("Not enough information for line %d", recordsNum);
				continue;
			}
			token = strtok(NULL, "\"");
		}	
		if (token==NULL){
			printf("Not enough information for line %d", recordsNum);
			continue;	
		}
		else{
			(curDemo->objs)[demoIdx] = strdup(token);
			token = strtok(NULL, "\"");
		} 			
		recordsNum++;
			
	}
	printf("%d records loaded\n", recordsNum);

	
	char* opLine = NULL;
	size_t opSize;
	int op;

	char* filterState = NULL;
	while((op=getline(&opLine, &opSize, operationFile))>=0){
		if (strcmp(opLine, "population-total")==0 || strcmp(opLine, "population-total\n")==0){
			total_pop(demoList);
			return 0;
		}
		char* word = strtok(opLine, ":");
		if (strcmp(word, "filter-state")==0){
			word = strtok(NULL, ":");
			char* rmNewLine = strtok(word, "\n");
			char* target = strdup(rmNewLine);
			filterState = target; 
			rmNewLine = strtok(NULL, "\n");
			filter_state(demoList, target);
			word = strtok(NULL, ":");
		}
		else if (strcmp(word, "population")==0 || strcmp(word, "percent")==0){
			char* operation = strdup(word);
			word = strtok(NULL, ":");
			demoPopPer(demoList, strdup(word), operation, filterState);
			word = strtok(NULL, ":");		
		}
		
		word = strtok(opLine, ":");	
	}

	return 0;	
}

int main(int argc, char *argv[]){
	validate_args(argc, argv);
	FILE* demoFile = fopen(argv[1], "r");
	FILE* operationFile = fopen(argv[2], "r");

	demo_program(demoFile, operationFile);
	fclose(demoFile);
	fclose(operationFile);
}
