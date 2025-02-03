#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "Usage: %s <command> <args>\n", argv[0]);
        exit(1);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error creating pipe");
        exit(1);
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("Error creating fork");
        exit(1);
    }

    if (pid == 0) {
        struct timeval start_time;
        gettimeofday(&start_time, NULL);

        if (write(pipefd[1], &start_time, sizeof(struct timeval)) == -1) {
            perror("Error writing to pipe");
            exit(1);
        }

        execvp(argv[1], &argv[1]);
        exit(1);
    } else {
        struct timeval start_time;
        if (read(pipefd[0], &start_time,sizeof(struct timeval)) == -1) {
            perror("Error reading from pipe");
            exit(1);
        }

        wait(NULL);

        struct timeval end_time;
        gettimeofday(&end_time, NULL);

        double elapsed_time = (end_time.tv_sec-start_time.tv_sec) + (end_time.tv_usec-start_time.tv_usec)/1000000.0;

        printf("Elapsed time: %.5f ms\n", elapsed_time);
    }

    return 0;
}
