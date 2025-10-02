#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

/* iterative server*/
#include <arpa/inet.h>  // определить ip-адрес и порт в удобном формате
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>  // структура адреса сокетаsockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_HEADERS 32

typedef struct {
  char name[32];
  char value[256];
} header_type;

typedef struct {
  char method[8];
  char path[1024];
  char version[8];
  header_type headers[MAX_HEADERS];
  int header_count;
} http_request;

int server_init(int *server_fd);

#endif