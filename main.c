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
#define MAX_LINES_OUTPUT 20

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

	char *token = strtok(word, " \n");

	i=0;
	while(token) {
	    puts(token);
	    result[i] = token;
	    i++;
	    token = strtok(NULL, " \n");
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
		printf("%s", path);
		output[i] = fgets(path, sizeof(path)-1, fp);
		i++;
	}

	/* close */
	pclose(fp);
	return output;
}

/*********************************************
			MAIN
*********************************************/
int main(int argc, char const *argv[])
{	
	int i=0;
	
	// Init variables	
	char * line = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
	char * command = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
	size_t bufsize = MAX_LENGTH_STRING;
	char ** commands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));

	for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
		commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
	}

	printf("Terminal started\n");

	pid_t pid;

	// Run terminal infinitely
	while(1) {
		printf("@");
		getline(&line, &bufsize, stdin);
		printf("%s\n", line);

		commands = split(line, " ");

		printf("Commands separated:\n");
		printf("String length: %d\n", stringLength(commands));
		for(i=0; i<stringLength(commands); i++) {
			printf("%s\n", commands[i]);
		}
		printf("#%s\n", commands[0]);

		// char *childargv[] = {"", input_nr_char, NULL};
		char *childargv[] = {"", NULL};

		// if (stringLength(commands) > 1) {
			// commands[stringLength(commands)] = NULL;
		// } else {
			// commands[stringLength(commands)] = "";
		commands[stringLength(commands)+1] = NULL;
		commands[MAX_NR_COMMAND_ARGUMENTS] = NULL;
		// }
		// 
		printf("Argumente:\n" );

		for(i=0; i<=stringLength(commands+1); i++) {
			printf("%s\n", (commands+1)[i]);
		}

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

		printf("%s\n", which);		

		char ** lsOutput = getCommandOutput(which);
		strcpy(command, lsOutput[0]);
		// printf("%s\n", lsOutput[0] );		



			// printf("Command to be executed: %s\n", command);
			// printf("Command to be executed: %s", command);
			
			command = strtok(command, "\n");
			printf("%s", command);
			printf("))))))))))))))%s\n",(commands+1)[0] );
			printf("))))))))))))))%s\n",(commands+1)[1] );
			printf("))))))))))))))%s\n",(commands+1)[2] );
			execve(command, commands+1, NULL);
			perror(NULL);
			return errno;
		} 
		// return 0;

		pid_t child_pid = wait(NULL);
		if(child_pid < 0){
			perror(NULL);
		}
		else {
			printf("Done parent %d Me %d \n", getpid(), child_pid);
		}	

		// printf("%line\n", line[0] );
	}

	return 0;
}