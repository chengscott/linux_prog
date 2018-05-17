#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

int help(void) {
  const char *usage[] = {
      "hw2 - Process Finder",
      "usage: ./hw2 [-a] [-x] [-pqrs] [-t]",
      "",
      "-a Show processes from all users",
      "-x Show processes without associated terminal",
      "-p Sort by pid (default)",
      "-q Sort by ppid",
      "-r Sort by pgid",
      "-s Sort by sid",
      "-t Show processes in tree view",
  };
  const size_t len = sizeof(usage) / sizeof(char *);
  for (size_t i = 0; i < len; ++i) printf("%s\n", usage[i]);
  return 0;
}

// process table
struct PT {
  char St, img[256], img_end[10], cmd[4096];
  int pid, ppid, pgid, sid, uid, gid, tty, ttyno;
} pt[32768];
size_t pts = 0;

// compare function
int cmp_ppid(const void *lhs, const void *rhs) {
  return ((struct PT *)lhs)->ppid > ((struct PT *)rhs)->ppid;
}
int cmp_pgid(const void *lhs, const void *rhs) {
  return ((struct PT *)lhs)->pgid > ((struct PT *)rhs)->pgid;
}
int cmp_sid(const void *lhs, const void *rhs) {
  return ((struct PT *)lhs)->sid > ((struct PT *)rhs)->sid;
}
int (*cmp[3])(const void *, const void *) = {cmp_ppid, cmp_pgid, cmp_sid};

// tty
int tty[1024] = {0};
size_t ttyds, ttyns;

int starts_with(const char *lhs, const char *rhs) {
  const char *path = lhs, *prefix = rhs;
  while (*prefix) {
    if (*prefix++ != *path++) return 0;
  }
  return 1;
}

void search_device(const char *path, int exact, char tty_name[32][1024],
                   char tty_drivers[1024][1024]) {
  DIR *devdir = opendir(path);
  if (devdir == NULL) return;
  struct dirent *dev;
  struct stat buf;
  int mj, mn;
  while ((dev = readdir(devdir)) != NULL) {
    sprintf(tty_name[ttyns], "%s/%s", path, dev->d_name);
    stat(tty_name[ttyns], &buf);
    if (!(S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode))) continue;
    for (size_t i = 0; i < ttyds; ++i) {
      if (exact || starts_with(tty_name[ttyns], tty_drivers[i])) {
        mj = major(buf.st_rdev), mn = minor(buf.st_rdev);
        tty[ttyns++] = ((mn << 8) & 0xff0000) | (mj << 8) | (mn & 0xff);
        break;
      }
    }
  }
  closedir(devdir);
}

// output in tree view
int pa[32768];
int find(int x) {
  return (pa[x] == x || pa[x] == -1) ? x : (pa[x] = find(pa[x]));
}

void tree_view(int ppid, int depth) {
  for (size_t i = 0; i < pts; ++i) {
    if (pt[i].ppid == ppid) {
      for (size_t j = 0; j < depth; ++j) printf(" ");
      printf("`-");
      printf("%s%s\n", pt[i].img, pt[i].img_end + 1);
      tree_view(pt[i].pid, depth + 2);
    }
  }
}

int main(int argc, char **argv) {
  char tty_drivers[32][1024], tty_name[1024][1024] = {"------"};
  // input option
  char opt_all_user = 0,  // Show processes from all users
      opt_term = 1,       // Show processes with associted terminal
      opt_sort_by = 0, opt_tree_view = 0;
  for (int i = 1; i < argc; ++i) {
    char c;
    if (argv[i][0] == '-' && (c = argv[i][1]) != '\0' && argv[i][2] == '\0') {
      if (c == 'a')
        opt_all_user = 1;
      else if (c == 'x')
        opt_term = 0;
      else if (c == 'p' || c == 'q' || c == 'r' || c == 's')
        opt_sort_by = c - 'p';
      else if (c == 't')
        opt_tree_view = 1;
      else
        return help();
    } else
      return help();
  }
  // tty drivers
  FILE *file;
  ttyds = 0;
  file = fopen("/proc/tty/drivers", "r");
  while (fscanf(file, "%*s %s %*s %*s %*s", tty_drivers[ttyds]) != EOF) ++ttyds;
  fclose(file);
  ttyns = 1;
  search_device("/dev", 0, tty_name, tty_drivers);
  for (size_t i = 0; i < ttyds; ++i)
    search_device(tty_drivers[i], 1, tty_name, tty_drivers);
  // retrieve procfs
  const int EUID = geteuid();
  DIR *procfs = opendir("/proc");
  // if (procfs == NULL) return 1;
  struct dirent *proc;
  struct PT *p;
  char path[4096];
  int euid, egid, fd;
  while ((proc = readdir(procfs)) != NULL) {
    if (!(*proc->d_name > '0' && *proc->d_name <= '9')) continue;
    // parse /proc/{pid:d}/status
    sprintf(path, "/proc/%s/status", proc->d_name);
    file = fopen(path, "r");
    if (file == NULL) continue;
    fscanf(file,
           "%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s"
           "%*s%*s%d%*s%*s%*s%*s%d%*s",
           &euid, &egid);
    fclose(file);
    // filter opt_all_user
    if (opt_all_user == 0 && euid != EUID) continue;
    p = pt + pts++;
    p->uid = euid, p->gid = egid;
    // parse /proc/{pid:d}/stat
    sprintf(path, "/proc/%s/stat", proc->d_name);
    file = fopen(path, "r");
    if (file == NULL) {
      --pts;
      continue;
    }
    fscanf(file, "%d (%[^)] %[)] %c %d %d %d %u %*s", &p->pid, p->img,
           p->img_end, &p->St, &p->ppid, &p->pgid, &p->sid, &p->tty);
    fclose(file);
    // filter opt_term
    if (opt_term == 1 && p->tty == 0) {
      --pts;
      continue;
    }
    // parse tty
    for (size_t i = 0; i < ttyns; ++i) {
      if (tty[i] == p->tty) {
        p->ttyno = i;
        break;
      }
    }
    // parse /proc/{pid:d}/cmdline
    sprintf(path, "/proc/%s/cmdline", proc->d_name);
    fd = open(path, O_RDONLY);
    if (fd == -1) {
      --pts;
      continue;
    }
    read(fd, p->cmd, 4096);
    close(fd);
    for (size_t i = 0; i < 4096; ++i) {
      if (p->cmd[i] == '\0') {
        if (p->cmd[i + 1] == '\0')
          break;
        else
          p->cmd[i] = ' ';
      }
    }
  }
  closedir(procfs);
  // output
  if (opt_sort_by > 0)
    qsort((void *)pt, pts, sizeof(struct PT), cmp[opt_sort_by - 1]);
  if (opt_tree_view) {
    for (size_t i = 0; i < 32768; ++i) pa[i] = -1;
    for (size_t i = 0; i < pts; ++i) pa[pt[i].pid] = pt[i].ppid;
    for (size_t i = 0; i < pts; ++i) find(pt[i].pid);
    int upa[32768], upas = 0;
    for (size_t i = 0; i < pts; ++i) {
      int is_unique = 1, pai = pa[pt[i].pid];
      for (size_t j = 0; j < upas; ++j) {
        if (pai == upa[j]) {
          is_unique = 0;
          break;
        }
      }
      if (is_unique) {
        tree_view(pai, 0);
        upa[upas++] = pai;
      }
    }
  } else {
    printf("  pid   uid   gid  ppid  pgid   sid      tty St (img) cmd\n");
    p = pt;
    for (size_t i = 0; i < pts; ++i, ++p) {
      printf("%5d %5d %5d %5d %5d %5d %8s %2c (%s%s %s\n", p->pid, p->uid,
             p->gid, p->ppid, p->pgid, p->sid, tty_name[p->ttyno] + 5, p->St,
             p->img, p->img_end, p->cmd);
    }
  }
  return 0;
}
