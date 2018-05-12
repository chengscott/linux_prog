#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"

int arg_parse(int, char **);
int shell_exec(char *);

int main(int argc, char **argv) {
  if (arg_parse(argc, argv) == -1) return EXIT_FAILURE;

  char *cmd = NULL;
  size_t len = 0;
  do {
    printf("\x1B[32m~\033[0m> ");
    fflush(stdout);
    if (getline(&cmd, &len, stdin) == -1) break;
  } while (shell_exec(cmd) != -1);
  free(cmd);
  return EXIT_SUCCESS;
}

int arg_parse(int argc, char **argv) {
  if (argc == 3) {
    int euid = atoi(argv[1]), egid = atoi(argv[2]);
    if (set_euid_egid(euid, egid) == -1) {
      fprintf(stderr, "Failed to set euid or egid.\n");
      return -1;
    }
    return 0;
  }
  fprintf(stderr, "Usage: %s {UID} {GID}\n", argv[0]);
  return -1;
}

int shell_exec(char *line) {
  char cmd[1024], extra[1024];
  if (sscanf(line, "%s %s", cmd, extra) == -1) return 0;
  const size_t len = sizeof(builtin_name) / sizeof(char *);
  for (size_t i = 0; i < len; ++i) {
    if (strcmp(cmd, builtin_name[i]) == 0) {
      int ret = (*builtin_func[i])(line);
      if (ret == 1) return -1;
      if (ret == -1) fprintf(stderr, "Wrong arguments\n");
      return 0;
    }
  }
  fprintf(stderr, "Unknown command. Type 'help' for help\n");
  return 0;
}
