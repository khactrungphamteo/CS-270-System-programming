#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
//using namespace std;

#define MAXINPUT 256
#define SIZE 1000

typedef handler_t;
struct pair {
    char varName[MAXINPUT];
    char value[MAXINPUT];
};

void dieWithError(char *s) {
  fputs(s,stderr);
  exit(1);
}

void sigint_handler(int signum) {
  sigset_t mask, prev_mask;
  sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  write(STDOUT_FILENO, "program will not be terminated!\n", 13);
  sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}

handler_t* Signal(int signum, handler_t* handler) {
  struct sigaction action, old_action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask); //block sigs of type being handled
  action.sa_flags = SA_RESTART; //restart syscalls if possible

  if (sigaction(signum, &action, &old_action) < 0) {
    //unix_error("Signal error");
    return (old_action.sa_handler);
  }
}

char** scanner(char* inputLine) {
    int i = 0;
    char** outcome = calloc(SIZE, sizeof(char*)); 
    char* delim;
    delim = " \t\n";
    //char* token = strsep(&inputLine, delim);
    char fileName[MAXINPUT] = " ";
    /*
    while (token != NULL) {
        if (strstr(&token, "\"") != NULL) {
            strcat(fileName, token);
            //printf("Filename: %s\n", fileName);
            strcat(fileName, " ");
            //printf("Filename: %s\n", fileName);
            //printf("%s\n", fileName);
            token = strsep(NULL, delim);
        }
        else {
        
            outcome[i] = token;
            token = strsep(NULL, delim);
            i += 1;
        }
    }
    
    char* substring;
    substring = strtok(fileName, "\"");
    substring = strtok(NULL, "\"");
    outcome[i] = substring;
    */
    char* token;
    while ((token = strsep(&inputLine, delim)) != NULL) {
        if (strstr(token, "\"") != NULL) {
            strcat(fileName, token);
            strcat(fileName, " ");
        }
        else {
            outcome[i] = token;
            i += 1;
        }
    }

    char* substring;
    substring = strtok(fileName, "\"");
    substring = strtok(NULL, "\"");
    outcome[i] = substring;
    
    return outcome;
}

int main(int argc, char* argv[]) {
    Signal(SIGINT, sigint_handler);
    int status;
    char* CWD = getcwd(CWD, MAXINPUT);
    char* PATH;
    PATH = "/user/bin";
    char *PS = "CS270";
    
    //char variable[MAXINPUT];
    //char value[MAXINPUT];
    struct pair table[SIZE];
    char inputLine[MAXINPUT];
    char** parse;
    char** en_argv = calloc(SIZE, sizeof(char*));
    char** envp = calloc(SIZE, sizeof(char*));
    int size = 0;
    int counter = 0;
    //int arr_index = 0;
    int envp_index = 0;

    strcpy(table[counter].varName, "$PATH");
    strcpy(table[counter].value, PATH);
    counter += 1;
    strcpy(table[counter].varName, "$CWD");
    strcpy(table[counter].value, CWD);
    counter += 1;
    strcpy(table[counter].varName, "$PS");
    strcpy(table[counter].value, PS);
    counter += 1;
    
    char string2[MAXINPUT] = " ";
    strcat(string2, "CWD");
    strcat(string2, "=");
    strcat(string2, CWD);
    envp[0] = string2;

    char string3[MAXINPUT] = " ";
    strcat(string3, "PATH");
    strcat(string3, "=");
    strcat(string3, PATH);
    envp[1] = string3;

    char string1[MAXINPUT] = " ";
    strcat(string1, "PS");
    strcat(string1, "=");
    strcat(string1, PS);
    envp[2] = string1;
    envp[3] = "\0";

    while (1) {
        //PS = getcwd(CWD, MAXINPUT);
        //strcpy(table[2].value, PS);
        printf("%s>>", PS);
        //fgets(inputLine, MAXINPUT, stdin);
        //fputs(inputLine, stdout);
        if (fgets(inputLine, MAXINPUT, stdin) == NULL) {
            dieWithError("Program terminated!\n");
        }
        
        parse = scanner(inputLine);
        
        while (parse[size] != NULL) {
            printf("%s\n", parse[size]);
            size += 1;
        }
        
        if (strcmp(parse[0], "#") == 0) {
            // do nothing
        }
        else if (strcmp(parse[0], "cd") == 0) {
            //printf("current working directory: %s\n", CWD);
            if (chdir(parse[1]) != 0) {
                printf("cd error!\n");
            }
            else {
                memset(CWD, 0, sizeof(CWD));
                strcpy(CWD, parse[1]);
                strcpy(table[1].value, CWD);
            }
        }
        else if (strcmp(parse[1], "=") == 0) {
            int i = 0;
           int counter = 0;
           for (i = 0; parse[0][i] != '\0'; i++) {
               
           }

            if (counter == i) {
                if (strcmp(parse[0], "PS") == 0) {
                    PS = parse[2];
                }
                else if (strcmp(parse[0], "PATH") == 0) {
                    PATH = parse[2];
                }
            
            int counter_1 = 0;
            char** tokens = calloc(SIZE, sizeof(char*));
            char *token;
            int index_1, index_2;
            char newValue[MAXINPUT] = " ";
            if (strstr(parse[2], "$") != NULL) {
                if (strstr(parse[2], ":") != NULL) {
                    token = strtok(parse[2], ":");
                    while (token != NULL) {
                        tokens[counter_1] = token;
                        token = strtok(NULL, ";");
                        printf("%s\n", tokens[counter_1]);
                        counter_1 += 1;
                    }

                    for (int i = 0; i < counter; i++) {
                        if (strcmp(table[i].varName, tokens[0]) == 0) {
                            strcpy(newValue, table[i].value);
                            strcat(newValue, ":");
                        }
                        else if (strcmp(table[i].varName, tokens[1]) == 0) {
                            strcat(newValue, table[i].value);
                        }
                    }
                    //printf("%s\n", newValue);
                    strcpy(PATH, newValue);
                }
            }

            char variable[MAXINPUT];
            int index;
            int flag = 1;
            strcpy(variable, "$");
            strcat(variable, parse[0]);
            for (int i = 0; i < counter; i++) {
                if (strcmp(table[i].varName, variable) == 0) {
                    flag = 0;
                    index = i;
                }
            }

            if (flag == 1) {
                strcpy(table[counter].varName, variable);
                strcpy(table[counter].value, parse[2]);
                counter++;
            }
            else {
                strcpy(table[index].varName, variable);
                strcpy(table[index].value, parse[2]);
            }
            }
            else {
                printf("invalid variable\n");
            }

            
        }
        else if (strcmp(parse[0], "!") == 0) {
            //char* path;
            //path = getcwd(CWD, MAXINPUT);
            int flag = 0;
            //fflush(0);
            if (fork() == 0) {
                if (strstr(parse[1], "/") != NULL) {
                    en_argv[0] = parse[1];
                    for (int i = 2; parse[i] != NULL; i++) {
                        if (strcmp(parse[i], "infrom:") == 0) {
                            int fd1 = open(parse[i + 1], O_CREAT|O_RDONLY, 0);
                            dup2(fd1, 0);
                            close(fd1);
                        }
                        else if (strcmp(parse[i], "outto:") == 0) {
                            int fd2 = open(parse[i + 1], O_CREAT|O_TRUNC|O_WRONLY, 0);
                            dup2(fd2, 1);
                            close(fd2);
                        }
                        else {
                            en_argv[i - 1] = parse[i];
                        }
                    }
                    execve(parse[1], en_argv, envp);
                }
                else if(strstr(parse[1], "./") != NULL) { //check error to see if parse[1] is ../
                    
                    char* currDr;
                    currDr = getcwd(currDr, MAXINPUT);
                    //printf("current Directory: %s\n", currDr);
                    //fputs(currDr, stdout);
                    
                    char* token; 
                    token = strtok(parse[1], "./");
                    token = strtok(NULL, "./");
                    
                    strcat(currDr, "/");
                    strcat(currDr, token);
                    en_argv[0] = currDr;
                    for (int i = 2; parse[i] != NULL; i++) {
                        if (strcmp(parse[i], "infrom:") == 0) {
                            int fd1 = open(parse[i + 1], O_CREAT|O_RDONLY, 0);
                            dup2(fd1, 0);
                            close(fd1);
                        }
                        else if (strcmp(parse[i], "outto:") == 0) {
                            int fd2 = open(parse[i + 1], O_CREAT|O_TRUNC|O_WRONLY, 0);
                            dup2(fd2, 1);
                            close(fd2);
                        }
                        else {
                            int flag = 1;
                            int index; 
                            if (strstr(parse[2], "$" != NULL)) {
                                for (int j = 0; j < counter; i++) {
                                    if (strcmp(table[j].varName, parse[i]) == 0) {
                                        flag = 0;
                                        index = j;
                                    }
                                }
                            }

                            if (flag != 1) {
                                parse[i] = table[index].value;
                            }
                            else {
                                dieWithError("error!\n");
                            }
                            en_argv[i - 1] = parse[i];
                        }
                    }
                    execve(parse[1], en_argv, envp);
                }
                else {
                    PATH = CWD;
                    en_argv[0] = parse[1];
                    DIR* streamp;
                    struct dirent *dep;
                    streamp = opendir(PATH);
                    errno = 0;
                    while ((dep = readdir(streamp)) != NULL) {
                        if (strcmp(dep->d_name, parse[1]) == 0){
                            flag = 1;
                        }
                    }
                    
                    if (flag != 1) {
                        dieWithError("fileName not found\n");
                    }
                    else {
                        int flag = 1;
                        for (int i = 2; parse[i] != NULL; i++) {
                            if (strcmp(parse[i], "infrom:") == 0) {
                                int fd1 = open(parse[i + 1], O_CREAT|O_RDONLY, 0);
                                dup2(fd1, 0);
                                close(fd1);
                            }
                            else if (strcmp(parse[i], "outto:") == 0) {
                                int fd2 = open(parse[i + 1], O_CREAT|O_TRUNC|O_WRONLY, 0);
                                dup2(fd2, 1);
                                close(fd2);
                            }
                            else {
                                
                                int index = 0;
                                if (strstr(parse[i], "$") != NULL) {
                                    for (int j = 0; j < counter; j++) {
                                        if (strcmp(parse[i], table[j].varName) == 0) {
                                            flag = 0;
                                            index = j;
                                        }
                                        else {
                                            //dieWithError("Error!\n");
                                        }
                                    }

                                    if (flag == 0) {
                                        parse[i] = table[index].value;
                                        en_argv[i] = parse[i];
                                        //flag = 1;
                                    }
                                    en_argv[i - 1] = parse[i];

                                }
                            }
                        }

                        if (flag == 1) {
                            dieWithError("variable not found!\n");
                        }
                        else {
                            execve(parse[1], en_argv, envp);
                        }
                    }
                    exit(0);
                }
            }
            wait(&status);
                /*
                int fd1 = open(parse[3], O_CREAT|O_RDONLY, 0);
                dup2(fd1, 0);
                close(fd1);
                DIR* streamp;
                struct dirent *dep;
                streamp = opendir(PATH);
                errno = 0;
                while ((dep = readdir(streamp)) != NULL) {
                    if (strcmp(dep->d_name, parse[1]) == 0){
                        flag = 1;
                        en_argv[0] = dep->d_name;
                    }
                }
                en_argv[1] = "\0";
                if (errno != 0) {
                    dieWithError("readdir error");
                }

                int fd2 = open(parse[5], O_CREAT|O_WRONLY, 0);
                dup2(fd2, 1);
                close(fd2);

                if (flag != 1) {
                    dieWithError("fileName not found");
                }
                else {
                    execve(en_argv[0], en_argv, envp);
                }
                exit(0);
            }
            */
        }
        else if (strcmp(parse[0], "lv") == 0) {
            for (int i = 0; i < counter; i++) {
                printf("variable: %s\n", table[i].varName);
                printf("value: %s\n", table[i].value);
            }
        }
        else if (strcmp(parse[0], "unset") == 0) {
            int i;
            char variable[MAXINPUT];
            strcpy(variable, "$");
            strcat(variable, parse[1]);
            if (strcmp(parse[1], "PATH") == 0) {
                dieWithError("error!\n");
            }
            else if (strcmp(parse[1], "CWD") == 0) {
                dieWithError("error!\n");
            }
            else if (strcmp(parse[1], "PS") == 0) {
                dieWithError("error!\n");
            }
            
            for (i = 0; i < counter; i++) {
                if (strcmp(table[i].varName, variable) == 0) {
                    //memset(table[i].value, 0, sizeof(table[i].value));
                    break;
                }
            }

            if (i < counter) {
                // remove the size of the array
                counter = counter - 1;
                for (int j = i; j < counter; j++) {
                    table[j] = table[j+1];
                }
            }


        }
        else if (strcmp(parse[0], "quit") == 0) {
            exit(0);
        }
        else {
            dieWithError("command not found!\n");
        }

        char string2[MAXINPUT] = " ";
        strcat(string2, "CWD");
        strcat(string2, "=");
        strcat(string2, CWD);
        envp[0] = string2;

        char string3[MAXINPUT] = " ";
        strcat(string3, "PATH");
        strcat(string3, "=");
        strcat(string3, PATH);
        envp[1] = string3;

        //printf("%s\n", envp[0]);
        //printf("%s\n", envp[1]);
        //printf("%s\n", envp[2]);
    }
    exit(0);
}