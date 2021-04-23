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

void sigusr1_handler(int signum) {
  sigset_t mask, prev_mask;
  //sigemptyset(&mask);
  //sigaddset(&mask, SIGUSR1);
  //block SIGALRM and save previous blocked set
  sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  sigFlag_1++;
  //alarm(PERIOD);
  //restore previously blocked set, unblocking SIGALRM
  sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}

/* YOUR CODE HERE - Signal handler for SIGINT */
void sigusr2_handler(int signum) {
  sigset_t mask, prev_mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR2);
  //block SIGALRM and save previous blocked set
  sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  sigFlag_2++;
  //restore previously blocked set, unblocking SIGALRM
  sigprocmask(SIG_SETMASK, &prev_mask, NULL);
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

handler_t* Signal(int signum, handler_t* handler) {
  struct sigaction action, old_action;
  action.sa_handler = handler;
  //sigemptyset(&action.sa_mask); //block sigs of type being handled
  action.sa_flags = SA_RESTART; //restart syscalls if possible

  if (sigaction(signum, &action, &old_action) < 0) {
    dieWithError("Error");
    return (old_action.sa_handler);
  }
}

pid_t Fork(void)
{
  pid_t pid;
  if ((pid = fork()) < 0)
    dieWithError("Fork error");
  return pid;
}


int main() {
  Signal(SIGUSR1, sigusr1_handler);
  Signal(SIGUSR2, sigusr2_handler);
  sigset_t unblockUSR1, unblockUSR2, maskAll, prev_mask;
  if(sigfillset(&maskAll) < 0) dieWithError("sigfillset Error");
  if(sigfillset(&unblockUSR1) < 0) dieWithError("sigfillset Error");
  if(sigdelset(&unblockUSR1, SIGUSR1) <0) dieWithError("sigdelset Error");
  if(sigfillset(&unblockUSR2) < 0) dieWithError("sigfillset Error");
  if(sigdelset(&unblockUSR2, SIGUSR2) <0) dieWithError("sigdelset Error");

  int child_status_1, child_status_2, child_status_3;
  pid_A = getpid();
  pid_B = pid_A + 1;
  pid_C = pid_B + 2;
  pid_D = pid_A + 2;

  //static pid_t pid_C_1;
  // base process A
  pid_1 = Fork();
  if (pid_1 == 0) { // in child proces B
    greet('B');
    //pid_B = getpid();
  }
  else {
    greet('A');
    //pid_A = getpid();
  }
  
  pid_2 = Fork();
  if (pid_2 == 0) { //in child process C or D
    if (getppid() == pid_B) {
      //in child process C
      //pid_C = getpid();
      greet('C');
      kill(getppid(), SIGUSR1);
      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
      while(sigFlag_2 == 0){
        sigsuspend(&unblockUSR2);
      }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_2 == 1) {
          sigFlag_2 = 0;
          goaway('C');
          kill(getppid(), SIGUSR1);
          exit(0);
      }
      
    }
    else if (getppid() == pid_A) {
      //in child process D
      //pid_D = getpid();
      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
      while(sigFlag_1 == 0){
        sigsuspend(&unblockUSR1);
      }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_1 == 1 ) {
        sigFlag_1 = 0;
        greet('D');
        kill(getppid(), SIGUSR2);
      }
      
      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
      while(sigFlag_1 == 0){
        sigsuspend(&unblockUSR1);
      }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_1 == 1) {
        sigFlag_1 = 0;
        goaway('D');
        kill(getppid(), SIGUSR2);
        exit(0);
      }
    }
  }
  else { // in parent process A or B
    
    if (getpid() == pid_B) {
      // in child process B

      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_1 == 0){
          sigsuspend(&unblockUSR1);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_1 == 1) {
        sigFlag_1 = 0;
        kill(getppid(), SIGUSR1);
      }

      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_2 == 0){
          sigsuspend(&unblockUSR2);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_2 == 1) {
        sigFlag_2 = 0;
        kill(pid_C, SIGUSR2);
      }

      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_1 == 0){
          sigsuspend(&unblockUSR1);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_1 == 1) {
        sigFlag_1 = 0;
        kill(getppid(), SIGUSR1);
      }
      if (sigFlag_1 == 1) {
        sigFlag_1 = 0;
      }
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
      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_1 == 0){
          sigsuspend(&unblockUSR1);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_1 == 1) {
        sigFlag_1 = 0;
        kill(pid_D, SIGUSR1);
      }

      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_2 == 0){
          sigsuspend(&unblockUSR2);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_2 == 1) {
        sigFlag_2 = 0;
        kill(pid_B, SIGUSR2);
      }

      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_1 == 0){
          sigsuspend(&unblockUSR1);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_1 == 1) {
        sigFlag_1 = 0;
        kill(pid_D, SIGUSR1);
      }
      if(sigprocmask(SIG_BLOCK, &maskAll, &prev_mask) < 0) dieWithError("sigprocmask Error");
        while(sigFlag_2 == 0){
          sigsuspend(&unblockUSR2);
        }

      if(sigprocmask(SIG_SETMASK,&prev_mask,NULL) < 0) dieWithError("sigprocmask Error");
      if (sigFlag_2 == 1) {
        sigFlag_2 = 0;
      }

      
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