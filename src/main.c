#include <unistd.h>
#include "config.h"
#include "server.h"

int main(int argc, char** argv)
{
  /* Print help */
  if (argc != 1) { /* Print help menu */
    printf(HELP_STR);
    return -1;
  }

  /* Check port */
  if (PORT <= 0 || PORT > 60000) {
    printf("Bad port number!, Try something between 1 and 60000\n");
    return -1;
  }

  /* Change directory */
  if (chdir(ROOT_DIR) == -1) {
    printf("Failed to change directory to ROOT_DIR in config!\n");
    return -1;
  }

  /* Create child */
  if (fork() != 0) {
    return 0; /* Parent is done */
  }
  start_listener();
}
