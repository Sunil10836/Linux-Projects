/*
    Programmer   : Sunil Sutar
    Subject      : Linux System Programming
    Mini Project : Linux Shell Utility
    
  - main() displays a prompt, receives string from keyboard pass it to tokenize().
  - tokenize() allocates dynamic memory and tokenize string and return char**.
  - main() then pass tokenized string to execute() which calls fork() and exec().
  - Finaly, main() again displays prompt and wait for next command string.

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

void init_shell();
void printDir();

int execute(char* arglist[]);
char* read_cmd(char *, FILE *);
char** tokenize(char* cmdline);

int main()
{
    char* cmdline;
    char** arglist;
    char* prompt = PROMPT;

    init_shell();
    printDir();
   
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

void init_shell()
{
    printf("\n************ WELCOME TO MY SHELL *************\n");       
    char* username = getenv("USER");
    printf("\nUSER is = @%s", username);
    printf("\n");
}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s\n", cwd);
}

void openhelp()
{
    puts("\n***********Welcome to My Shell Help ********"
        "\nList of Commands Supported"
        "\n>ls   : List the contents of directory"
        "\n>exit : Exit"
        "\n>cd   : Change Directory" 
        "\n>pwd  : Print Working Directory"
        );

    return;
}

int execute(char* arglist[])
{
    int err, status, ret;

    if((strcmp(arglist[0], "exit")) == 0)      //Internal Command 1 : exit
        _exit(0);
    else if((strcmp(arglist[0], "cd")) == 0)   //Internal Command 2 : cd 
        chdir(arglist[1]);  
    else if(strcmp(arglist[0], "pwd") == 0)    //Internal Command 3 : pwd
        printDir();
    else if(strcmp(arglist[0], "help") == 0)   
        openhelp();
    else                                       //External Commands 
    {    
         ret = fork();
         
         switch(ret)
         {
            case -1:
                perror("fork() failed\n");
                exit(1);
            case 0:
                err = execvp(arglist[0], arglist);
                if(err < 0)
                {   
                    perror("Command not found..");
                    exit(1);
                }
            default:
                waitpid(ret, &status, 0);
                //printf("Child Exited with Status = %d\n", status >>8);
                return 0;
         }
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

