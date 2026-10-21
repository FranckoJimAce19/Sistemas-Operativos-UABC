#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

// ls -l -h | head -1 |wc -c
int main(void) {
  int fd_pipe1[2];
  int fd_pipe2[2];
  int pid_ls, pid_head, pid_wc;
  char *ls[] = {"ls", "-lh", NULL};
  char *head[] = {"head", "-1", NULL};
  char *wc[] = {"wc", "-c", NULL};
  // se abre la primera pipa que se compartirá entre ls y head
  pipe(fd_pipe1);
  // Se crea el proceso que correrá ls
  pid_ls = fork();
  if (pid_ls == 0) {
    close(fd_pipe1[READ_END]);
    dup2(fd_pipe1[WRITE_END], STDOUT_FILENO);
    close(fd_pipe1[WRITE_END]);
    execvp(ls[0], ls);
  } else {
    wait(NULL);
    // se abre la segunda pipa que se compartirá entre head y wc
    pipe(fd_pipe2);
    // Se crea el proceso que realizará el head
    pid_head = fork();
    if (pid_head == 0) {
      close(fd_pipe1[WRITE_END]);
      dup2(fd_pipe1[READ_END], STDIN_FILENO);
      close(fd_pipe1[READ_END]);
      close(fd_pipe2[READ_END]);
      dup2(fd_pipe2[WRITE_END], STDOUT_FILENO);
      close(fd_pipe2[WRITE_END]);
      execvp(head[0], head);
    } else {
      close(fd_pipe2[WRITE_END]);
      wait(NULL);
      // Se crea el proceso que realizará el wc
      pid_wc = fork();
      if (pid_wc == 0) {
        close(fd_pipe1[READ_END]);
        close(fd_pipe1[WRITE_END]);
        close(fd_pipe2[WRITE_END]);
        dup2(fd_pipe2[READ_END], STDIN_FILENO);
        close(fd_pipe2[READ_END]);
        execvp(wc[0], wc);
      } else {
        wait(NULL);
        close(fd_pipe1[READ_END]);
        close(fd_pipe1[WRITE_END]);
        close(fd_pipe2[READ_END]);
        close(fd_pipe2[WRITE_END]);
        printf("\nMis hijos han terminado\n");
      }
    }
  }
  return 0;
}
