#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include "UDPCookie.h"

typedef handler_t;

int sigalarm_count = 0;

void sigalrm_handler(int signum) {
  sigset_t mask, prev_mask;
  //sigemptyset(&mask);
  //sigaddset(&mask, SIGALRM);
  sigfillset(&mask);
  //block SIGALRM and save previous blocked set
  sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  sigalarm_count++;
  //alarm(PERIOD);
  //restore previously blocked set, unblocking SIGALRM
  sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}


handler_t Signal(int signum, handler_t handler) {
  struct sigaction action, old_action;
  action.sa_handler = handler;
  action.sa_flags = SA_RESTART; //restart syscalls if possible

  if (sigaction(signum, &action, &old_action) < 0) 
    dieWithError("Signal error");
    return (old_action.sa_handler);
}

int main(int argc, char*argv[]) {
  struct sigaction action, old_action;
  action.sa_handler = sigalrm_handler;
  sigemptyset(&action.sa_mask); //block sigs of type being handled
  action.sa_flags = 0; //restart syscalls if possible
  if (sigaction(SIGALRM, &action, &old_action) < 0) {
    dieWithError("Signal error");
  }
  //return (old_action.sa_handler);
  

  //Signal(SIGALRM, sigalrm_handler);
  char msgBuf[MAXMSGLEN];
  char buffer[MAXMSGLEN];
  header_t* msgptr = (header_t*) msgBuf;
  
  int msgLen = 0; // quiet the compiler
  //int recMsgLen = 0;
  int Flag = 0;

  if (argc != 3) // Test for correct number of arguments
    dieWithError("Parameter(s): <Server Address/Name> <Server Port/Service>");

  char *server = argv[1];     // First arg: server address/name
  char *servPort = argv[2];

  // Tell the system what kind(s) of address info we want
  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_INET;               // IPv4 only
  // For the following fields, a zero value means "don't care"
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
  addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP protocol

  // Get address(es)
  struct addrinfo *servAddr;      // List of server addresses
  int rtnVal = getaddrinfo(server, servPort, &addrCriteria, &servAddr);
  if (rtnVal != 0) {
    char error[MAXMSGLEN];
    snprintf(error,MAXMSGLEN,"getaddrinfo() failed: %s",gai_strerror(rtnVal));
    dieWithError(error);
  }
  /* Create a datagram socket */
  int sock = socket(servAddr->ai_family, servAddr->ai_socktype,
        servAddr->ai_protocol); // Socket descriptor for client
  if (sock < 0)
    dieWithSystemError("socket() failed");

  /* YOUR CODE HERE - construct Request message in msgBuf              */
  /* msgLen must contain the size (in bytes) of the Request msg        */
  msgptr->magic = 270;
  msgptr->xactionid = 5544332211;
  msgptr->flags = 0x22;
  msgptr->port = 0;
  msgptr->result = 0;
  char *varied_length = "tpng223";
  msgptr->length = sizeof(header_t) + sizeof(varied_length) - 1;
  msgLen = msgptr->length;
  //strncpy(&msgBuf[12], varied_length, 7);
  strncpy(msgBuf + sizeof(header_t), varied_length, sizeof(varied_length) - 1);
  msgBuf[msgLen] = '\0';

  printf("message sent to the server\n");
  printf("magic=%d \n",msgptr->magic);
  printf("length= %d\n", msgptr->length);
  printf("xid=%x %x %x %x\n", msgptr->xactionid >> 24, (msgptr->xactionid >> 16)&0xff, (msgptr->xactionid >> 8)&0xff, msgptr->xactionid & 0xff);
  printf("version=%d\n", (msgptr->flags) >> 4);
  printf("flags=0x%x \n",  ((msgptr->flags << 4) >> 4) & 0xf);
  printf("port=%d \n", msgptr->port);
  printf("result=%d \n",msgptr->result);
  printf("variable part=%s\n", varied_length);

  msgptr->port = htons(msgptr->port);
  msgptr->magic = htons(msgptr->magic);
  //msgptr->flags = htons(msgptr->flags);
  msgptr->length = htons(msgptr->length);
  ssize_t numBytes = sendto(sock, msgBuf, msgLen, 0, servAddr->ai_addr,
          servAddr->ai_addrlen);
  if (numBytes < 0)
    dieWithSystemError("sendto() failed");
  else if (numBytes != msgLen)
    dieWithError("sendto() returned unexpected number of bytes");

  /* YOUR CODE HERE - receive, parse and display response message */
  
  while ((sigalarm_count <= 5) && (Flag == 0)) {
    alarm(3);
    if (recvfrom(sock, buffer, msgLen, 0, NULL, NULL) < 0) {
      if (errno == EINTR) {
          numBytes = sendto(sock, msgBuf, msgLen, 0, servAddr->ai_addr, servAddr->ai_addrlen);
            if (numBytes < 0) {
              dieWithSystemError("sendto() failed");
            }
            else if (numBytes != msgLen){
              dieWithError("sendto() returned unexpected number of bytes");
            }
      }
      printf("message sent! \n");
    }
    else {
      alarm(0);
      Flag = 1;
      printf("message received!\n");
    }
  }
  
 /*
  while ((sigalarm_count <= 5) && (Flag == 0)) {
    alarm(3);
    if (recvfrom(sock, buffer, msgLen, 0, NULL, NULL) > 0) {
      alarm(0);
      Flag = 1;
      printf("message received!\n");
    }
    else {
      numBytes = sendto(sock, msgBuf, msgLen, 0, servAddr->ai_addr, servAddr->ai_addrlen);
            if (numBytes < 0) {
              dieWithSystemError("sendto() failed");
            }
            else if (numBytes != msgLen){
              dieWithError("sendto() returned unexpected number of bytes");
            }
      printf("message sent! \n");
    }
  }
  */

  header_t* msg = (header_t*) buffer;
  msg->port = ntohs(msg->port);
  msg->magic = ntohs(msg->magic);
  //msgptr->flags = ntohs(msgptr->flags);
  msg->length = ntohs(msg->length);
  msgLen = msg->length;

  //extract the varied-length part of the character array msgBuf
  char text[MAXMSGLEN];
  int text_length = msg->length - 12;
  memcpy(text, buffer + 12, text_length);
  text[text_length] = '\0';
  //memcpy(text + text_length, '\0', 1);
  
  printf("message received from the server\n");
  printf("magic=%d \n",msg->magic);
  printf("length= %d\n", msg->length);
  printf("xid=%x %x %x %x\n", msg->xactionid >> 24, (msg->xactionid >> 16)&0xff, (msg->xactionid >> 8)&0xff, msg->xactionid & 0xff);
  printf("version=%d\n", (msg->flags) >> 4);
  printf("flags=0x%x \n",  msg->flags & 0xf);
  printf("port=%d \n", msg->port);
  printf("result=%d \n",msg->result);
  printf("variable part=%s\n", text);

  freeaddrinfo(servAddr);
  close(sock);
  exit(0);

}