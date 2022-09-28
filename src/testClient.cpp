#include <string>
#include <sys/types.h> // includes unix type definitions?
#include <unistd.h> // unix c standard library
#include <sys/socket.h>
#include <signal.h>
#include <sys/un.h> // includes type sockaddr_un
#include <stdio.h>
#include <string.h> // includes memset
#include <stdlib.h>
#include <iostream>
#include "server_protocol.h"

void error (const char *msg);
void msg(const char *msg);

int main(int argc, char *argv[]) {
    int confd = 0;
    std::string mystring;
    char buff[30];
    // construct server address
    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sun_family = AF_LOCAL;
    memcpy(saddr.sun_path,DOMAIN_NAME,sizeof(DOMAIN_NAME));
    //create socket
    msg("calling socket");
    confd = socket(AF_LOCAL,SOCK_STREAM,0);
    // call connect
    msg("calling connect()");
    if (connect(confd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_un))==-1){
        error("connect() Failed");}
    std::cout << "enter a string to be sent to the server!\n";
    while(1){
        std::cout << ">";
        std::getline(std::cin, mystring);
        write(confd, mystring.data(), mystring.length());
        int len = read(confd, buff, sizeof(buff));
        memcpy(&buff[len], "\0", 1); // this is neccecery  to null terminate the string we wish to send
        std::cout << buff << "\n";
    }
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
