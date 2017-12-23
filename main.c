#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


/*********************************************
			DEFINES
*********************************************/
#define MAX_NR_COMMAND_ARGUMENTS 10
#define MAX_LENGTH_STRING 255

/*********************************************
			GENERAL FUNCTIONS
*********************************************/
/**
 * Split string by a delimiter
 * @param char * word      
 * @param char * delimiter 
 * @return char**
 */
char** split(char * word, const char * delimiter){ 
	int i;
	char ** result = (char**) malloc(MAX_NR_COMMAND_ARGUMENTS * sizeof(char *));
	
	for(i=0; i<MAX_NR_COMMAND_ARGUMENTS; i++) {
		result[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
	}

	char *token = strtok(word, " ");

	i=0;
	while(token) {
	    puts(token);
	    result[i] = token;
	    i++;
	    token = strtok(NULL, " ");
	}
	return result;
}

/**
 * Calculate the length of a char**
 * @param  char * string
 * @return int
 */
int stringLength(char ** string) {
	int i=0;
	int length=0;

	while((i<MAX_NR_COMMAND_ARGUMENTS) && (string[i] != NULL && string[i][0] != '\0')) {
		length++;
		i++;
	}

	return length;
}

/**
 * Transform a char* to int
 * @param char * number
 * @return  int
 */
int toInt(char * number) {
	int result = 0;
	int i=0;
	for(i=0; i<strlen(number); i++ ){
		 result = result*10 + (number[i]-'0');
	}
	return result;
}


/*********************************************
			MAIN
*********************************************/
int main(int argc, char const *argv[])
{	
	int i=0;
	
	// Init variables	
	char * line = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
	size_t bufsize = MAX_LENGTH_STRING;
	char ** commands = (char**) malloc(MAX_NR_COMMAND_ARGUMENTS * sizeof(char *));

	for(i=0; i<=MAX_NR_COMMAND_ARGUMENTS; i++) {
		commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
	}

	printf("Terminal started\n");

	// Run terminal infinitely
	while(1) {
		printf("@");
		getline(&line, &bufsize, stdin);

		commands = split(line, " ");

		printf("Commands separated:\n");

		for(i=0; i<stringLength(commands); i++) {
			printf("%s\n", commands[i]);
		}

		// printf("%line\n", line[0] );
	}

	return 0;
}