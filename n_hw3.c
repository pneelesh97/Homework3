#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include<sys/wait.h>


/*
  Neelesh Palaparthi
  palaparn 
  Homework 3
  compile command:<gcc -Wall -o n_hw3.c n_hw3>
  run command:<./n_hw3 -s 1024 -e "ls -l">
         :<./n_hw3 -f jpg -E "tar cvf jpg.tar">
         :<./n_hw3 -s 1024 -e "wc -l"
*/

typedef struct
{
    int S_flag;           // is the S flag provided?
    int s_flag;           // is the s flag provided?
    int f_flag;           // is the f flag provided?
    int t_flag;           // is the t flag provided?
    int fileSize;         // s flag value
    char filterTerm[300]; // f flag value
    char fileType[2];     // t flag value
    // initialization e and E flags
    int e_flag;           
    int E_flag;           
    char e_ucommand[300]; 
    char E_ucommands[300];

} FlagArgs;

// Global variable initilization
char gpath_e[800][800];
int Index_e = 0;

// type definition of the function pointer. It's void because it won't return anything
typedef void FileHandler(char *filePath, char *dirfile, FlagArgs flagArgs, int nestingCount);

// the function that will be used for this assignment
void myPrinterFunction(char *filePath, char *dirfile, FlagArgs flagArgs, int nestingCount)
{
    struct stat buf;       // buffer for data about file
    lstat(filePath, &buf); // very important that you pass the file path, not just file name
    char line[100];        // init some memory for the line that will be printed
    strcpy(line, "");      // verify a clean start
    strcat(line, dirfile); // init the line with the file name

    if (flagArgs.S_flag) // S case
    {
        char strsize[10];                          // allocate memory for the string format of the size
        sprintf(strsize, " %d", (int)buf.st_size); // assign the size to the allocated string
        strcat(line, strsize);                     // concatenate the line and the size
    }
    if (flagArgs.s_flag) // s case
    {
        if (flagArgs.fileSize > (int)buf.st_size) 
        {
            strcpy(line, ""); // clear line print
        }
    }
    if (flagArgs.f_flag) // f case
    {
        if (strstr(dirfile, flagArgs.filterTerm) == NULL) // if the filter does not appear in the file
        {
            strcpy(line, ""); // clear the line print
        }
    }
    if (flagArgs.t_flag) // t case
    {
        if (strcmp(flagArgs.fileType, "f") == 0) // if the provided t flag is "f"
        {
            if (S_ISDIR(buf.st_mode) != 0) // if the file is a dir
            {
                strcpy(line, ""); // clear the line print
            }
        }
        if (strcmp(flagArgs.fileType, "d") == 0) // if the provided t flag is "d"
        {
            if (S_ISREG(buf.st_mode) != 0) // if the file is a regular file
            {
                strcpy(line, ""); // clear the line print
            }
        }
    }

    if (strcmp(line, "") != 0) // check to prevent printing empty lines
    {
        int i = 0;
        for (i = 0; i <= nestingCount; i++) // tab printer
        {
            printf("\t"); // print a tab for every nesting
        }
        printf("line-%s\n", line); // print the line after the tabs

        if (flagArgs.e_flag == 1) {
            int status, i = 0;
            char *token = strtok(flagArgs.e_ucommand, " ");
            char** argument_list = (char**) malloc(500); 

            while( token != NULL ) {     
                argument_list[i] = token;   
                i++;
                token = strtok(NULL, " ");
            }
            
            strcpy(gpath_e[Index_e++], filePath);
            
            *(argument_list + i) = filePath;
            *(argument_list + i + 1) = NULL;

            // Fork creation
            pid_t id = fork();
    
            if (id == 0) {
                execvp(*argument_list + 0, argument_list);
                perror("exec");
                free(argument_list);
                exit(-1);
            } else if (id > 0){
                wait(&status);
                int exitStatus = WIFEXITED(status);
                free(argument_list);                
                if (exitStatus != 1)
                    printf("Child process exited incorrectly\n");
            } else {
                perror("fork"); 
                free(argument_list);
                exit(EXIT_FAILURE);
            }
        }
    }
}

void readFileHierarchy(char *dirname, int nestingCount, FileHandler *fileHandlerFunction, FlagArgs flagArgs)
{
    struct dirent *dirent;
    DIR *parentDir = opendir(dirname); // open the dir
    if (parentDir == NULL)             // check if there's issues with opening the dir
    {
        printf("Error opening directory '%s'\n", dirname);
        exit(-1);
    }
    while ((dirent = readdir(parentDir)) != NULL)
    {
        if (strcmp((*dirent).d_name, "..") != 0 &&
            strcmp((*dirent).d_name, ".") != 0) // ignore . and ..
        {
            char pathToFile[300];                                                      // init variable of the path to the current file
            sprintf(pathToFile, "%s/%s", dirname, ((*dirent).d_name));                 // set above variable to be the path
            fileHandlerFunction(pathToFile, (*dirent).d_name, flagArgs, nestingCount); // function pointer call
            if ((*dirent).d_type == DT_DIR)                                            // if the file is a dir
            {
                nestingCount++;                                                             // increase nesting before going in
                readFileHierarchy(pathToFile, nestingCount, fileHandlerFunction, flagArgs); // reccursive call
                nestingCount--;                                                             // decrease nesting once we're back
            }
        }
    }
    closedir(parentDir); // make sure to close the dir
}

// for E commands
void readFileHierarchy2(char *dirname, int nestingCount, FileHandler *fileHandlerFunction, FlagArgs flagArgs, char** argv)
{
    struct dirent *dirent;
    DIR *parentDir = opendir(dirname); // open the dir
    if (parentDir == NULL)             // check if there's issues with opening the dir
    {
        printf("Error opening directory '%s'\n", dirname);
        exit(-1);
    }
    char* keyword = argv[2];
    
    int status, i = 0;
    char** argument_list = (char**) malloc(2000);
    char *token = strtok(flagArgs.E_ucommands, " ");

    while( token != NULL ) {     
        argument_list[i] = token;   
        i++;
        token = strtok(NULL, " ");
    }

    while ((dirent = readdir(parentDir)) != NULL)
    {
        if (strcmp((*dirent).d_name, "..") != 0 &&
            strcmp((*dirent).d_name, ".") != 0) // ignore . and ..
        {                                                      // init variable of the path to the current file  
            if (strstr((*dirent).d_name, keyword)) {
                argument_list[i++] = (*dirent).d_name;
            }
        }
    }


    argument_list[i+1] = NULL;

    pid_t pid = fork();

    if (pid == 0) {
        printf("\nf - %d \n", pid);
        execvp(argument_list[0], argument_list);
        perror("exec");
        free(argument_list);
        exit(-1);
    } else if (pid > 0){
        wait(&status);
        int exitStatus = WIFEXITED(status);
        free(argument_list);                
        if (exitStatus != 1)
            printf("Child process exited incorrectly\n");
    } else {
        perror("fork"); 
        free(argument_list);
        exit(EXIT_FAILURE);
    }


    closedir(parentDir); // make sure to close the dir
}

int main(int argc, char **argv)
{
    // init opt :
    int opt = 0;
    // init a flag struct with 0s
    FlagArgs flagArgs = {
        .S_flag = 0,
        .s_flag = 0,
        .f_flag = 0,
        .t_flag = 0,
        .e_flag = 0,
        .E_flag = 0
        };

    // Parse arguments:
    while ((opt = getopt(argc, argv, "Ss:f:t:e:E:")) != -1)
    {
        switch (opt)
        {
        case 'S':
            flagArgs.S_flag = 1; // set the S_flag to a truthy value
            break;

        case 's':
            flagArgs.s_flag = 1;              // set the s_flag to a truthy value
            flagArgs.fileSize = atoi(optarg); // set fileSize to what was provided
            break;

        case 'f':
            flagArgs.f_flag = 1;                 // set the f_flag to a truthy value
            strcpy(flagArgs.filterTerm, optarg); // set filterTerm to what was provided
            break;

        case 't':
            flagArgs.t_flag = 1;               // set the t_flag to a truthy value
            strcpy(flagArgs.fileType, optarg); // set fileType to what was provided
            break;

        case 'e':
            flagArgs.e_flag = 1;
            strcpy(flagArgs.e_ucommand, optarg);
            break;

        case 'E':
            flagArgs.E_flag = 1;
            strcpy(flagArgs.E_ucommands, optarg);
            break;
        }
    }

    if (opendir(argv[argc - 1]) == NULL) // check for if a dir is provided
    {
        char defaultdrive[300];
        getcwd(defaultdrive, 300);    // get the current working directory (if no directory was provided)
        printf("%s\n", defaultdrive); // prints the top-level dir
        if (flagArgs.E_flag == 1) readFileHierarchy2(defaultdrive, 0, myPrinterFunction, flagArgs, argv);
        else readFileHierarchy(defaultdrive, 0, myPrinterFunction, flagArgs);
        return 0;
    }
    printf("%s\n", argv[argc - 1]); // prints the top-level dir
    if (flagArgs.E_flag == 1) readFileHierarchy2(argv[argc - 1], 0, myPrinterFunction, flagArgs, argv);
    else readFileHierarchy(argv[argc - 1], 0, myPrinterFunction, flagArgs);
    return 0;
}

// Reference:
// https://percona.community/blog/2021/01/04/fork-exec-wait-and-exit/
// https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
