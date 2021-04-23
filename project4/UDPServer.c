#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "UDPCookie.h"

#define ERRLEN 128

int isNotEqual(int x, int y) {
  return !!(x^y);
}

int main(int argc, char *argv[]) {
  int msgLen = 0;
  if (argc != 2) // Test for correct number of arguments
    dieWithError("Parameter(s): <Server Port #>");
  
  char *service = argv[1]; // First arg:  local port/service

  // Construct the server address structure
  struct addrinfo addrCriteria;                   // Criteria for address
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_INET;               // We want IPv4 only
  addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram socket
  addrCriteria.ai_protocol = IPPROTO_UDP;         // UDP socket

  struct addrinfo *servAddr; // List of server addresses
  int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
  if (rtnVal != 0) {
    char error[ERRLEN];
    if (snprintf(error,ERRLEN,"getaddrinfo() failed: %s",
		 gai_strerror(rtnVal)) < 0) // recursive?!
      dieWithSystemError("snprintf() failed");
    dieWithError(error);
  }

  // Create socket for incoming connections
  int sock = socket(servAddr->ai_family, servAddr->ai_socktype,
      servAddr->ai_protocol);
  if (sock < 0)
    dieWithSystemError("socket() failed");

  // Bind to the local address/port
  if (bind(sock, servAddr->ai_addr, servAddr->ai_addrlen) < 0)
    dieWithSystemError("bind() failed");

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  for (;;) { // Run forever
    struct sockaddr_storage clntAddr; // Client address
    // Set Length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    // Block until receive message from a client
    char buffer[MAXMSGLEN]; // I/O buffer
    
    // Size of received message
    ssize_t numBytesRcvd = recvfrom(sock, buffer, MAXMSGLEN, 0,
        (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (numBytesRcvd < 0)
      dieWithSystemError("recvfrom() failed");

    /* YOUR CODE HERE:  parse & display incoming request message */

    header_t* msg = (header_t*) buffer;
    msgLen = msg->length;
    char text[MAXMSGLEN];
    int text_length = msgLen - 12;
    memcpy(text, buffer + 12, text_length);
    text[text_length] = '\0';

    printf("message received from the client\n");
    printf("magic=%d \n",ntohs(msg->magic));
    printf("length= %d\n", ntohs(msg->length));
    printf("xid=%x %x %x %x\n", msg->xactionid >> 24, (msg->xactionid >> 16)&0xff, (msg->xactionid >> 8)&0xff, msg->xactionid & 0xff);
    printf("version=%d\n", msg->flags >> 4);
    printf("flags=0x%x \n",  msg->flags & 0xf);
    printf("port=%d \n", ntohs(msg->port));
    printf("result=%d \n",msg->result);
    printf("variable part=%s\n", text);

    /* YOUR CODE HERE:  construct Response message in buffer, display it */
    char msgBuf[MAXMSGLEN];
    header_t* a = (header_t*)msgBuf;
    a->magic = ntohs(msg->magic);
    a->xactionid = msg->xactionid;
    a->flags = msg->flags;
    a->port = ntohs(msg->port);
    a->result = msg->result;

    
    // adding cookie to the character array msgBuff to send back to the client
    char* cookie;
    if (a->flags == 0x22) {
      if (a->magic != 270) {
        a->flags = 0x21;
        cookie = "error";
        a->length = sizeof(header_t) + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));
      }
      else if (a->port != 0) {
        a->flags = 0x21;
        cookie = "error";
        a->length = sizeof(header_t) + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));
      }
      else if (ntohs(msg->length) != (12 + strlen(text))) {
        cookie = "error";
        a->flags = 0x21;
        a->length = sizeof(header_t) + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));
      }
      else if (ntohs(msg->length) == 12 + sizeof(text)) {
        //a->result -= 1; // Terminating null included in length
        a->flags = 0x21;
        cookie = "error";
        a->length = sizeof(header_t) + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));
      }
      else {
        char hoststr[NI_MAXHOST];
        char portstr[NI_MAXSERV];
        int rc = getnameinfo((struct sockaddr*)&clntAddr, clntAddrLen, hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST
        | NI_NUMERICSERV);
        if (rc == 0) printf("new connection from %s %s\n", hoststr, portstr);
        else dieWithError("getnameinfo() fail!\n");

        cookie = portstr;
        memcpy(cookie + strlen(cookie), hoststr, strlen(hoststr));
        a->length = 12 + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));
      }
  }
    else if (a->flags == 0x2a) {
      if (a->magic != 270) {
      a->flags = 0x29;
      //cookie = "bad magic!";
      //memcpy(&cookie[0], "bad magic!", sizeof("bad magic!"));
      cookie = "error";
      a->length = sizeof(header_t) + strlen(cookie);
      memcpy(msgBuf + 12, cookie, strlen(cookie));
      }
      else if (a->port != 0) {
        cookie = "error";
        a->flags = 0x29;
        a->length = sizeof(header_t) + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));

      }
      else if (ntohs(msg->length) != (12 + strlen(text))) {
        cookie = "error";
        a->flags = 0x29;
        a->length = sizeof(header_t) + strlen(cookie);
        memcpy(msgBuf + 12, cookie, strlen(cookie));
    }
    else if (ntohs(msg->length) == 12 + sizeof(text)) {
      //a->result -= 1; // Terminating null included in length
      a->flags = 0x29;
      cookie = "error";
      a->length = sizeof(header_t) + strlen(cookie);
      memcpy(msgBuf + 12, cookie, strlen(cookie));
    }
    else {
      cookie = text;
      a->length = sizeof(header_t) + strlen(cookie);
      memcpy(msgBuf + 12, cookie, strlen(cookie));
    }
  }
  
    a->magic = htons(a->magic);
    a->port = htons(a->port);
    numBytesRcvd = a->length;
    a->length = htons(a->length);
    
    printf("message sent to the client\n");
    printf("magic=%d \n",ntohs(a->magic));
    printf("length= %d\n", ntohs(a->length));
    printf("xid=%x %x %x %x\n", msg->xactionid >> 24, (msg->xactionid >> 16)&0xff, (msg->xactionid >> 8)&0xff, msg->xactionid & 0xff);
    //printf("version=%d\n", (a->flags) >> 4);
    //printf("flags=0x%x \n",  (a->flags)& 0xf);
    printf("version=%d\n", a->flags >> 4);
    printf("flags=0x%x \n",  a->flags & 0xf);
    printf("port=%d \n", ntohs(a->port));
    printf("result=%d \n", a->result);
    printf("variable part=%s\n", cookie);

    ssize_t numBytesSent = sendto(sock, msgBuf, numBytesRcvd, 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));
    if (numBytesSent < 0) 
      dieWithSystemError("sendto() failed)");
  
  }
  // NOT REACHED
}
