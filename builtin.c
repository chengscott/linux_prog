#include <dirent.h>
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

  FILE *file = fopen(arg, "r");
  // if (file == NULL)
  char *content = NULL;
  size_t len = 0;
  while (getline(&content, &len, file) != -1) {
    printf("%s", content);
    free(content);
  }
  fclose(file);
  return 0;
}

int builtin_cd(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  int ret = chdir(arg);
  // if (ret == -1)
  return 0;
}

int builtin_chmod(char *line) {
  char arg[N], extra[N];
  int mode;
  if (sscanf(line, "%*s %o %s %s", &mode, arg, extra) != 2) return -1;

  int ret = chmod(arg, mode);
  // if (ret == -1)
  return 0;
}

int builtin_echo(char *line) {
  char arg[N], filename[N], extra[N];
  int argc;
  if ((argc = sscanf(line, "%*s %s %s %s", arg, filename, extra)) > 2)
    return -1;

  if (argc == 2) {
    FILE *file = fopen(filename, "a+");
    // if (file == NULL)
    fprintf(file, "%s", arg);
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

  DIR *dir = opendir(argc == 1 ? arg : ".");
  struct dirent *dp;
  struct stat info;
  int ret;
  while ((dp = readdir(dir)) != NULL) {
    printf("%-20s", dp->d_name);
    ret = stat(dp->d_name, &info);
    // if (ret == -1) { ; continue; }
    printf("\t%-20s\t%-5ld\n", get_file_type(info.st_mode & S_IFMT),
           info.st_size);
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

  int ret = mkdir(arg, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // if (ret == -1)
  return 0;
}

int builtin_pwd(char *line) {
  char extra[N];
  if (sscanf(line, "%*s %s", extra) > 0) return -1;

  char name[N];
  getcwd(name, N);
  // if (name == NULL)
  printf("%s\n", name);
  return 0;
}

int builtin_rm(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  int ret = remove(arg);
  // if (ret == -1)
  return 0;
}

int builtin_rmdir(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  int ret = rmdir(arg);
  // if (ret == -1)
  return 0;
}

int builtin_stat(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  struct stat info;
  int ret = stat(arg, &info);
  // if (ret == -1)
  printf(
      "\
  File: %s\n\
  Size: %lld               Blocks: %lld          IO Block: %ld   %s\n\
Device: %s      Inode: %ld   Links: %d\n\
Access: (%lo)  Uid: (%ld)   Gid: (%ld)\n\
Access: %s\
Modify: %s\
Change: %s",
      arg, info.st_size, info.st_blocks, info.st_blksize,
      get_file_type(info.st_mode & S_IFMT),
      "",  // info.st_dev,
      info.st_ino, info.st_nlink, info.st_mode & 0777, info.st_uid, info.st_gid,
      ctime(&info.st_atime), ctime(&info.st_mtime), ctime(&info.st_ctime));
  return 0;
}

int builtin_touch(char *line) {
  char arg[N], extra[N];
  if (sscanf(line, "%*s %s %s", arg, extra) != 1) return -1;

  int fd = open(arg, O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK, 0666);
  // if (fd < 0)
  int rc = utimensat(AT_FDCWD, arg, NULL, 0);
  // if (rc)
  return 0;
}

int builtin_unmask(char *line) {
  char extra[N];
  int mode;
  if (sscanf(line, "%*s %o %s", &mode, extra) != 1) return -1;

  umask(mode);
  return 0;
}
