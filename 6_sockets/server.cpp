#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

using namespace std;

// server speaks to the client
int main()
{
    int sockfd = 0, newsockfd = 0, BACKLOG = 5;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in my_addr, their_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(12340);

    bind(sockfd,(struct sockaddr*)&my_addr, sizeof(my_addr));

    listen(sockfd, BACKLOG);

    while(1)
    {
      socklen_t sin_size = sizeof(struct sockaddr_in);
      newsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
      char inputbuf[200];

      cin >> inputbuf;
      while (sizeof(inputbuf) > 0)
      {
       send(newsockfd, inputbuf, 200, 0);
       cin >> inputbuf;
      }
      close(newsockfd);
    }
    close(sockfd);
    return 0;
}
