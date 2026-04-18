#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void print_usage() {
    printf("Usage:\n");
    printf("  ./engine run <id> <rootfs> <command>\n");
}

int cmd_run(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: ./engine run <id> <rootfs> <command>\n");
        return 1;
    }

    char *id = argv[2];
    char *rootfs = argv[3];
    char *command = argv[4];

    pid_t pid = fork();

    if (pid == 0) {
        // Child process

        printf("Starting container: %s\n", id);

        if (chroot(rootfs) != 0) {
            perror("chroot failed");
            exit(1);
        }

        if (chdir("/") != 0) {
            perror("chdir failed");
            exit(1);
        }

        char *cmd[] = {command, NULL};
        execvp(command, cmd);

        perror("exec failed");
        exit(1);
    } 
    else if (pid > 0) {
        // Parent
        wait(NULL);
        printf("Container %s exited\n", id);
    } 
    else {
        perror("fork failed");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "run") == 0) {
        return cmd_run(argc, argv);
    }

    print_usage();
    return 1;
}
