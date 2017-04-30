#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_SIZE 1024
#define MAX_CLIENTS 1023

int client_list[MAX_CLIENTS + 1]; /* We're going to use slot 0 for */
int num_clients;                  /* the listen file descriptor.   */

void error(char *s)
{
  perror(s);

  exit(1);
}

int get_maxfd()
{
  int max, i;

  for (max = client_list[0], i = 1; i <= num_clients; i++) {
    if (client_list[i] > max) {
      max = client_list[i];
    }
  }

  return max;
}

int close_client(int client) {
  close(client_list[client]);
  client_list[client] = client_list[num_clients--];

  return 0;
}

int send_to_all(int from, char *msg, int msg_len) {
  int i;
  char fd[8];

  if (from) {
    snprintf(fd, 8, "%d: ", client_list[from]);
  } else {
    snprintf(fd, 8, "Host: ");
  }

  for (i = 1; i <= num_clients; i++) {
    if (i != from) {
      if (write(client_list[i], fd, strlen(fd)) < 0) {
        if (errno == EINTR) {
          /* To simplify the logic, the client just loses this message. */
          /* Do nothing. */
        } else {
          /* And we're not going to handle error.  Terminate everything here. */
          exit(1);
        }
      }
      if (write(client_list[i], msg, msg_len) < 0) {
        if (errno == EINTR) {
        } else {
          exit(1);
        }
      }
    }
  }

  if (write(STDOUT_FILENO, msg, msg_len) < 0) {
    if (errno == EINTR) {
    } else {
      exit(1);
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  char b[MAX_SIZE];
  int i, n;
  unsigned short port;
  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *host;
  char *host_addr;
  int maxfdp1;
  fd_set readfds;
  int nready;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);

    exit(1);
  }

  port = atoi(argv[1]);

  if ((client_list[0] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("socket");
  }

  if (setsockopt(client_list[0], SOL_SOCKET, SO_REUSEADDR,
                 (int []) { 1 }, sizeof (int))) {
    error("setsockopt");
  }

  bzero((char *) &server, sizeof (server));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  if (bind(client_list[0], (struct sockaddr *) &server, sizeof (server))) {
    error("bind");
  }

  if (listen(client_list[0], 3) < 0) {
    error("listen");
  }

  client_list[++num_clients] = STDIN_FILENO;

  for (;;) {
    maxfdp1 = get_maxfd(client_list, num_clients) + 1;
    FD_ZERO(&readfds);
    for (i = 0; i <= num_clients; i++) {
      FD_SET(client_list[i], &readfds);
    }

    if ((nready = select(maxfdp1, &readfds, NULL, NULL, NULL)) < 0) {
      if (errno == EINTR) {
        continue;
      }

      error("select");
    }


    for (i = 0; i <= num_clients; i++) {
      if (FD_ISSET(client_list[i], &readfds)) {
        if (i == 0) {
          if (num_clients < MAX_CLIENTS) {
            if ((client_list[++num_clients] =
                 accept(client_list[0], (struct sockaddr *) &client,
                        (unsigned []) { sizeof (client) })) < 0) {
              error("accept");
            }

            if (!(host = gethostbyaddr((const char *) &client.sin_addr.s_addr, 
                                       sizeof (client.sin_addr.s_addr),
                                       AF_INET))) {
              error("gethostbyaddr");
            }

            if (!(host_addr = inet_ntoa(client.sin_addr))) {
              error("inet_ntoa\n");
            }

            snprintf(b, MAX_SIZE,
                     "Welcome friend!  Your ID is %d.\n"
                     "Send \"quit\" to log out.\n", client_list[num_clients]);
            if (write(client_list[num_clients], b, strlen(b)) < 0) {
              if (errno == EINTR) {
              } else {
                exit(1);
              }
            }

            n = snprintf(b, MAX_SIZE,
                         "New connection from host: %s; IP: %s; ID: %d\n", 
                         host->h_name, host_addr, client_list[num_clients]);

            send_to_all(0, b, n);
            
          }
        } else {
          if ((n = read(client_list[i], b, MAX_SIZE)) < 0) {
            if (errno == EINTR) {
              /* Again, to simplify the logic, ignore this error. */
            } else {
              close_client(i);
            }
          }

          b[n] = '\0';

          if (!strcmp(b, "quit\r\n")) {
            if (write(client_list[i], "Bye-bye!\n", 9) < 0) {
              if (errno == EINTR) {
              } else {
                exit(1);
              }
            }

            snprintf(b, MAX_SIZE, "%d has logged off.\n", client_list[i]);
            close_client(i);
            send_to_all(0, b, strlen(b));
            --i;

          } else {
            send_to_all(i, b, strlen(b));
          }
        }
      }
    }
  }

  return 0;
}
