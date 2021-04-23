#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

typedef void (*sighandler_t)(int);
typedef handler_t;
void dieWithError(char *);

#define LIMIT 20
#define PERIOD 5

/* global variables for communication between handlers and main */
int sigalarm_count = 0;
int sigint_count = 0;

/* YOUR CODE HERE - Signal handler for SIGALRM */
void sigalrm_handler(int signum) {
  sigset_t mask, prev_mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  //block SIGALRM and save previous blocked set
  sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  sigalarm_count++;
  alarm(PERIOD);
  //restore previously blocked set, unblocking SIGALRM
  sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}

/* YOUR CODE HERE - Signal handler for SIGINT */
void sigint_handler(int signum) {
  /*
  sigset_t mask_all, prev_all;
  sigfillset(&mask_all);
  sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
  sigint_count++;
  sigprocmask(SIG_SETMASK, &prev_all, NULL);
  */
  sigset_t mask, prev_mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  //block SIGALRM and save previous blocked set
  sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  sigint_count++;
  //restore previously blocked set, unblocking SIGALRM
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

int main() {
  /* YOUR CODE HERE - the textbook and slides have examples
   * of how to install signal handlers, block signals, etc.
   */
  Signal(SIGALRM, sigalrm_handler);
  Signal(SIGINT, sigint_handler);
  pid_t pid = fork();
  if (pid == 0) {
    //char c;
    pid_t pid_parent = getppid();
    printf("Input number: ");
    getchar();
   // sigprocmask(SIG_SETMASK, &prev, NULL);
    for (int i = 0; i < LIMIT; i++) {
      kill(pid_parent, SIGINT);
    }
    printf("Child: finished sending SIGINT LIMIT times.");
    printf("\n");
    exit(0);
  }
  else {
    sigset_t mask, prev;
    //sigemptyset(&mask);
    //sigaddset(&mask, SIGALRM);
    //sigaddset(&mask, SIGINT);
    sigfillset(&mask);
    
    alarm(PERIOD);
    sigprocmask(SIG_BLOCK, &mask, &prev);
    while (sigint_count < LIMIT && sigalarm_count < LIMIT) {
        printf(".");
        sigsuspend(&prev);
    }
    printf("\n");
    sigprocmask(SIG_SETMASK, &prev, NULL);
    printf("%d\n", sigalarm_count);
    printf(" ");
    printf("%d\n", sigint_count);
    exit(0);
  }
}
