#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>

pid_t pid_A, pid_B, pid_C, pid_D, pid_2, pid_1;
static volatile sig_atomic_t sigFlag_1 = 0;
static volatile sig_atomic_t sigFlag_2 = 0;
typedef void (*sighandler_t)(int);
typedef handler_t;
void dieWithError(char *);

/* YOUR CODE HERE - Signal handler for SIGALRM */
char getName(pid_t pid) {
   char name;
  if (pid == pid_A) {
    name = 'A';
    return name;
  }
  else if (pid == pid_B) {
    name = 'B';
    return name;
  }
  else if (pid == pid_C) {
    name = 'C';
    return name;
  }
  else {
    name = 'D';
    return name;
  }
}

void greet(char myName) {
  printf("Process %c, PID %d greets you.\n",myName,getpid());
  fflush(stdout);
}

void goaway(char name){
  printf("Process %c exits normally\n", name);
  fflush(stdout);
  
}
void mourn(char parent, char child, int status){
  printf("Process %c here: Process %c exited with status 0x%x \n", parent, child, status);
  fflush(stdout);
}

pid_t Fork(void)
{
  pid_t pid;
  if ((pid = fork()) < 0)
    dieWithError("Fork error");
  return pid;
}


int main() {

  int child_status_1, child_status_2, child_status_3;
  
  pid_A = getpid();
  pid_B = pid_A + 1;
  pid_C = pid_B + 2;
  pid_D = pid_A + 2;
  
  // base process A

  pid_1 = Fork();
  if (pid_1 == 0) { // in child proces B
    greet('B');
    //pid_B = getpid();
  }
  else {
    greet('A');
    //pid_B = getpid();
  }
  
  pid_2 = Fork();
  if (pid_2 == 0) { //in child process C or D
    if (getppid() == pid_B) {
      //in child process C
      greet('C');
      goaway('C');
    }
    else if (getppid() == pid_A) {
      //in child process D
      //pid_D = getpid();
        greet('D');
        goaway('D');
        exit(0);
    }
    
  }
  else { // in parent process A or B
    
    if (getpid() == pid_B) {
      // in child process B
      pid_t child_pid = waitpid(-1, &child_status_1, 0);
        if (child_pid > 0) {
          if(WIFEXITED(child_status_1)){
            int exit_status = WEXITSTATUS(child_status_1);
            char child_name = getName(child_pid);
          char parent_name = getName(pid_B);
            mourn(parent_name, child_name, exit_status);
            goaway('B');
            exit(0);
            }
          }
        
    }
    
    if (getpid() == pid_A) {
      // in procces A 
        
        pid_t child_pid;
        while((child_pid = waitpid(-1, &child_status_1, 0)) > 0){
          if(WIFEXITED(child_status_1) > 0) {
            int exit_status = WEXITSTATUS(child_status_1);
            char child_name = getName(child_pid);
            char parent_name = getName(pid_A);
            mourn(parent_name, child_name, exit_status);
        }
      }
      goaway('A');
    }
  }
  
  return 0;
}

