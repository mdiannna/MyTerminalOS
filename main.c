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
#include <ctype.h>


/*********************************************
            TO DOS
*********************************************
*!!posibil bug cu ghilimelele in split()
*
* - modificat conversia cifrelor in istorie sa accepte si numere 
* de mai multe cifre
* 
*vezi man sh for more info about the shell
*  echo "ssdg" > test.txt
*
*
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
#define MAX_HISTORY_SIZE 10

/*********************************************
        FUNCTION PROTOTYPES
*********************************************/
void printColor(const char * color);
int stringLength(char ** string); 
int toInt(char * number);
char** split(char * word, const char * delimiter);
int runCommandFromLine(char * line, char ** history, int * run_command); 
int save_history(char **history, char *cmd_input, int commands_run);
int run_history(char **history, int commands_run, int num_back);
int print_history(char **history, int commands_run);
int nrOfPipes(char * line);
void changeDirectory(char * directoryName);
char ** getCommandOutput(char * command);


/*********************************************
            ENVIRONMENT
*********************************************/
extern char **environ;


/*********************************************
            COLORS
*********************************************/
/******************************
WARNING: ANSI codes were not supported on all Operating Systems
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
       
        // token = strtok(NULL, " \n");
        token = strtok(NULL, delimiter);
        printf("Token: %s\n", token);
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
    if(chdir(directoryName) == -1) {
        if (errno == 2) {
            printColor("red");
            printf("No such file or directory\n");
            printColor("white");    
            return;
        }
        // else
        perror(NULL);
    }
}


/*
Find number of pipes
 */
int nrOfPipes(char * line) {
    int count = 0;
    int i=0;
    char quotations = 0;

    while(line[i]!='\0') {
        if(line[i] == '"' && quotations == 0) {
            quotations = 1;
        } else if(quotations == 1) {
            if(line[i] == '"') {
                quotations = 0;
            }
        } else if(line[i] == '|') {
            count++;
        }
        i++;
    }

    return count;
}



/*********************************************
            HISTORY
*********************************************/
/**
 * Print history
 */
 int print_history(char **history, int commands_run)
 {
    printColor("cyan");
    printf("History:\n");
    printColor("white");

    int n = MAX_HISTORY_SIZE + 1;
    int last_index = (commands_run - 1) % MAX_HISTORY_SIZE;
    int i;

    for(i=last_index+1; i < MAX_HISTORY_SIZE; i++) {
        if(--n < commands_run) {
            printf("%d %s\n", n, history[n]);
        }
    }

    for(i=0; i <= last_index; i++) {
        if(--n <= commands_run) {
            printf("%d %s\n", n, history[i % MAX_HISTORY_SIZE]);
        }
    }

    return commands_run;
 }


 /**
  * Run history 
  */
int run_history(char **history, int commands_run, int num_back) {
    
    int i = (commands_run - 1 - num_back) % MAX_HISTORY_SIZE;
    int j;

    printf("Run history\n");
    printf("%d: %s\n", num_back + 1, history[i]);
    char * commandToRun = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));

    strcpy(commandToRun,history[i]);

    printf(" #1 Commands run: %d\n", commands_run );
    commands_run = save_history(history, commandToRun, commands_run);
    printf(" #2 Commands run: %d\n", commands_run );

    runCommandFromLine(commandToRun, history, &commands_run);
    printf(" #3 Commands run: %d\n", commands_run );


    free(commandToRun);
    return commands_run;
}


 /**
 * Save history
 */
 int save_history(char **history, char *cmd_input, int commands_run) {
    //for debug
    // printf("commands run: %d\n", commands_run);

    int i = commands_run % MAX_HISTORY_SIZE;
    commands_run++;

    //for debug
    // printf("cmd_input:%s\n", cmd_input );

    strcpy(history[i], strtok(cmd_input, "\n"));

    //for debug
    // printf("History[i]: %s\n", history[i]);

    
    return commands_run;
}
/*******************************/



int runPipes(char * line) {
    
    pid_t cpid;
    char buf;

    int i = 0;
    int hasPipe = 0;

    char ** pipeCommands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));
    for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
        pipeCommands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
    }

    char ** commands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));
    for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
        commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
    }

    char * command = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
    char * which = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));


    pipeCommands = split(line, "|");
    printf("pipeCommands[0]: %s\n", pipeCommands[0]);

    // TODO: foreach pipeCommands the following
    commands = split(line, " \n");
    printf("commands[0]: %s\n", commands[0]);
    

    printf("stringLength(commands: %d)\n", stringLength(commands));
    // int pipefd[stringLength(commands)];
    int pipefd[2];


    // for (i=0; i<stringLength(commands); i++) {
    //     printf("i=%d\n",i );
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        else {
            printColor("green");
            printf("pipe created\n");
            printColor("white");
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
        return -1;
    }

    strcpy(command, whichCommandPath[0]);
    command = strtok(command, " \n");




        cpid = fork();
        

        if(cpid<0) {
            return errno;
        }
        if (cpid == 0) {    /* Child reads from pipe */
            printf("PIPE :) \n");
           //close(pipefd[0]);          /* Close unused read end */

            // dup2 (pipefd[1], STDOUT_FILENO);
            // printf("Command: %s, commands[0]: %s\n",command, commands[0] );
            execve(command, commands, environ);
            perror(NULL);
           // close(pipefd[1]);          /* Close unused write end */
           _exit(EXIT_SUCCESS);


           // while (read(pipefd[0], &buf, 1) > 0)
           //     write(STDOUT_FILENO, &buf, 1);

           // write(STDOUT_FILENO, "\n", 1);
           // close(pipefd[0]);
           // _exit(EXIT_SUCCESS);

       } else {            /* Parent writes command to pipe */
           close(pipefd[0]);          /* Close unused read end */
           // write(pipefd[1], argv[1], strlen(argv[1]));
           write(pipefd[1], commands[0], strlen(commands[0]));
           close(pipefd[1]);          /* Reader will see EOF */
           wait(NULL);                /* Wait for child */
           // exit(EXIT_SUCCESS);
       }
   // }


  

    // if(pid<0) {
    //     return errno;
    // } else if(pid==0) {
    //     if (hasPipe) {
    //         printf("HAS PIPE :) \n");
    //         close(pipefd[0]);          /* Close unused read end */
            
    //             write(pipefd[1], commands[0], strlen(commands[0]));
    //             printf("Trying to write '%s' to parent\n", commands[0] );
    //            // write(STDOUT_FILENO, "\n", 1);

    //             close(pipefd[1]);          /* Reader will see EOF */
                

    //            // close(pipefd[0]);

    //         printf("BUF:%c\n", buf);
    //         _exit(EXIT_SUCCESS);
    //     } else {
    //         if (hasPipe) {
    //             printf("PIPE\n");
    //             close(pipefd[1]);          /* Close unused write end */
    //             // write(pipefd[1], argv[1], strlen(argv[1]));
    //         while (read(pipefd[0], &buf, 1) > 0) {
    //             write(STDOUT_FILENO, &buf, 1);
    //             printf("Buf: %c\n", buf);
    //         }
    //            close(pipefd[0]);


    //             wait(NULL);                /* Wait for child */


    //             exit(EXIT_SUCCESS);

    //         } else {
    //             // execute command with arguments and environment variables
    //             execve(command, commands, environ);
    //             perror(NULL);
    //             return errno;   
    //         }
    //     }
    // } 

   
    // pid_t child_pid = wait(NULL);
    // if(child_pid < 0){
    //     perror(NULL);
    //     return errno;
    //     return 0;
    // }
    // else {
        printf("\n_____________________\n\n");
    // }

}

/**
* 
* Run a command from string
* 
*/
int runCommandFromLine(char * line, char ** history, int * commands_run) 
{

    // Variable initialization
    pid_t pid;
    int i = 0;

    //memory allocation
    char * command = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
    char ** commands = (char**) malloc((MAX_NR_COMMAND_ARGUMENTS+1) * sizeof(char *));
    for(i=0; i<MAX_NR_COMMAND_ARGUMENTS+1; i++) {
        commands[i] =  (char*) malloc(MAX_LENGTH_STRING * sizeof(char));
    }
    char * which = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));


    printf("Nr of pipes:\n");
    printf("%d\n", nrOfPipes(line));
    int nrOfPipesInt = nrOfPipes(line);

    if (nrOfPipesInt > 0) {
        runPipes(line);
        return -1;
    } else {
        // split read string into command and arguments
        commands = split(line, " \n");
    }


    // History
    if(!strcmp(commands[0], "hs")) {
        print_history(history, *commands_run); 
        return -1;      
    }

    if(!strcmp(commands[0], "!!")) {
        // run_history(history, commands_run, 0);
        *commands_run = run_history(history, *commands_run, 1);
        return -1;
    }
    
    if(commands[0][0] == '!' && isdigit(commands[0][1])) {
        int history_num = 0;
        if(sscanf(commands[0], "!%d", &history_num) == 1) {
            if(history_num > 0 && history_num <= MAX_HISTORY_SIZE && history_num <= *commands_run) {
                *commands_run = run_history(history, *commands_run, history_num - 1);
                return -1;
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

    // Change directory if "cd" command
    if(!strcmp(commands[0], "cd")) {
        changeDirectory(commands[1]);
        return -1;
    }

    // Exit terminal
    if(!strcmp(commands[0], "exit")) {
        printColor("blue");
        printf("Exit terminal\n");
        printColor("white");
        _exit(EXIT_SUCCESS);
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
        return -1;
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

    free(command);
    free(commands);
    free(which);
    // free(pipefd);
}

/*********************************************
            MAIN
*********************************************/
int main(int argc, char const *argv[])
{
    int i=0;
    int commands_run = 0; //position on each string
    char last[100]; //last command
    int line_len = 0;

    char ** history = (char**) malloc(MAX_HISTORY_SIZE * sizeof(char*));
    for(i=0; i<MAX_HISTORY_SIZE; i++) {
            history[i] = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));
            // history[i] = "\0";
            strcpy(history[i], "\0");
            // strcpy(history[i], "aa");
            // history[i][0] = '\0';
        }
    // set terminal color to magenta
    printColor("magenta");
    printf("Terminal started\n");
    // reset terminal color to white
    printColor("white");

    size_t bufsize = MAX_LENGTH_STRING;


    // Run terminal infinitely (until exit)
    while(1) {

        // Allocate memory for variables    
        char * line = (char *) malloc(MAX_LENGTH_STRING * sizeof(char));

      

        // set yellow color in terminal
        printColor("yellow");

        // print terminal specific symbol
        printf("@");


        //read line from terminal
        getline(&line, &bufsize, stdin);
        // printf("%s\n", line);
        // 
         
        if(!strcmp(line, "\n") || strspn(line, " \n") == strlen(line)) {
            continue;
        }

        //reset color to white
        printColor("white");

        if( !(line[0] == '!') 
        &&  !(line[0] == 'h' && line[1] == 's') ) {
            commands_run = save_history(history, line, commands_run);
        }

        runCommandFromLine(line, history, &commands_run);

        // Cleanup
        free(line);
    }

    free(history);

    return 0;
}