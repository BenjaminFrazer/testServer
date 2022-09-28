#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // unix c standard library
#include <signal.h>
#include <sys/un.h> // includes type sockaddr_un
#include <stdio.h>
#include <string.h> // includes memset
#include <stdlib.h>
#include <iostream>
#include "server_protocol.h"

#define BACKLOG 0 // number of pending connections we will accept

void error (const char *msg);
void msg (const char *msg);
void fatal_error_signal (int sig);

volatile int listen_fd=0, connect_fd=0;

int main(int argc, char *argv[]) {
    signal(SIGINT,fatal_error_signal);
    // int len = 0;
    // int doContinue=1;
    char buff[30];
    struct sockaddr_un saddr;//,caddr; //server and client adress struct
    memset(&saddr, 0, sizeof(saddr));          /* clear the bytes */
    saddr.sun_family= AF_LOCAL;
    memcpy(saddr.sun_path,DOMAIN_NAME,sizeof(DOMAIN_NAME));// write socket domain
    // Delete any file that already exists at the address. Make sure the deletion
    // succeeds. If the error is just that the file/directory doesn't exist, it's fine.
    if (remove(DOMAIN_NAME) == -1 && errno != ENOENT) {
        error("remove-%s");
    }

    // initialise socket
    msg("Creating socket");
    listen_fd = socket(AF_LOCAL, SOCK_STREAM,0);
    if (listen_fd==-1){
        error("socket");
    }
    // bind file descriptor
    msg("Calling bind on socket");
    if (bind(listen_fd, (const struct sockaddr*) &saddr, sizeof(struct sockaddr_un))==-1){
        error("bind");
    }
    // mark socket for accepting incoming connections
    msg("Calling Listen");
    if (listen(listen_fd,BACKLOG)==-1){
        error("listen");
    }
    //accept a connection
    msg("Calling Accept");
    connect_fd = accept(listen_fd, NULL, NULL);
    // connect_fd = accept(listen_fd, (struct sockaddr*) &caddr, (socklen_t*)sizeof(caddr));
    if (connect_fd==-1){
        error("connection");
    }
    msg("Connection Accepted");
    int numRead;
    while ((numRead=recv(connect_fd,buff,sizeof(buff),0))>0) {// loop forever working through connections
        // msg("Calling Recieve");
        memcpy(&buff[numRead], "\0", 1);
        std::cout << buff <<"\n";
        send(connect_fd,buff,numRead,0);
    }
    close(listen_fd);
    return 0;
}

void error (const char *msg){
    perror (msg);
    exit (1);
}

void msg (const char *msg){
    if (DO_DEBUG){
        std::cout << msg << "\n";
    }
}

/* cleanup the socket domain (pinched from:
https://www.gnu.org/software/libc/manual/html_node/Termination-in-Handler.html)*/
volatile sig_atomic_t fatal_error_in_progress = 0;

void fatal_error_signal (int sig)
{

  /* Since this handler is established for more than one kind of signal,
     it might still get invoked recursively by delivery of some other kind
     of signal.  Use a static variable to keep track of that. */

  if (fatal_error_in_progress)
    raise (sig);
  fatal_error_in_progress = 1;

  /* Now do the clean up actions:
     - reset terminal modes
     - kill child processes
     - remove lock files */

  msg("Cleaning up!");
  close(listen_fd);
  close(connect_fd);

  /* Now reraise the signal.  We reactivate the signal’s
     default handling, which is to terminate the process.
     We could just call exit or abort,
     but reraising the signal sets the return status
     from the process correctly. */
  signal (sig, SIG_DFL);
  raise (sig);
}
