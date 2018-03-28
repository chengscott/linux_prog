#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"

int arg_parse(int, char **);
void shell();
int shell_exec(char *);

int main(int argc, char **argv) {
  if (arg_parse(argc, argv) == -1) {
    return EXIT_FAILURE;
  }

  shell();

  return EXIT_SUCCESS;
}

int arg_parse(int argc, char **argv) {
  if (argc == 3) {
    int EUID = atoi(argv[1]);
    int EGID = atoi(argv[2]);
    // TBA change euid egid
    return 0;
  }
  fprintf(stderr, "Usage: %s {UID} {GID}\n", argv[0]);
  return -1;
}

void shell() {
  char *cmd = NULL;
  size_t len = 0;
  int eof = 0;
  do {
    printf("~> ");
    if ((eof = getline(&cmd, &len, stdin)) != -1) eof = shell_exec(cmd);
    // cleanup
    free(cmd);
    cmd = NULL;
    len = 0;
  } while (eof != -1);
}

int shell_exec(char *line) {
  char cmd[1024], extra[1024];
  if (sscanf(line, "%s %s", cmd, extra) == -1) return 0;
  const size_t len = sizeof(builtin_name) / sizeof(char *);
  for (size_t i = 0; i < len; ++i) {
    if (strcmp(cmd, builtin_name[i]) == 0) {
      int ret = (*builtin_func[i])(line);
      if (ret == 1) return -1;
      if (ret == -1) fprintf(stderr, "Too many arguments\n");
      return 0;
    }
  }
  fprintf(stderr, "Unknown command. Type 'help' for help\n");
  return 0;
}