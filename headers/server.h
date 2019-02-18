/*
Author: Ryan Moore
Date: 2019 Feb 15
Description: Contains useful functions used in the server. These fuctions are outside the main.c to allow easy testing.
*/
#include <stdio.h>

enum error_type {
  FOUR_ZERO_FOUR,
  NON_GET,
  USED_PARENT,
  INVALID_CONTENT_TYPE,
};

enum log_type {
  ERROR,
  INFO,
  WARN,
};

/* Checks that the requested file is allowed to be transmitted */
const char* get_content_type(const char* request);
void write_to_log(FILE* file, enum log_type, const char* message, int connection_num);
void send_error(int socket_fd, enum error_type error);
void serve_client(int socket_fd, FILE* log_file, int connection_num);
void start_listener();
