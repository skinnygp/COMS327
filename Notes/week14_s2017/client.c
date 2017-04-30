#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 1024

void error(const char *s)
{
  perror(s);

  exit(0);
}

int main(int argc, char *argv[])
{
  int fd;
  int port;
  int n;
  struct sockaddr_in server_addr;
  struct hostent *server;
  char b[MAX_SIZE];

  if (argc != 3) {
    fprintf(stderr,"Usage: %s <hostname> <port>\n", argv[0]);
    exit(0);
  }

  port = atoi(argv[2]);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("socket");
  }

  if (!(server = gethostbyname(argv[1]))) {
    fprintf(stderr, "hostname lookup failure\n");

    exit(0);
  }

  bzero((char *) &server_addr, sizeof (server_addr));

  server_addr.sin_family = AF_INET;
  memcpy(server->h_addr, &server_addr.sin_addr.s_addr, server->h_length);
  server_addr.sin_port = htons(port);

  if (connect(fd, (struct sockaddr *) &server_addr,
              sizeof (server_addr)) < 0) {
    error("connect");
  }

  printf("Enter message to send: ");
  fgets(b, MAX_SIZE, stdin);

  if ((n = write(fd, b, strlen(b))) < 0) {
    error("write");
  }

  if ((n = read(fd, b, strlen(b))) < 0) {
    error("read");
  }

  printf("Received \"%s\" from server.", b);

  return 0;
}
