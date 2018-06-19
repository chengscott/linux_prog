#ifndef __LIBMINIC_H__
#define __LIBMINIC_H_
typedef unsigned int size_t;
typedef unsigned long sigset_t;
typedef struct {
    unsigned long long reg[8];
    sigset_t mask;
} jmp_buf[1];
typedef struct {
  int si_signo;
} siginfo_t;
struct sigaction {
  void (*sa_handler)(int);
//  void (*sa_sigaction)(int, siginfo_t *, void *);
  sigset_t sa_mask;
  unsigned long sa_flags;
  void (*sa_restorer)(void);
} __nact;
sigset_t __jmp_mask;

long write(unsigned int fd, const char *buf, size_t nbytes);
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
long sys_rt_sigaction(int, const struct sigaction *, struct sigaction *, int);
void sys_rt_sigreturn(void);
long sigaction(int signum, const struct sigaction *act, struct sigaction *oact) {
  __nact.sa_handler = act->sa_handler;
//  __nact.sa_sigaction = act->sa_sigaction;
  __nact.sa_mask = act->sa_mask;
  __nact.sa_flags = act->sa_flags | 0x04000000;//SA_RESTORER;
  __nact.sa_restorer = sys_rt_sigreturn;
  return sys_rt_sigaction(signum, &__nact, oact, sizeof(sigset_t));
}
long sigprocmask(int how, sigset_t *set, sigset_t *oset);
long alarm(unsigned int seconds);
long sys_pause(void); // direct syscall
long pause(void) { return sys_pause(); }
long sleep(int s);
long exit(int error_code);
#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGIOT 6
#define SIGBUS 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPOLL SIGIO
#define SIGPWR 30
#define SIGSYS 31
#define SIGUNUSED 31
#endif
