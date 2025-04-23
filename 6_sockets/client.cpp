#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

int main()
{
    int sockfd = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12340);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd,(struct sockaddr *)&servaddr, sizeof(struct sockaddr));

    int msglen;
    char buf[200];
    msglen= recv(sockfd,buf, 200, 0);
    while (msglen > 0)
    {
       cout << buf << endl;
       msglen= recv(sockfd,buf, 200, 0);
    }
    close(sockfd);
    return 0;
}
