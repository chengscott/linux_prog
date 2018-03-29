int builtin_cat(char *);
int builtin_cd(char *);
int builtin_chmod(char *);
int builtin_echo(char *);
int builtin_exit(char *);
int builtin_find(char *);
int builtin_help(char *);
int builtin_id(char *);
int builtin_mkdir(char *);
int builtin_pwd(char *);
int builtin_rm(char *);
int builtin_rmdir(char *);
int builtin_stat(char *);
int builtin_touch(char *);
int builtin_unmask(char *);
const char *builtin_name[] = {"cat",  "cd",    "chmod", "echo",  "exit",
                              "find", "help",  "id",    "mkdir", "pwd",
                              "rm",   "rmdir", "stat",  "touch", "unmask"};
const int (*builtin_func[])(char *) = {
    &builtin_cat,   &builtin_cd,    &builtin_chmod, &builtin_echo,
    &builtin_exit,  &builtin_find,  &builtin_help,  &builtin_id,
    &builtin_mkdir, &builtin_pwd,   &builtin_rm,    &builtin_rmdir,
    &builtin_stat,  &builtin_touch, &builtin_unmask};
int set_euid_egid(int, int);
