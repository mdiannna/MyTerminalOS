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
			TO DOS
*********************************************
*
*vezi man sh for more info about the shell
*  echo "ssdg" > test.txt
* 
* ls -A = la  (ceva din manualul de sh)
* 
* !! cd daca nu merge sa dea eroare& in general comenzi eroare
* !! use pipe for running which ????
* pipe info: https://stackoverflow.com/questions/7292642/grabbing-output-from-exec
* http://www.microhowto.info/howto/capture_the_output_of_a_child_process_in_c.html
**********************************************/

/*********************************************
			DEFINES
*********************************************/
#define MAX_NR_COMMAND_ARGUMENTS 20
#define MAX_LENGTH_STRING 255
#define MAX_LINES_OUTPUT 20


/*********************************************
			ENVIRONMENT
*********************************************/
extern char **environ;


/*********************************************
			COLORS
*********************************************/
/******************************
WARNING: Colors will work on most UNIX systems but
	are not supported in Win32 console component of Microsoft Windows 
	before Windows 10 update TH2
more information here: https://en.wikipedia.org/wiki/ANSI_escape_code
*****************************/
static char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
static char green[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
static char yellow[] = { 0x1b, '[', '1', ';', '3', '3', 'm', 0 };
static char blue[] = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };
static char magenta[] = { 0x1b, '[', '1', ';', '3', '5', 'm', 0 };
static char cyan[] = { 0x1b, '[', '1', ';', '3', '6', 'm', 0 };
static char white[] = { 0x1b, '[', '1', ';', '3', '7', 'm', 0 };

void printColor(const char * color) {
	if(color == "red"){
		printf("%s", red);
	}
	if(color == "green"){
		printf("%s", green);
	}
	if(color == "yellow"){
		printf("%s", yellow);
	}
	if(color == "blue"){
		printf("%s", blue);
	}
	if(color == "magenta"){
		printf("%s", magenta);
	}
	if(color == "cyan"){
		printf("%s", cyan);
	}
	if(color == "white"){
		printf("%s", white);
	}
}


/*********************************************
			GENERAL FUNCTIONS
*********************************************/
/**
 * Split string by a delimiter
 */
char** split(char * word, const char * delimiter){ 
	int i;
	char ** result = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));
	
	for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
		result[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
	}

	char  * token = (char*) malloc(MAX_LENGTH_STRING * sizeof(char));

	token = strtok(word, delimiter);

	i=0;
	while(token) {
		strcpy(result[i], token);

	    if(token[0]=='"') {
    		token = strtok(NULL, "\"");
	    	
	    	strcat(result[i], " ");
	    	strcat(result[i], token);
	    	strcat(result[i], "\"");

	    }
	    // puts(result[i]);

	    token = strtok(NULL, " \n");

	    i++;
	}
	return result;
}


/**
 * Calculate the length of a char**
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
 */
int toInt(char * number) {
	int result = 0;
	int i=0;
	for(i=0; i<strlen(number); i++ ){
		 result = result*10 + (number[i]-'0');
	}
	return result;
}


/**
 * Run a command in terminal and get its output 
 * (used for getting the output of "which command")
 */
char ** getCommandOutput(char * command) {
	FILE *fp;
	char path[1035];

	/* Open the command for reading. */
	fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command 'which'\n" );
		exit(1);
	}

	char ** output = (char**) malloc(MAX_LINES_OUTPUT * sizeof(char*));
	int i=0;
	output[i] = fgets(path, sizeof(path)-1, fp);
	i++;
	/* Read the output a line at a time - output it. */
	while ( output[i]!= NULL) {
		output[i] = fgets(path, sizeof(path)-1, fp);
		i++;
	}

	/* close */
	pclose(fp);
	return output;
}

/**
 * Change directory
 */
void changeDirectory(char * directoryName) 
{
	chdir(directoryName);
}



/*********************************************
			MAIN
*********************************************/
int main(int argc, char const *argv[])
{	
	int i=0;
	size_t bufsize = MAX_LENGTH_STRING;
		
	// set terminal color to magenta
	printColor("magenta");
	printf("Terminal started\n");
	// reset terminal color to white
	printColor("white");

	pid_t pid;


	// Run terminal infinitely (until exit)
	while(1) {

		// Allocate memory for variables	
		char * line = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
		char * command = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
		char ** commands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));
		for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
			commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
		}
		char * which = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
			

		// set yellow color in terminal
		printColor("yellow");

		// print terminal specific symbol
		printf("@");

		//read line from terminal
		getline(&line, &bufsize, stdin);
		// printf("%s\n", line);
		
		//reset color to white in terminal
		printColor("white");

		// split read string into command and arguments
		commands = split(line, " \n");

		//if no command specified, do nothing
		if(stringLength(commands) ==0 ) {
			continue;
		}

		// Change directory if "cd" command
		if(!strcmp(commands[0], "cd")) {
			changeDirectory(commands[1]);
			continue;
		}

		// Exit terminal
		if(!strcmp(commands[0], "exit")) {
			printColor("red");
			printf("Exit terminal\n");
			printColor("white");
			return 0;
		}

		// terminate commands array with NULL for execve
		commands[stringLength(commands)] = NULL;
		
		strcpy(which, "which ");
		strcat(which, commands[0]);	

		// run "which <command>" and get the full command path
		char ** whichCommandPath = getCommandOutput(which);			
		
		if(whichCommandPath[0] == NULL){
			printColor("red");
			printf("%s: Command not found\n", line);
			printColor("white");
			continue;
		}

		strcpy(command, whichCommandPath[0]);
		command = strtok(command, " \n");

		printf("\n_____________________\n\n");


		// fork process
 		pid = fork();

 		if(pid<0) {
			return errno;
		} else if(pid==0) {
			// execute command with arguments and environment variables
			execve(command, commands, environ);
			perror(NULL);
			return errno;
		} 


		pid_t child_pid = wait(NULL);
		if(child_pid < 0){
			perror(NULL);
			return errno;
			return 0;
		}
		else {
			printf("\n_____________________\n\n");
		}	

		// Cleanup
		free(line);
		free(command);
		free(commands);
	}

	return 0;
}