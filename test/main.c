/*
Author: Ryan Moore
Date: 2019 Feb 14
Description: Created to test that the pinite returns expected results from the server. And correctly logs the actions. This program is meant to execute inside of the directory of the website.
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include "config.h"

#define FILE_BUFFER_SIZE 16384
#define PATH_SIZE 256
#define PATH_FILE_BUFFER_SIZE 256

void get_curl(char buffer[])
{
  pid_t pid = fork();
  int pipe_fd[2];
  int read_size = 0;
  int response_size = 0;

  (void)pipe(pipe_fd);
  if (pid == 0) {
    (void)close(pipe_fd[0]);
    (void)execlp("curl", "curl", buffer, NULL);
  } else {
    (void)close(pipe_fd[1]);
    waitpid(pid, NULL, 0);
    while((read_size = read(pipe_fd[0], buffer, FILE_BUFFER_SIZE)) > 0) {
      response_size += read_size;
    }
    buffer[response_size] = 0;
  }
}

void recursively_read_dir(char buffer[PATH_FILE_BUFFER_SIZE][PATH_SIZE], char header[PATH_SIZE], int* index)
{
  DIR* dir;
  static struct dirent* dir_structure;
  int len = 0;
  int i = 0;
  dir = opendir(header);
  if (dir) {
    while((dir_structure = readdir(dir)) != NULL) {
      if (dir_structure->d_name[0] == '.' && dir_structure->d_name[1] == 0 ||
          dir_structure->d_name[0] == '.' && dir_structure->d_name[1] == '.') {
        continue;
      }
      (void)sprintf(buffer[*index], "%s/%s", header, dir_structure->d_name);
      (*index)++;
      if(dir_structure->d_type == DT_DIR) {
        len = strlen(header);
        (void)sprintf(&header[len], "/%s", dir_structure->d_name);
        recursively_read_dir(buffer, header, index);
      }
    }

    len = strlen(header);
    for (i = len; header[i] != '/' && i >= 0; i--) {}
    header[i] = 0;

    (void)closedir(dir);
  }
}

int main(int argc, char** args)
{
  if (argc != 2) {
    printf("Please enter the server address like so\n  SSWSTest <server url>\n");
    return -1;
  }

  static char path_buffer[PATH_FILE_BUFFER_SIZE][PATH_SIZE];
  static char head_buffer[PATH_SIZE];
  static char buffer[FILE_BUFFER_SIZE];
  (void)sprintf(buffer, args[1]);
  (void)sprintf(head_buffer, ".");
  int index = 0;
  int content_location = 0;
  int position = 0;

  while (HTTP10_404[position] != 0) {
    if (content_location == -3) {
      if(HTTP10_404[position] == '\n') {
        content_location = position + 1;
        break;
      } else {
        content_location = 0;
      }
    }

    if (content_location == -2) {
      if (HTTP10_404[position] == '\r') {
        content_location = -3;
      } else {
        content_location = 0;
      }
    }

    if (content_location == -1) {
      if ( HTTP10_404[position] == '\n') {
        content_location = -2;
      } else {
       content_location = 0;
      }
    }

    if(HTTP10_404[position] == '\r' && content_location == 0) {
      content_location = -1;
    }

    position++;
  }

  recursively_read_dir(path_buffer, head_buffer, &index);
  for (int i = 0; i < index; i++) {
    (void)sprintf(buffer, "%s%s", args[1], &path_buffer[i][1]);
    get_curl(buffer);
    if (buffer[0] == 0) {
      continue;
    }
    if (strcmp(&HTTP10_404[content_location], buffer) != 0){
      printf("404: \n%s\n Returned:\n%s\n\n", &path_buffer[i][1], buffer);
    }
  }
}
