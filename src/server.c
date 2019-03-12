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
#include "web_minimizer.h"
const char* get_content_type(const char* request)
{
  int ext_length;
  int length = strlen(request);
  for (int i = 0; extensions[i].extension != 0; i++) {
    ext_length = strlen(extensions[i].extension);
    if (ext_length < length) {
      if (strncmp(&request[length - ext_length], extensions[i].extension, ext_length) == 0) {
        return extensions[i].content_type;
      }
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

void send_error(int socket_fd)
{
  (void)write(socket_fd, HTTP10_404, strlen(HTTP10_404));
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
    send_error(socket_fd);
  }
  buffer[read_size] = 0;

  /* Only get request allowed */
  if (strncasecmp(buffer, "GET ", 4) != 0) {
    write_to_log(log_file, WARN, "Client tried to use a non-get request!", connection_num);
    send_error(socket_fd);
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
      send_error(socket_fd);
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
    send_error(socket_fd);
  }

  file = fopen(&buffer[5], "r");
  if (file == NULL) {
    write_to_log(log_file, WARN, "Client tried to get a non-existent file!", connection_num);
    send_error(socket_fd);
  }

  write_to_log(log_file, INFO, buffer, connection_num);
  (void)fseek(file, 0, SEEK_END);
  file_length = ftell(file);
  (void)fseek(file, 0, SEEK_SET);

  char* file_input = malloc(file_length);
  (void)fread(file_input, 1, file_length, file);

  if (strcmp(content_type, "text/html") == 0) {
    file_length = minimize_html(file_input);
  } else if (strcmp(content_type, "text/css") == 0) {
    file_length = minimize_css(file_input);
  } else if (strcmp(content_type, "text/javascript") == 0) {
    file_length = minimize_js(file_input);
  }

  /* Write Header of Response */
  (void)sprintf(buffer, HTTP10_OK_HEADER, content_type, file_length);
  (void)write(socket_fd, buffer, strlen(buffer));
  (void)write(socket_fd, file_input, file_length);

  /* Close and sleep for a bit to flush buffers */
  (void)fclose(file);
  (void)fclose(log_file);
  free(file_input);
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
