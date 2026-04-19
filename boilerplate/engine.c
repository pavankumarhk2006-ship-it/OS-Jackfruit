#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// ---------------- RUN ----------------
int cmd_run(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: ./engine run <id> <rootfs> <command>\n");
        return 1;
    }

    char *id = argv[2];
    char *rootfs = argv[3];
    char *command = argv[4];

    // Remove '/' from command
    char *proc = command[0] == '/' ? command + 1 : command;

    // 🔥 Prevent duplicate
    char check_cmd[256];
    snprintf(check_cmd, sizeof(check_cmd), "pgrep -x %s > /dev/null", proc);

    if (system(check_cmd) == 0) {
        printf("[ERROR] Container already running!\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child

        char logname[128];
        snprintf(logname, sizeof(logname), "%s.log", id);

        freopen(logname, "a", stdout);
        freopen(logname, "a", stderr);

        printf("[LOG] Starting container: %s\n", id);
        fflush(stdout);

        if (chroot(rootfs) != 0) {
            perror("chroot failed");
            exit(1);
        }

        if (chdir("/") != 0) {
            perror("chdir failed");
            exit(1);
        }

        printf("[LOG] Inside container %s (PID: %d)\n", id, getpid());
        fflush(stdout);

        char *cmd[] = {command, NULL};
        execvp(cmd[0], cmd);

        perror("exec failed");
        exit(1);

    } else if (pid > 0) {
        printf("Container %s started with PID %d\n", id, pid);

        // Save container info
        FILE *f = fopen("containers.db", "a");
        if (f) {
            fprintf(f, "%s %d\n", id, pid);
            fclose(f);
        }

        return 0;

    } else {
        perror("fork failed");
        return 1;
    }
}

// ---------------- START ----------------
int cmd_start(int argc, char *argv[]) {
    return cmd_run(argc, argv);
}

// ---------------- PS ----------------
int cmd_ps() {
    printf("\n=============================\n");
    printf("   Running Containers\n");
    printf("=============================\n");

    FILE *f = fopen("containers.db", "r");
    if (!f) {
        printf("No containers found\n");
        return 0;
    }

    char id[64];
    int pid;

    printf("ID\tPID\n");
    printf("-----------------------------\n");

    while (fscanf(f, "%s %d", id, &pid) == 2) {
        // check if process is alive
        if (kill(pid, 0) == 0) {
            printf("%s\t%d\n", id, pid);
        }
    }

    fclose(f);
    printf("=============================\n\n");

    return 0;
}

// ---------------- STOP ----------------
int cmd_stop(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: ./engine stop <process_name>\n");
        return 1;
    }

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "pkill -x %s", argv[2]);

    if (system(cmd) != 0) {
        printf("[WARN] No such process running\n");
    } else {
        printf("[LOG] Stopped: %s\n", argv[2]);
    }

    return 0;
}

// ---------------- RESTART ----------------
int cmd_restart(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: ./engine restart <id> <rootfs> <command>\n");
        return 1;
    }

    printf("[LOG] Restarting container: %s\n", argv[2]);

    char *proc = argv[4][0] == '/' ? argv[4] + 1 : argv[4];

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "pkill -x %s", proc);
    system(cmd);

    sleep(1);

    return cmd_run(argc, argv);
}

// ---------------- MAIN ----------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  ./engine run <id> <rootfs> <command>\n");
        printf("  ./engine start <id> <rootfs> <command>\n");
        printf("  ./engine ps\n");
        printf("  ./engine stop <process_name>\n");
        printf("  ./engine restart <id> <rootfs> <command>\n");
        return 1;
    }

    if (strcmp(argv[1], "run") == 0) {
        return cmd_run(argc, argv);
    } else if (strcmp(argv[1], "start") == 0) {
        return cmd_start(argc, argv);
    } else if (strcmp(argv[1], "ps") == 0) {
        return cmd_ps();
    } else if (strcmp(argv[1], "stop") == 0) {
        return cmd_stop(argc, argv);
    } else if (strcmp(argv[1], "restart") == 0) {
        return cmd_restart(argc, argv);
    } else {
        printf("Unknown command\n");
        return 1;
    }
}
