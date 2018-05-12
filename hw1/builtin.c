#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#define N 1024

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

int error_handler() {
  fprintf(stderr, "Error: %s\n", strerror(errno));
  return 0;
}

int set_euid_egid(int euid, int egid) {
  if (setegid(egid) == -1 || seteuid(euid) == -1) {
    error_handler();
    return -1;
  }
  return 0;
}

const char *get_file_type(int mode) {
  const char *file_type[] = {"named pipe (fifo)",
                             "character special file",
                             "directory",
                             "block special file",
                             "regular file",
                             "symbolic link",
                             "socket",
                             "unknown"};
  const int file_mask[] = {S_IFIFO, S_IFCHR, S_IFDIR, S_IFBLK,
                           S_IFREG, S_IFLNK, S_IFSOCK};

  for (size_t i = 0; i < 7; ++i) {
    if (mode == file_mask[i]) {
      return file_type[i];
    }
  }
  return file_type[7];
}

int builtin_cat(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  struct stat info;
  if (stat(arg, &info) == -1) return error_handler();
  if (!S_ISREG(info.st_mode)) {
    fprintf(stderr, "Error: Is a %s\n", get_file_type(info.st_mode & S_IFMT));
    return 0;
  }
  FILE *file = fopen(arg, "r");
  if (file == NULL) return error_handler();
  char *content = NULL;
  size_t len = 0;
  while (getline(&content, &len, file) != -1) printf("%s", content);
  free(content);
  fclose(file);
  return 0;
}

int builtin_cd(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  if (chdir(arg) == -1) return error_handler();
  return 0;
}

int builtin_chmod(char *line) {
  char arg[N], extra[N];
  int mode;
  if (sscanf(line, "%*s %o %s %s", &mode, arg, extra) != 2) return -1;

  if (chmod(arg, mode) == -1) return error_handler();
  return 0;
}

int builtin_echo(char *line) {
  char arg[N], filename[N], extra[N];
  int argc;
  if ((argc = sscanf(line, "%*s %s %s %s", arg, filename, extra)) > 2)
    return -1;

  if (argc == 2) {
    FILE *file = fopen(filename, "a+");
    if (file == NULL) return error_handler();
    fprintf(file, "%s\n", arg);
    fclose(file);
  } else
    printf("%s\n", arg);
  return 0;
}

int builtin_exit(char *line) {
  char extra[N];
  if (sscanf(line, "%*s %s", extra) > 0) return -1;

  return 1;
}

int builtin_find(char *line) {
  char arg[N], extra[N];
  int argc;
  if ((argc = sscanf(line, "%*s %s %s", arg, extra)) > 1) return -1;

  const char *path = argc == 1 ? arg : ".";
  DIR *dir = opendir(path);
  if (dir == NULL) return error_handler();
  struct dirent *dp;
  struct stat info;
  char filename[N];
  while ((dp = readdir(dir)) != NULL) {
    sprintf(filename, "%s/%s", path, dp->d_name);
    printf("%-20s\t%-20s", dp->d_name,
           get_file_type(DTTOIF(dp->d_type) & S_IFMT));
    if (stat(filename, &info) == -1) {
      printf("\n");
      error_handler();
      continue;
    }
    printf("\t%-5ld\n", info.st_size);
  }
  closedir(dir);
  return 0;
}

int builtin_help(char *line) {
  char extra[N];
  if (sscanf(line, "%*s %s", extra) > 0) return -1;

  const size_t len = sizeof(help_string) / sizeof(char *);
  for (size_t i = 0; i < len; ++i) printf("%s\n", help_string[i]);
  return 0;
}

int builtin_id(char *line) {
  char extra[N];
  if (sscanf(line, "%*s %s", extra) > 0) return -1;

  printf("uid=%d gid=%d\n", geteuid(), getegid());
  return 0;
}

int builtin_mkdir(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  if (mkdir(arg, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    return error_handler();
  return 0;
}

int builtin_pwd(char *line) {
  char extra[N];
  if (sscanf(line, "%*s %s", extra) > 0) return -1;

  char name[N];
  getcwd(name, N);
  if (name == NULL) return error_handler();
  printf("%s\n", name);
  return 0;
}

int builtin_rm(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  struct stat info;
  if (stat(arg, &info) == -1) return error_handler();
  if (!S_ISREG(info.st_mode)) {
    fprintf(stderr, "Error: Is a %s\n", get_file_type(info.st_mode & S_IFMT));
    return 0;
  }
  if (remove(arg) == -1) return error_handler();
  return 0;
}

int builtin_rmdir(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  if (rmdir(arg) == -1) return error_handler();
  return 0;
}

int builtin_stat(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  struct stat info;
  if (stat(arg, &info) == -1) return error_handler();
  char atime[N], mtime[N], ctime[N];
  strftime(atime, N, "%F %T %z", localtime(&info.st_atime));
  strftime(mtime, N, "%F %T %z", localtime(&info.st_mtime));
  strftime(ctime, N, "%F %T %z", localtime(&info.st_ctime));
  printf(
      "\
  File: %s\n\
  Size: %ld               Blocks: %ld          IO Block: %ld   %s\n\
Device: %lxh/%lud      Inode: %ld   Links: %lu\n\
Access: (%04o)  Uid: (%5u)   Gid: (%5u)\n\
Access: %s\n\
Modify: %s\n\
Change: %s\n",
      arg, info.st_size, info.st_blocks, info.st_blksize,
      get_file_type(info.st_mode & S_IFMT), info.st_dev, info.st_dev,
      info.st_ino, info.st_nlink, info.st_mode & 07777, info.st_uid,
      info.st_gid, atime, mtime, ctime);
  return 0;
}

int builtin_touch(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  if (open(arg, O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK, 0666) == -1 ||
      utimensat(AT_FDCWD, arg, NULL, 0) == -1)
    return error_handler();
  return 0;
}

int builtin_unmask(char *line) {
  char extra[N];
  int mode;
  if (sscanf(line, "%*s %o %s", &mode, extra) != 1) return -1;

  umask(mode);
  return 0;
}
