#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define N 1024

int builtin_cat(char *cmd) { return 0; }

int builtin_cd(char *line) {
  char cmd[N], arg[N], extra[N];
  if (sscanf(line, "%s %s %s", cmd, arg, extra) != 2) return -1;

  int ret = chdir(arg);
  // if (ret == -1)
  return 0;
}

int builtin_chmod(char *cmd) { return 0; }

int builtin_echo(char *cmd) { return 0; }

int builtin_exit(char *line) {
  char cmd[N], extra[N];
  if (sscanf(line, "%s %s", cmd, extra) != 1) return -1;

  return 1;
}

int builtin_find(char *cmd) { return 0; }

int builtin_help(char *line) {
  char cmd[N], extra[N];
  if (sscanf(line, "%s %s", cmd, extra) != 1) return -1;

  const char *help_string[] = {
      "cat {file}:              Display content of {file}.",
      "cd {dir}:                Switch current working directory to {dir}.",
      "chmod {mode} {file/dir}: Change the mode (permission) of a file or "
      "directory.",
      "                         {mode} is an octal number.",
      "                         Please do not follow symbolc links.",
      "echo {str} [filename]:   Display {str}. If [filename] is given,",
      "                         open [filename] and append {str} to the file.",
      "exit:                    Leave the shell.",
      "find [dir]:              List files/dirs in the current working "
      "directory",
      "                         or [dir] if it is given.",
      "                         Minimum outputs contatin file type, size, and "
      "name.",
      "help:                    Display help message.",
      "id:                      Show current euid and egid.",
      "mkdir {dir}:             Create a new directory {dir}.",
      "pwd:                     Print the current working directory.",
      "rm {file}:               Remove a file.",
      "rmdir {dir}:             Remove an empty directory.",
      "stat {file/dir}:         Display detailed information of the given "
      "file/dir.",
      "touch {file}:            Create {file} if it does not exist,",
      "                         or update its access and modification "
      "timestamp.",
      "umask {mode}:            Change the umask of the current session."};
  size_t len = sizeof(help_string) / sizeof(char *);
  for (size_t i = 0; i < len; ++i) printf("%s\n", help_string[i]);
  return 0;
}

int builtin_id(char *line) {
  char cmd[N], extra[N];
  if (sscanf(line, "%s %s", cmd, extra) != 1) return -1;

  printf("uid=%d gid=%d\n", geteuid(), getegid());
  return 0;
}

int builtin_mkdir(char *line) {
  char cmd[N], arg[N], extra[N];
  if (sscanf(line, "%s %s %s", cmd, arg, extra) != 2) return -1;

  int ret = mkdir(arg, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // if (ret == -1)
  return 0;
}

int builtin_pwd(char *line) {
  char cmd[N], extra[N];
  if (sscanf(line, "%s %s", cmd, extra) != 1) return -1;

  char name[N];
  getcwd(name, N);
  // if (name == NULL)
  printf("%s\n", name);
  return 0;
}

int builtin_rm(char *line) {
  char cmd[N], arg[N], extra[N];
  if (sscanf(line, "%s %s %s", cmd, arg, extra) != 2) return -1;

  int ret = remove(arg);
  // if (ret == -1)
  return 0;
}

int builtin_rmdir(char *line) {
  char cmd[N], arg[N], extra[N];
  if (sscanf(line, "%s %s %s", cmd, arg, extra) != 2) return -1;

  int ret = rmdir(arg);
  // if (ret == -1)
  return 0;
}

int builtin_stat(char *cmd) { return 0; }

int builtin_touch(char *line) {
  char cmd[N], arg[N], extra[N];
  if (sscanf(line, "%s %s %s", cmd, arg, extra) != 2) return -1;

  FILE *file;
  // if (access(arg, F_OK) != -1)
  file = fopen(arg, "a+");
  fclose(file);
  return 0;
}

int builtin_unmask(char *cmd) { return 0; }
