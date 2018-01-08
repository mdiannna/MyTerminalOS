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
#define HISTORY_SIZE 10


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

/**
 * Print history
 */
 int print_history(char **history, int commands_run) 
 {
 	int n = HISTORY_SIZE + 1;
 	int last_index = (commands_run - 1) % HISTORY_SIZE;
 	int i;

 	for(i=last_index+1; i < HISTORY_SIZE; i++) {
 		if(--n <= commands_run) {
 			printf("%d %s\n", n, history[i % HISTORY_SIZE]);
 		}
 	}

 	for(i=0; i <= last_index; i++) {
 		if(--n <= commands_run; i++) { 
 			printf("%d %s\n", n, history[i % HISTORY_SIZE]);
 		}
 	}

 	return commands_run;
 } 

 /**
  * Run history
  */
int run_history(char **history, int commands_run, int num_back) 
{
	int i = (commands_run - 1 - num_back) % HISTORY_SIZE;
	int j;
	char ** commands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));
	
	for(j=0; j<MAX_NR_COMMAND_ARGUMENTS+1; j++) {
		commands[j] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
		commands[j][0] = '\0';
	}

	printf("%d: %s\n", num_back + 1, history[i]);

	size_t argc = 0;
	int k;
	j = 0;
	do {
		for(k = 0; isalpha(history[i][j]); k++, j++) {
			commands[argc][k] = history[i][j];
		}
		commands[argc++][k] = '\0';
	} while(history[i][j++] == ' ');

	commands[argc++] = NULL;

	pid_t pid;
	char * command = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));

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

	return commands_run;
}

/**
 * Save history
 */

int save_history(char **history, char *cmd_input, int commands_run) {
	int i = commands_run % HISTORY_SIZE;
	commands_run++;
	strcpy(history[i], cmd_input);
	return commands_run;
}


/*********************************************
			MAIN
*********************************************/
int main(int argc, char const *argv[])
{	
	int i=0;
	size_t bufsize = MAX_LENGTH_STRING;
	int commands_run = 0; //position on each string
	char last[100]; //last command
	int line_len = 0;
		
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
		char ** history = (char**) malloc(HISTORY_SIZE * sizeof(char*));

		for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
			commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
		}
		char * which = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));

		for(i=0; i<HISTORY_SIZE; i++) {
			history[i] = malloc(HISTORY_SIZE * sizeof(char));
			history[i][0] = '\0';
		}
			
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
		for(i=0; !isspace(line[i]); i++) {
			last[line_len++] = line[i];
		}
		last[line_len] = '\0';

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
		// History
		else if(!strcmp(commands[0], "hs")) {
			print_history(history, commands_run);		
		}
		else if(!strcmp(commands[0], "!!")) {
			run_history(history, commands_run, 0);
		}
		else if(commands[0][0] == '!' && isdigit(commands[0][1])) {
			int history_num = 0;
			if(sscanf(commands[0], "!%d", &history_num) == 1) {
				if(history_num > 0 && history_num <= HISTORY_SIZE && history_num <= commands_run) {
					run_history(history, commands_run, history_num - 1);
				}
				else {
					printColor("red");
					printf("Error: Invalid history request\n");
				}
			}
			else{
				printColor("red");
				printf("Something went wrong!\n");
			}
		}

		// terminate commands array with NULL for execve
		commands[stringLength(commands)] = NULL;
		commands_run = save_history(history, last, commands_run);
		
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