/*
    Programmer   : Sunil Sutar
    Subject      : Linux System Programming
    Mini Project : Linux Shell Utility
    
  - main() displays a prompt, receives string from keyboard pass it to tokenize().
  - tokenize() allocates dynamic memory and tokenize string and return char**.
  - main() then pass tokenized string to execute() which calls fork() and exec().
  - Finnaly, main() again displays prompt and wait for next command string.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN     512
#define MAXARGS     10
#define ARGLEN      30
#define PROMPT      "MYSHELL:- "

int execute(char* arglist[]);
char* read_cmd(char *, FILE *);
char** tokenize(char* cmdline);

int main()
{
    char* cmdline;
    char** arglist;
    char* prompt = PROMPT;
    
    while((cmdline = read_cmd(prompt, stdin)) != NULL)
    {
        if((arglist = tokenize(cmdline)) != NULL)
        {
            execute(arglist);

            //Need to free arglist
            for(int j=0; j < MAXARGS+1; j++)
                free(arglist[j]);
            free(arglist);
            free(cmdline);
        }

    }//end of while loop
    
    printf("\n");
    
    return 0;
}

int execute(char* arglist[])
{
    int status;
    int cpid = fork();
    
    switch(cpid)
    {
        case -1:
            perror("fork() failed\n");
            exit(1);
        case 0:
            execvp(arglist[0], arglist);
            perror("Command not found..");
            exit(1);
        default:
            waitpid(cpid, &status, 0);
            printf("Child Exited with Status = %d\n", status >>8);
            return 0;
    }

}

//This function gets input char by char and returns string
char* read_cmd(char *prompt,  FILE *fp)
{
    int c;
    int pos = 0;
    
    printf("%s", prompt);

    char *cmdline = (char*) malloc(sizeof(char) * MAX_LEN);
   
    while((c = getc(fp)) != EOF)
    {
        if(c == '\n')
            break;
        cmdline[pos++] = c;
    }

    //This if condition is added in case user press Cltr+D to exit the shell
    if(c == EOF && pos == 0)
        return NULL;

    cmdline[pos] = '\0';
    
    return cmdline;
}


char** tokenize(char *cmdline)
{
    char** arglist = (char**) malloc(sizeof(char *) * (MAXARGS + 1));
    
    for(int i=0; i<MAXARGS; i++)
    {
        arglist[i] = (char *)malloc(sizeof(char*) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }
    
    char *cp = cmdline;
    char *start;
    int len;
    int argnum=0;

    while(*cp != '\0')
    {
        while(*cp == ' ' || *cp == '\t')
            cp++;

        start = cp;
        len = 1;

        //Find end of the word 
        while(*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
            len++;

        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    arglist[argnum] = NULL;

    return arglist;
}

