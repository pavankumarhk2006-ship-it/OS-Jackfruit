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

        // 🔥 Logging (NEW)
        freopen("container.log", "a", stdout);
        freopen("container.log", "a", stderr);

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
        // Parent waits
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

    if (strcmp(argv[1], "run") == 0)
        return cmd_run(argc, argv);

    if (strcmp(argv[1], "start") == 0)
        return cmd_start(argc, argv);

    if (strcmp(argv[1], "ps") == 0)
        return cmd_ps();

    if (strcmp(argv[1], "stop") == 0)
        return cmd_stop(argc, argv);

    print_usage();
    return 1;
}
// -------- START COMMAND --------
int cmd_start(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: ./engine start <id> <rootfs> <command>\n");
        return 1;
    }

    char *id = argv[2];
    char *rootfs = argv[3];
    char *command = argv[4];

    pid_t pid = fork();

    if (pid == 0) {
        printf("Starting container (background): %s\n", id);

        if (chroot(rootfs) != 0) {
            perror("chroot failed");
            exit(1);
        }

        chdir("/");

        char *cmd[] = {command, NULL};
        execvp(command, cmd);

        perror("exec failed");
        exit(1);
    } 
    else if (pid > 0) {
        printf("Container %s started with PID %d\n", id, pid);
        return 0;
    } 
    else {
        perror("fork failed");
        return 1;
    }
}


// -------- PS COMMAND --------
int cmd_ps() {
    printf("Running containers:\n");
    system("ps -ef | grep -E 'cpu_hog|memory_hog' | grep -v grep");
    return 0;
}


// -------- STOP COMMAND --------
int cmd_stop(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./engine stop <process_name>\n");
        return 1;
    }

    char cmd[100];
    sprintf(cmd, "pkill -f %s", argv[2]);
    system(cmd);

    printf("Stopped container process: %s\n", argv[2]);
    return 0;
}
