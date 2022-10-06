// #include "server.h"
#include <sys/socket.h>
#include <string>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <functional>

#define DO_DEBUG true
#define SOCKET_PATH "/tmp/server.socket"

class Server{
    public:
        Server(std::function<void(std::string)> Message_cb);
        void start_listen_thread();
    private:
        std::function<void(std::string)> recv_cb; /// user defined callback
        std::thread listener;
        void listen_and_accept();
        int fd;
        int con_fd;
        struct sockaddr_un saddr;
        void clear_sockaddr();
        void populate_sockaddr();
        void bind_socket();
        void call_accept();
        void error(std::string msg);
        void msg(std::string msg);
        int num_read;
        char recv_buff[30]; /// recived messages will be read into here
};

Server::Server(std::function<void(std::string)> Message_cb){
    recv_cb = Message_cb;
    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    clear_sockaddr();
    populate_sockaddr();
}

void Server::clear_sockaddr(){
    memset(&saddr, 0, sizeof(saddr));
}

void Server::populate_sockaddr(){
    saddr.sun_family = AF_LOCAL;
    memcpy(&saddr.sun_path,SOCKET_PATH,sizeof(SOCKET_PATH));
}

void Server::bind_socket(){
    bind(fd,(sockaddr*)&saddr,sizeof(saddr));
}

void Server::error(std::string msg){
    perror(msg.data());
    exit(1);
}
void Server::msg(std::string msg){
    if (DO_DEBUG){
        std::cout << msg << "\n";
    }
}

void Server::start_listen_thread(){
    listener = std::thread(&Server::listen_and_accept);
}

void Server::call_accept(){
    msg("Calling Accept");
    con_fd = accept(fd,NULL,NULL);
    if(con_fd==-1){
        error("accept");
    }
}
/**
   * Starts thread and handles incoming connections and messages.
     **/
void Server::listen_and_accept(){
    while(1){
        call_accept();
        while((num_read = recv(con_fd,recv_buff,sizeof(recv_buff),0))>0){
            recv_cb(std::string(recv_buff,num_read));
        }
        close(con_fd);
    }
}


int main(int argc, char *argv[]) {
    auto cb = [](std::string s){
        std::cout << "Message: " << s << " recieved";
    };
    Server s(cb);
    s.start_listen_thread();
}
