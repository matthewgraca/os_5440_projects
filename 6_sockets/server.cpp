#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

#define CLIENT_SOCKET 12341
#define SERVER_SOCKET 12340
#define BUFF_LEN 200 

using namespace std;

int main(){
  int pid = fork();
  // child: listener
  if (pid == 0){
    // define client socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(CLIENT_SOCKET);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to client socket
    cout << "Connecting to client socket..." << endl;
    while (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1){
      cout << "..." << endl;
      sleep(1);
    }
    cout << "Connected! You can now read messages sent by the server." << endl;

    // read messages
    char buf[BUFF_LEN];
    int msglen = 1;
    while (msglen > 0){
      msglen = recv(sockfd, buf, BUFF_LEN, 0);
      string msg = (msglen <= 0) ? "Peer has left, exiting..." : "A: " + string(buf);
      cout << msg << endl;
    }

    // close client socket once finished
    close(sockfd);
  }
  else{
    // create server socket
    int sockfd = 0, newsockfd = 0, BACKLOG = 5;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in my_addr, their_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(SERVER_SOCKET);

    // bind and listen
    bind(sockfd,(struct sockaddr*)&my_addr, sizeof(my_addr));
    listen(sockfd, BACKLOG);

    // accept connection from client socket
    socklen_t sin_size = sizeof(struct sockaddr_in);
    newsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

    cout << "Connection accepted from client. You can now send messages. Type \"/q\" to exit." << endl;

    // write data
    char inputbuf[BUFF_LEN];
    int retval = 1;
    while (retval != -1){
      cin.getline(inputbuf, BUFF_LEN);
      retval = (string(inputbuf) == "/q") ? -1 : send(newsockfd, inputbuf, BUFF_LEN, 0); 
    }

    // close connection to client socket
    close(newsockfd);

    // close server socket
    close(sockfd);
  }

  return 0;
}
