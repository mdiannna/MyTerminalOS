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
			QUESTIONS
*********************************************
*execve vs system
*
* eroarea '' not found - inainte de ultimul NULL
**********************************************/

/*********************************************
			TO DOS
*********************************************
*SPLIT-uieste "something lalala" ca un string impreuna, nu le separa dupa spatii
*
*vazi man sh for more info about the shell
*
* sudo & comenzi cu sudo
* - sudo apt-get update etc
* 
* commands not working:
* - ls -la 
* - gcc
* This WORKS:
execl("/usr/bin/gcc", "gcc", "test.c", "-o", "test2", NULL);
* - git commit & push
*
* -strace ls de ex. tb scris ca strace /bin/ls
* 
* autocomplete with tab (some code done)
*
* ls -A = la  (ceva din manualul de sh)
**********************************************/

/*********************************************
			DEFINES
*********************************************/
#define MAX_NR_COMMAND_ARGUMENTS 20
#define MAX_LENGTH_STRING 255
#define MAX_LINES_OUTPUT 20


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
 * @param char * word      
 * @param char * delimiter 
 * @return char**
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
	    // result[i] = token;

	    if(token[0]=='"') {
    		token = strtok(NULL, "\"");
	    	
	    	strcat(result[i], " ");
	    	strcat(result[i], token);
	    	strcat(result[i], "\"");

	    }
	    puts(result[i]);

	    token = strtok(NULL, " \n");

	    i++;
	    // token = strtok(NULL, "\n");
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

/**
 * Run a command in terminal and get its output
 * @param  char * command 
 * @return char **
 */
char ** getCommandOutput(char * command) {
	FILE *fp;
	char path[1035];

	/* Open the command for reading. */
	fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	}

	char ** output = (char**) malloc(MAX_LINES_OUTPUT * sizeof(char*));
	int i=0;
	output[i] = fgets(path, sizeof(path)-1, fp);
	i++;
	/* Read the output a line at a time - output it. */
	while ( output[i]!= NULL) {
		// printf("%s", path);
		output[i] = fgets(path, sizeof(path)-1, fp);
		i++;
	}

	/* close */
	pclose(fp);
	return output;
}

/**
 * Change directory
 * @param char * directoryName 
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
		
	printColor("magenta");
	printf("Terminal started\n");
	printColor("white");

	pid_t pid;


	// char *childargv[] = {"", input_nr_char, NULL};
	// char *childargv[] = {"gcc", "test.c", "-o", "test2", NULL};

	// execve("/usr/bin/gcc", childargv, NULL);
	
	// This WORKS:
	// execl("/usr/bin/gcc", "gcc", "test.c", "-o", "test2", NULL);
	// This WORKS:
	// execl("/usr/bin/subl", "test.c", NULL);
	// return 0;


	// Run terminal infinitely
	while(1) {

		// Init variables	
		char * line = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
		char * command = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
		char ** commands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));

		for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
			commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
		}

		printColor("yellow");
		printf("@");
		getline(&line, &bufsize, stdin);
	

		// printf("%s\n", line);
		printColor("white");

		// system(line);

		commands = split(line, " ");
		if(stringLength(commands) ==0 ) {
			continue;
		}



		// printf("Commands separated:\n");
		// printf("String length: %d\n", stringLength(commands));
		// for(i=0; i<stringLength(commands); i++) {
		// 	printf("%s\n", commands[i]);
		// }
		// printf("#%s\n", commands[0]);
		// printf("%d", strcmp("cd", "cd"));

		// Change directory
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


	
		// if (stringLength(commands) > 1) {
			// commands[stringLength(commands)] = NULL;
		// } else {
			// commands[stringLength(commands)] = "";
		

		// WHY THIS GIVES SEGMENTATION FAULT???
		// commands[stringLength(commands)] = NULL;
		// printf("length(commands:%d\n", stringLength(commands)+1);
		// commands = (char **) realloc(commands, (stringLength(commands)+1) * sizeof(char*));
		// commands[stringLength(commands)] = NULL;
		// commands[stringLength(commands)] = '\0';
		// commands[stringLength(commands)+1] = NULL;
		// commands[MAX_NR_COMMAND_ARGUMENTS] = NULL;
		// 
		// 
		commands[stringLength(commands)] = NULL;
		
		// }
		// 
		// printf("Argumente:\n" );

		// for(i=0; i<stringLength(commands); i++) {
		// 	printf("%s\n", (commands)[i]);
		// }



 		pid = fork();

 		if(pid<0) {
			return errno;
		} else if(pid==0) {
			// strcpy(command, "/bin/");
			// strcat(command, commands[0]);
			// strcat(command, "ls");

			// char ** lsOutput = getCommandOutput("/bin/ls");
			char * which = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
			strcpy(which, "which ");
			strcat(which, commands[0]);

			// printf("%s\n", which);		

			char ** lsOutput = getCommandOutput(which);
			
			if(lsOutput[0] == NULL){
				printColor("red");
				printf("%s: Command not found\n", line);
				printColor("white");
				continue;
			}

			strcpy(command, lsOutput[0]);



			// printf("Command to be executed: %s\n", command);
			// printf("Command to be executed: %s", command);
			
			command = strtok(command, " \n");
			// printf("%s", command);
			// printf("))))))))))))))%s\n",(commands)[0] );
			// printf("))))))))))))))%s\n",(commands)[1] );
			// printf("))))))))))))))%s\n",(commands)[2] );
			// printf("))))))))))))))%s\n",(commands)[3] );
			printf("\n_____________________\n\n");
			// first argument in second argument should be the name of the file
			// if(stringLength(commands) ==1) {
				// execl(command, command, NULL);
			// } else {
				execv(command, commands);
			// }

			perror(NULL);
			return errno;
			return 0;

		} 



		pid_t child_pid = wait(NULL);
		if(child_pid < 0){
			perror(NULL);
			return errno;
		}
		else {
			printf("\n_____________________\n\n");
			// printColor("green");
			// printf("Done parent %d Me %d \n", getpid(), child_pid);
			// printColor("white");
		}	

		// printf("%line\n", line[0] );
		
		// Cleanup
		free(line);
		free(command);
		free(commands);
	}

	return 0;
}