#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "server.h"
#include "config.h"

const char* get_content_type(const char* request)
{
  int ext_length;
  int length = strlen(request);
  for (int i = 0; extensions[i].extension != 0; i++) {
    ext_length = strlen(extensions[i].extension);
    if (strncmp(&request[length - ext_length], extensions[i].extension, ext_length) == 0) {
      return extensions[i].content_type;
    }
  }
  return (char*)0;
}
void write_to_log(FILE* file, enum log_type type, const char* message, int connection_num)
{
  static char buffer[BUFFER_SIZE];
  switch(type) {
  case ERROR:
    (void)sprintf(buffer, "Error: %d : %s\n", connection_num, message);
    break;
  case INFO:
    (void)sprintf(buffer, "Info: %d : %s\n", connection_num, message);
    break;
  case WARN:
    (void)sprintf(buffer, "WARN: %d : %s\n", connection_num, message);
    break;
  }
  (void)fwrite(buffer, 1, strlen(buffer), file);
  (void)fflush(file);
}

void send_error(int socket_fd, enum error_type err)
{
  switch(err) {
  case FOUR_ZERO_FOUR:
    (void)write(socket_fd, HTTP10_404, strlen(HTTP10_404)); break;
  case NON_GET:
    (void)write(socket_fd, HTTP10_NON_GET, strlen(HTTP10_NON_GET)); break;
  case USED_PARENT:
    (void)write(socket_fd, HTTP10_USED_PARENT, strlen(HTTP10_USED_PARENT)); break;
  case INVALID_CONTENT_TYPE:
    (void)write(socket_fd, HTTP10_INVALID_CONTENT_TYPE, strlen(HTTP10_INVALID_CONTENT_TYPE)); break;
  }
  sleep(1);
  exit(-1);
}

void serve_client(int socket_fd, FILE* log_file, int connection_num)
{
  int i = 0, j = 0;
  int ext_length = 0;
  int buf_length = 0;
  int file_length = 0;
  const char* content_type = (char*)0;
  long read_size = 0;
  static char buffer[BUFFER_SIZE + 1];
  FILE* file = NULL;

  /* Read clients requests */
  if ((read_size = read(socket_fd, buffer, BUFFER_SIZE)) < 1) {
    write_to_log(log_file, WARN, "Unable to read client's request!", connection_num);
    send_error(socket_fd, FOUR_ZERO_FOUR);
  }
  buffer[read_size] = 0;

  /* Only get request allowed */
  if (strncmp(buffer, "GET ", 4) != 0 && strncmp(buffer, "get ", 4) != 0) {
    write_to_log(log_file, WARN, "Client tried to use a non-get request!", connection_num);
    send_error(socket_fd, NON_GET);
  }

  /* End buffer after second word */
  for (i = 4; i < BUFFER_SIZE; i++) {
    if (buffer[i] == ' ') {
      buffer[i] = 0;
      break;
    }
  }

  /* No parent directory please, slightly annoying but for security reasons */
  for (j = 0; j < i - 1; j++) {
    if (buffer[j] == '.' && buffer[j + 1] == '.') {
      write_to_log(log_file, WARN, "Client tried to use parent directory!", connection_num);
      send_error(socket_fd, USED_PARENT);
    }
  }

  /* Convert empty /  to /index.html so it can actually load the file */
  if (strncmp(buffer, "GET /\0", 6) == 0 || strncmp(buffer, "get /\0", 6) == 0) {
    (void)strcpy(buffer,  "GET /index.html");
  }

  /* Check if the file extension is support, more security */
  content_type = get_content_type(buffer);

  if (content_type == 0) {
    write_to_log(log_file, WARN, "Client tried to get a non-supported extension type!", connection_num);
    send_error(socket_fd, FOUR_ZERO_FOUR);
  }

  file = fopen(&buffer[5], "r");
  if (file == NULL) {
    write_to_log(log_file, WARN, "Client tried to get a non-existent file!", connection_num);
    send_error(socket_fd, FOUR_ZERO_FOUR);
  }

  write_to_log(log_file, INFO, buffer, connection_num);
  fseek(file, 0L, SEEK_END);
  file_length = ftell(file);
  fseek(file, 0L, SEEK_SET);

  /* Write Header of Response */
  (void)sprintf(buffer, HTTP10_OK_HEADER, content_type, file_length);
  (void)write(socket_fd, buffer, strlen(buffer));

  /* Read and write data to the client from the file */
  while (read_size = fread(buffer, 1, BUFFER_SIZE, file)) {
    (void)write(socket_fd, buffer, read_size);
  }

  /* Close and sleep for a bit to flush buffers */
  fclose(file);
  fclose(log_file);
  sleep(1);
  exit(1);
}

void start_listener()
{
  FILE* log = fopen("server.log", "a+");
  static struct sigaction ignore;
  static struct sockaddr_in client_address;
  static struct sockaddr_in server_address;
  int listen_fd = 0;
  int socket_fd = 0;
  int length = 0;
  int hits = 0;
  pid_t pid = 0;

  if (log == NULL) {
    return;
  }

  ignore.sa_handler = SIG_IGN;
  (void)sigaction(SIGCHLD, &ignore, NULL);
  (void)sigaction(SIGHUP, &ignore, NULL);
  (void)setpgrp();

  write_to_log(log, INFO, "Starting server!", PORT);

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    write_to_log(log, ERROR, "Unable to create socket file descriptor!", -1);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(PORT);

  if (bind(listen_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
    write_to_log(log, ERROR, "Failed to bind to port!", PORT);
    return;
  }

  if (listen(listen_fd, CONNECT_BACKLOG_SIZE) == -1) {
    write_to_log(log, ERROR, "Failed to ready socket for listening!", -1);
    return;
  }

  length = sizeof(client_address);
  while (1) {
    if ((socket_fd = accept(listen_fd, (struct sockaddr*)&client_address, &length)) == -1) {
      write_to_log(log, ERROR, "Failed to accept connection, trying again!", hits);
    }

    if ((pid = fork()) == 0) {
      (void)close(listen_fd);
      serve_client(socket_fd, log, hits);
    } else {
      (void)close(socket_fd);
    }
    hits++;
  }
}
