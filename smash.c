//Kevin Monahan
//CompSci 537 - Homework 3

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define stdin stdin
#define stdout stdout
#define SPACETOKENS "\t\n \0"
#define MULTIPLECMDS ";"
#define PARALLELCMDS "&"
#define REDIRECTION ">"
#define MAXARGS 10
typedef struct pathList Node;

//A struct used to define the list of paths kept by the shell
struct pathList
{
    char *path;
    struct pathList *next;
};

struct pathList *pathHead;

//Empties the path list safely freeing space allocated to the struct itself and the char* within
void clearPath()
{
    Node *n = pathHead;
    Node *temp = 0;
    while (n != NULL)
    {
        temp = n->next;
        free(n->path);
        free(n);
        n = temp;
    }
    pathHead = NULL;
}

//Standard error message printed when an error occurs
void runError()
{
    char error_message[30] = "An error has occured\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

//Takes in a string and returns a string that has removed all whitespace before and after it removed
char *trimWhiteSpace(char *input){
    char *endOfStr;
    if(input == NULL)return NULL;
    while(isspace((unsigned char)*input)) input++;

    if(*input == 0){ //Only whitespace
        return input;
    }

    endOfStr = input + strlen(input) - 1;
    while(endOfStr > input && isspace((unsigned char)*endOfStr))endOfStr--;
    if(isspace(endOfStr[0])) endOfStr[0] = '\0';
    else endOfStr[1] = '\0';
    

    return input;
}

//Takes in a line from the shell and checks for syntactical errors
int parser(char *input)
{
    int fError = 0;
    while (input != NULL){
        char *rmMul = trimWhiteSpace(strsep(&input, MULTIPLECMDS));
        if (rmMul[0] == '\0'){
            break;
        }
        char *regCmd = malloc(strlen(rmMul) + 1);
        strcpy(regCmd, rmMul);
        char *builtCmd = strsep(&rmMul, SPACETOKENS);
        if (strcmp(builtCmd, "cd") == 0)
        {
            if (rmMul[0] == '\0')
            {
                runError();
                fError++;
            }
            else
            {
                char *temp = trimWhiteSpace(rmMul);
                char *path = strsep(&temp, SPACETOKENS);
                if (path[0] == '\0')
                {
                    runError();
                    fError++;
                }
                else if (temp != NULL && trimWhiteSpace(temp) != NULL)
                {
                    runError();
                    fError++;
                }
            }
        }
        else if (strcmp(builtCmd, "path") == 0)
        {
            if (rmMul == NULL)
            {
                runError();
                fError++;
            }
            else
            {
                char *temp = trimWhiteSpace(rmMul);
                char *arg = strsep(&temp, SPACETOKENS);
                if (strcmp(arg, "remove") == 0)
                {
                    char *str = trimWhiteSpace(temp);
                    char *rmpath = strsep(&str, SPACETOKENS);
                    Node *temp = pathHead;
                    Node *prev = malloc(sizeof(Node));
                    prev->next = temp;
                    int rm = 0;
                    while (temp != NULL)
                    {
                        if (strcmp(temp->path, rmpath) == 0)
                        {
                            rm++;
                        }
                        prev = temp;
                        temp = temp->next;
                    }
                    if (rm == 0)
                    {
                        runError();
                        fError++;
                    }
                }
                else if ((strcmp(arg, "clear") != 0) == (strcmp(arg, "add") != 0))
                {
                    runError();
                    fError++;
                }
            }
        }
        else if (strcmp(builtCmd, "exit") == 0)
        {

            char *temp = trimWhiteSpace(rmMul);
            char *arg = strsep(&temp, SPACETOKENS);
            if (arg != NULL)
            {
                fError++;
                runError();
            }
        }
        else
        {
            while (regCmd != NULL)
            {
                char *parCmd = trimWhiteSpace(strsep(&regCmd, PARALLELCMDS));
                if (parCmd != NULL)
                {
                    Node *cur = pathHead;
                    int errF = 0;
                    char *cmdWithArgs = strsep(&parCmd, REDIRECTION);
                    if (parCmd != NULL)
                    {
                        if (cmdWithArgs[0] == '\0')
                        {
                            runError();
                            fError++;
                            errF = 1;
                        }
                        else
                        {
                            char *temp = trimWhiteSpace(parCmd);
                            strsep(&temp, SPACETOKENS);
                            if (temp != NULL)
                            {
                                char *testForAdditionalInfo = trimWhiteSpace(temp);
                                char *addTest = strsep(&testForAdditionalInfo, SPACETOKENS);
                                if (addTest != NULL)
                                {
                                    runError();
                                    fError++;
                                    errF = 1;
                                }
                            }
                        }
                    }
                    if (errF == 0)
                    {
                        char *cmd = strsep(&cmdWithArgs, SPACETOKENS);
                        char *args[MAXARGS];
                        for (int i = 0; i < MAXARGS; i++)
                            args[i] = NULL;
                        args[0] = cmd;
                        int p = 1;
                        while (cmdWithArgs != NULL)
                        {
                            if (cmdWithArgs[0] == '\0')
                                break;
                            if (p >= MAXARGS)
                                break;
                            cmdWithArgs = trimWhiteSpace(cmdWithArgs);
                            args[p] = strsep(&cmdWithArgs, SPACETOKENS);
                            if (args[p][0] == '\0')
                            {
                                args[p] = NULL;
                                break;
                            }
                            p++;
                        }
                        int found = 0;
                        while (cur != NULL)
                        {
                            char *parseFix = malloc(strlen(cur->path) + 1);
                            strcpy(parseFix, (cur->path));
                            char *fullPath = cur->path;
                            strcat(strcat(fullPath, "/"), cmd);
                            if (access(fullPath, X_OK) == 0)
                            {
                                found = 1;
                                strcpy((cur->path),parseFix);
                                break;
                            }
                            else
                            {
                                strcpy((cur->path),parseFix);
                                cur = cur->next;
                            }
                            free(parseFix);
                        }
                        if (found == 0)
                        {
                            fError++;
                            runError();
                        }
                    }
                }
            }
        }
    }
    if (fError != 0)
    {
        return (2);
    }
    return (0);
}

//Used to run non built in commands
void runRegCmd(char *input)
{
    Node *cur = pathHead;
    int stream = 0, creErr = 0;
    char *cmdWithArgs = strsep(&input, REDIRECTION);
    if (input != NULL)
    {
        char *temp = trimWhiteSpace(input);
        char *file = strsep(&temp, SPACETOKENS);
        stream = open(file, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
        if (stream == -1)
        {
            runError();
            creErr++;
        }

        dup2(stream, STDOUT_FILENO);
        dup2(stream, STDERR_FILENO);
        close(stream);
        free(temp);
    }
    if(creErr == 0){
        char *cmd = strsep(&cmdWithArgs, SPACETOKENS);
    char *args[MAXARGS];
    for (int i = 0; i < MAXARGS; i++)
        args[i] = NULL;
    args[0] = cmd;
    int p = 1;
    while (cmdWithArgs != NULL)
    {
        if (cmdWithArgs[0] == '\0')
            break;
        if (p >= MAXARGS)
            break;
        cmdWithArgs = trimWhiteSpace(cmdWithArgs);
        args[p] = strsep(&cmdWithArgs, SPACETOKENS);
        if (args[p][0] == '\0')
        {
            args[p] = NULL;
            break;
        }
        p++;
    }
    while (cur != NULL)
    {
        char *fullPath = cur->path;
        strcat(strcat(fullPath, "/"), cmd);
        if (access(fullPath, X_OK) == 0)
        {
            int check = execv(fullPath, args);
            if (check != 0)
            {
                runError();
            }
            break;
        }
        else
        {
            cur = cur->next;
        }
    }
    } 
}

//Runs built in commands and spilts up non built in commands which are forked to runRegCmd. Returns a value that is useful if exit is called
int runCmd(char *input)
{
    if (input == NULL || input[0] == '\0')
        return 1;
    char *regCmd = malloc(strlen(input) + 1);
    strcpy(regCmd, input);
    char *builtCmd = strsep(&input, SPACETOKENS);
    if (strcmp(builtCmd, "cd") == 0)
    {
        char *temp = trimWhiteSpace(input);
        char *path = strsep(&temp, SPACETOKENS);
        if (chdir(path) != 0)
        {
            runError();
        }
    }
    else if (strcmp(builtCmd, "path") == 0)
    {

        char *temp = trimWhiteSpace(input);
        char *arg = strsep(&temp, SPACETOKENS);
        if (strcmp(arg, "clear") == 0)
        {
            clearPath();
        }
        else if (strcmp(arg, "add") == 0)
        {
            Node *newNode = malloc(sizeof(Node));
            newNode->next = pathHead;
            char *str = trimWhiteSpace(temp);
            char *path = strsep(&str, SPACETOKENS);
            newNode->path = malloc(strlen(path) + 1);
            strcpy(newNode->path, path);
            pathHead = newNode;
        }
        else if (strcmp(arg, "remove") == 0)
        {
            char *str = trimWhiteSpace(temp);
            char *rmpath = strsep(&str, SPACETOKENS);
            Node *temp = pathHead;
            Node *prev = NULL;
            int rm = 0;
            while (temp != NULL)
            {
                if (strcmp(temp->path, rmpath) == 0)
                {
                    if (prev == NULL)
                    {
                        pathHead = pathHead->next;
                    }
                    else
                    {
                        prev->next = temp->next;
                    }
                    rm++;
                }
                prev = temp;
                temp = temp->next;
            }
            if (rm == 0)
            {
                runError();
            }
        }
        else
        {
            runError();
        }
    }
    else if (strcmp(builtCmd, "exit") == 0)
    {
        return (0);
    }
    else
    {
        pid_t wpid;
        int status;
        while (regCmd != NULL)
        {
            char *parCmd = trimWhiteSpace(strsep(&regCmd, PARALLELCMDS));
            if (fork() == 0)
            {
                runRegCmd(parCmd);
                exit(0);
            }
        }
        while ((wpid = wait(&status)) > 0);
    }
    return (1);
}

//Sends command to parser and then spilts up the command based on the ";" character. Returns a value that is useful if exit is called
int runMultiCmd(char *input)
{
    char *parseStr = malloc(strlen(input) + 1);
    strcpy(parseStr, input);
    if (parser(parseStr) != 0)
    {
        free(parseStr);
        return (1);
    }
    while (input != NULL)
    {
        char *temp = trimWhiteSpace(strsep(&input, MULTIPLECMDS));
        int e = runCmd(temp);
        if (e == 0){
            free(parseStr);
            return (0);
        }
    }
    free(parseStr);
    return (1);
}

int main(int argc, char *argv[])
{
    int f = 1;
    FILE *input;
    char *bufferInput;

    size_t bufsize = 100;
    if (argc > 2)
    {
        exit(1);
    }
    else
    {
        if (argc > 1)
        {
            input = fopen(argv[1], "r");
            if (input == NULL)
            {
                exit(1);
            }
            f = 0;
        }
        else
            input = stdin;

        bufferInput = (char *)malloc(bufsize * sizeof(char));

        pathHead = malloc(sizeof(Node));
        pathHead->next = NULL;
        pathHead->path = malloc(strlen("/bin") + 1);
        strcpy(pathHead->path, "/bin");

        if (f == 0)
        { // Batch mode
            while (getline(&bufferInput, &bufsize, input) != -1)
                runMultiCmd(bufferInput);
        }
        while (f == 1)
        { // Regular Mode
            printf("smash> ");
            getline(&bufferInput, &bufsize, input);
            f = runMultiCmd(bufferInput);
        }

        fclose(input);
        clearPath();
        free(bufferInput);
        exit(0);
    }
}