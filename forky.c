#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void create_processes(int num_processes, int pattern);
void create_dynamic_hierarchy(int parent_id, int child_id, int max_count);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_processes> <pattern>\n", argv[0]);
        return 1;
    }

    int num_processes = atoi(argv[1]);
    int pattern = atoi(argv[2]);

    if (num_processes < 1 || num_processes > 256) {
        fprintf(stderr, "Number of processes should be between 1 and 256.\n");
        return 1;
    }
    if (pattern < 1 || pattern > 3) {
        fprintf(stderr, "Pattern number should be 1, 2, or 3.\n");
        return 1;
    }

    create_processes(num_processes, pattern);
    return 0;
}

void create_processes(int num_processes, int pattern) {
    int i;
    pid_t pid;

    if (pattern == 1) {
        // Pattern 1: Fork all processes independently, then wait for them
        for (i = 1; i <= num_processes; i++) {
            pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(1);
            } else if (pid == 0) {
                srand(time(NULL) ^ (getpid() << 16));
                int sleep_time = (rand() % 8) + 1;
                printf("Process %d (PID: %d) beginning\n", i, getpid());
                sleep(sleep_time);
                printf("Process %d (PID: %d) exiting\n", i, getpid());
                exit(0);
            } else {
                printf("Main parent (PID: %d) created Process %d (PID: %d)\n", getpid(), i, pid);
            }
        }

        // Main parent waits for all child processes to complete
        for (i = 1; i <= num_processes; i++) {
            wait(NULL);
        }
    } else if (pattern == 2) {
        // Pattern 2: Chain-like creation (main -> 1 -> 2 -> ... -> num_processes)
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Start the chain from Process 1
            for (int i = 1; i <= num_processes; i++) {
                printf("Process %d (PID: %d) beginning\n", i, getpid());
                
                if (i < num_processes) {
                    // Only create a child if we are not at the last process
                    printf("Process %d (PID: %d) creating Process %d\n", i, getpid(), i + 1);
                    pid = fork();
                    if (pid < 0) {
                        perror("Fork failed");
                        exit(1);
                    } else if (pid > 0) {
                        // Parent process waits for its child and then exits
                        wait(NULL);
                        printf("Process %d (PID: %d) exiting\n", i, getpid());
                        exit(0);
                    }
                }
                
                // Only the last process reaches here to exit its loop
                sleep((rand() % 8) + 1);  // Optional sleep for the last process
            }
        } else {
            // Main parent waits for the first child (Process 1) to complete
            wait(NULL);
            printf("Main parent (PID: %d) exiting\n", getpid());
        }
    } else if (pattern == 3) {
        // Pattern 3: Dynamic hierarchy with binary-tree-like structure
        printf("Main parent (PID: %d) creating Process 1\n", getpid());
        
        // Start from process ID 1
        create_dynamic_hierarchy(1, 1, num_processes);
        
        printf("Main parent (PID: %d) exiting\n", getpid());
    }
}

void create_dynamic_hierarchy(int parent_id, int child_id, int max_count) {
    if (child_id > max_count) return;

    // Determine child IDs in a binary tree (2 * child_id) for left and (2 * child_id + 1) for right
    int left_child_id = 2 * child_id;
    int right_child_id = 2 * child_id + 1;

    if (left_child_id <= max_count) {
        pid_t pid1 = fork();
        if (pid1 < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid1 == 0) {
            // Left child process
            srand(time(NULL) ^ (getpid() << 16));
            printf("Process %d (PID: %d) beginning\n", left_child_id, getpid());
            sleep(rand() % 8 + 1);

            // Recursively create children for this process
            create_dynamic_hierarchy(left_child_id, left_child_id, max_count);

            printf("Process %d (PID: %d) exiting\n", left_child_id, getpid());
            exit(0);
        } else {
            printf("Process %d (PID: %d) created Process %d (PID: %d)\n", parent_id, getpid(), left_child_id, pid1);
        }
    }

    if (right_child_id <= max_count) {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid2 == 0) {
            // Right child process
            srand(time(NULL) ^ (getpid() << 16));
            printf("Process %d (PID: %d) beginning\n", right_child_id, getpid());
            sleep(rand() % 8 + 1);

            // Recursively create children for this process
            create_dynamic_hierarchy(right_child_id, right_child_id, max_count);

            printf("Process %d (PID: %d) exiting\n", right_child_id, getpid());
            exit(0);
        } else {
            printf("Process %d (PID: %d) created Process %d (PID: %d)\n", parent_id, getpid(), right_child_id, pid2);
        }
    }

    // Wait for both children to complete if they exist
    if (left_child_id <= max_count) wait(NULL);
    if (right_child_id <= max_count) wait(NULL);
}
