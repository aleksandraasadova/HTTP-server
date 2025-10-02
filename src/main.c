#include "http-server.h"

void parse_request(const int client_fd, http_request *request) {
  char request_buffer[4024];

  int nbytes = read(client_fd, request_buffer, sizeof(request_buffer) - 1);

  if (nbytes > 0) {
    char reply[2048];
    request_buffer[nbytes] = '\0';
    sscanf(request_buffer, "%8s %1024s %8s", request->method, request->path,
           request->version);

    char *line = strchr(request_buffer, '\n');
    line++;  // parsing headers
    int header_count = 0;
    while (*line && header_count < MAX_HEADERS) {
      char *end_line =
          strstr(line, "\r\n");  // pointer to the beginning of sub-string
      *end_line = '\0';
      char *colon = strchr(line, ':');
      if (colon) {
        *colon = '\0';
        strncpy(request->headers[header_count].name, line, 31);
        request->headers[header_count].name[31] = '\0';
        const char *value = colon + 1;
        while (*value == ' ') value++;
        strncpy(request->headers[header_count].value, value, 255);
        request->headers[header_count].value[255] = '\0';
        header_count++;
      }
      line = end_line + 2;
    }
    request->header_count = header_count;
  }
}
const char *get_header(const http_request *req, const char *name) {
  for (int i = 0; i < req->header_count; i++) {
    if (strcasecmp(req->headers[i].name, name) == 0) {
      return req->headers[i].value;
    }
  }
}
/*
Task 1: Extract URL path (send 404 or 200 response):
    GET request, with a random string as the path - should answer with HTTP/1.1
404 Not Found\r\n\r\n; if path is / - HTTP/1.1 200 OK\r\n\r\n Task 2: the
/echo/{str} endpoint, which accepts a string and returns it in the response
body. Task 3: the /user-agent endpoint, which reads the User-Agent request
header and returns it in the response body.
*/
void send_response(const int client_fd, const http_request *request) {
  char reply[1024];
  const char *body = NULL;
  if (strcmp(request->path, "/") == 0) {
    // sprintf(reply, "HTTP/1.1 200 OK\r\n\r\n");
    body = "Welcome, Client!";
  } else if (strncmp(request->path, "/echo/", 6) == 0) {
    body = request->path + 6;
    // sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type:
    // text/plain\r\nContent-Length: %zu\r\n\r\n%s", strlen(echo_str),
    // echo_str);
  } else if (strcmp(request->path, "/user-agent") == 0) {
    body = get_header(request, "User-Agent");
  } else {
    sprintf(reply,
            "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n");
    send(client_fd, reply, strlen(reply), 0);
    return;
  }
  sprintf(reply,
          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
          "%zu\r\n\r\n%s",
          strlen(body), body);
  send(client_fd, reply, strlen(reply), 0);
}

int handle_client(int const server_fd, http_request *request) {
  int client_addr_len;
  struct sockaddr_in client_addr;
  client_addr_len = sizeof(client_addr);

  int client_fd =
      accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd == -1) {
    printf("Accept failed: %s\n", strerror(errno));
    close(server_fd);
    return 1;
  }
  printf("Client connected\n");

  parse_request(client_fd, request);
  send_response(client_fd, request);
  // request

  close(client_fd);

  return 0;
}

int main() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int server_fd;
  if (server_init(&server_fd) != 0) {
    return 1;
  }
  http_request request;
  if (handle_client(server_fd, &request) != 0) {
    return 1;
  }
  close(server_fd);

  return 0;
}

// printf("%s", request_buffer);

/*
char *client_ip = inet_ntoa(client_addr.sin_addr);
int client_port = ntohs(client_addr.sin_port);
printf("Client IP: %s\nClient Port: %d\n", client_ip, client_port);
*/

/* char *find = "GET / ";
    char *find_echo = "GET /echo/";
    if (strncmp(request, find, strlen(find)) == 0) {
      //char reply[] = "HTTP/1.1 200 OK\r\n\r\n";
      char reply[] = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello,
  world!"; send(client_fd, reply, strlen(reply), 0); } else { char reply[] =
  "HTTP/1.1 404 Not Found\r\n\r\n"; send(client_fd, reply, strlen(reply), 0);
    }
    if (strncmp(request, find_echo, strlen(find)) == 0) {
      ;
    }
  }
*/

/*    char request[4024];
    int nbytes = read(client_fd, request, sizeof(request) - 1);
    if (nbytes > 0) {
        request[nbytes] = '\0';   // превращаем в C-строку
        //printf("%s", request);

        //char *strtok_r(char *str, const char *delim, char **saveptr);
        char *request_tail;
        char *method = strtok_r(request, " ", &request_tail);
        char *path   = strtok_r(NULL, " ", &request_tail);
        char *path_tail;

        if (strncmp(method, "GET", 3) == 0) { // extract path from GET request
        char *directory = strtok_r(path, "/", &path_tail);
        char reply[1024];
        if (directory == NULL || directory[0] == '\0') { // path = "/"
            sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type:
   text/plain\r\n\r\n");
        }
        else if (strncmp(directory, "echo", 4) == 0) {
            if (path_tail == NULL || path_tail[0] == '\0') { // path /echo/ or
   /echo sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            } else {
            sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type:
   text/plain\r\nContent-Length: %zu\r\n\r\n%s", strlen(path_tail), path_tail);
            }
        } else {
            sprintf(reply, "HTTP/1.1 404 Not Found\r\nContent-Type:
   text/plain\r\n\r\n");
        }
        send(client_fd, reply, strlen(reply), 0);
        }
    }*/

// sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nMethod:
// %s\nPath: %s\nVersion: %s\n", request->method, request->path,
// request->version); send(client_fd, reply, strlen(reply), 0);