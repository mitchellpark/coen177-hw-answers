#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if(argc<2){
        fprintf(stderr, "Usage: %s <command> <args>\n", argv[0]);
        exit(1);
    }

    int shmid = shmget(IPC_PRIVATE, sizeof(struct timeval), IPC_CREAT | 0666);
    if(shmid<0) {
        perror("Error in shmget()");
        exit(1);
    }

    struct timeval *start_time = shmat(shmid, NULL, 0);
    if (start_time==(void *) -1) {
        perror("Error in shmat()");
        exit(1);
    }

    pid_t pid = fork();
    if(pid <0){
        perror("Error creating fork");
        exit(1);
    }

    if(pid==0){
        gettimeofday(start_time, NULL);

        execvp(argv[1], &argv[1]);
        exit(1);
    }else{
        wait(NULL);

        struct timeval end_time;
        gettimeofday(&end_time, NULL);

        double elapsed_time = (end_time.tv_sec - start_time->tv_sec) + (end_time.tv_usec - start_time->tv_usec) / 1000000.0;

        printf("Elapsed time: %.5f ms\n", elapsed_time);

        shmdt(start_time);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
