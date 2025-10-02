#include "http-server.h"

int main() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int server_fd;
  if (server_init(&server_fd) != 0) {
    return 1;
  }

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
      fprintf(stderr, "Accept failed: %s\n", strerror(errno));
      continue;
    }
    printf("Client connected\n");

    int *fd_ptr = malloc(sizeof(int));
    *fd_ptr = client_fd;

    pthread_t thread;
    if (pthread_create(&thread, NULL, handle_client_thread, fd_ptr) != 0) {
      fprintf(stderr, "Failed to create thread\n");
      close(client_fd);
      free(fd_ptr);
      continue;
    }
    pthread_detach(thread);
  }
  close(server_fd);

  return 0;
}

