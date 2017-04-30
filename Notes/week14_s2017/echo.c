#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define MAX_SIZE 1024

void error(char *s)
{
  perror(s);

  exit(1);
}

int main(int argc, char *argv[])
{
  int listenfd;
  int acceptfd;
  char b[MAX_SIZE];
  int n;
  unsigned short port;
  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *host;
  char *host_addr;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);

    exit(1);
  }

  port = atoi(argv[1]);

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("socket");
  }

  /* Not necessary */
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                 (int []) { 1 }, sizeof (int))) {
    error("setsockopt");
  }

  bzero((char *) &server, sizeof (server));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  if (bind(listenfd, (struct sockaddr *) &server, sizeof (server))) {
    error("bind");
  }

  if (listen(listenfd, 3) < 0) {
    error("listen");
  }

  for (;;) {
    if ((acceptfd = accept(listenfd, (struct sockaddr *) &client,
                           (unsigned []) { sizeof (client) })) < 0) {
      error("accept");
    }

    if (!(host = gethostbyaddr((const char *) &client.sin_addr.s_addr, 
                               sizeof (client.sin_addr.s_addr), AF_INET))) {
      error("gethostbyaddr");
    }

    if (!(host_addr = inet_ntoa(client.sin_addr))) {
      error("inet_ntoa\n");
    }

    printf("New connection from host: %s; IP: %s\n", 
	   host->h_name, host_addr);
    
    if ((n = read(acceptfd, b, MAX_SIZE)) < 0) {
      error("read");
    }
    b[n] = '\0';

    printf("Read %d bytes: %s", n, b);

    if ((n = write(acceptfd, b, strlen(b))) < 0) {
      error("write");
    }

    close(acceptfd);
  }

  return 0;
}
