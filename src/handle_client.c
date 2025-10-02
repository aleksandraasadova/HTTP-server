#include "http-server.h"

const char *get_header(const http_request *req, const char *name);

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

void handle_client(int client_fd) {
  http_request request;
  parse_request(client_fd, &request);
  send_response(client_fd, &request);
  close(client_fd);
}

void *handle_client_thread(void *arg) {
  int client_fd = *(int *)arg;
  free(arg);
  handle_client(client_fd);
  return NULL;
}