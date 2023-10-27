#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./nonleaf_process <directory_path> <pipe_write_end> \n");
        return 1;
    }
    //TODO(overview): fork the child processes(non-leaf process or leaf process) each associated with items under <directory_path>

    //TODO(step1): get <file_path> <pipe_write_end> from argv[]
    char *directory_path = argv[1];
    int pipe_write_end = atoi(argv[2]);

    //TODO(step2): malloc buffer for gathering all data transferred from child process as in root_process.c
    char *data_buffer = (char *) malloc(sizeof(char *) * 20);

    //TODO(step3): open directory
    DIR *dir = opendir(directory_path);
    if (dir == NULL) {
        perror("Failed to open directory\n");
        exit(1);
    }
    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process
    int fd[2];
    if (pipe(fd) == -1) {
        perror("Failed to create pipe\n");
        exit(-1);
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        pid_t child;
        child = fork();

        if (child == 0) { // child - needs to read
            close(fd[1]);
            close(pipe_write_end);

            char bytes_read[PATH_MAX];
            read(fd[0], bytes_read, sizeof(bytes_read)); // read from pipe to pass through execv

            if (entry->d_type == DT_DIR) { // check if directory
                dup2(fd[0], STDIN_FILENO); // give read end to child (?)

                char *child_arr[] = {"./nonleaf_process", directory_path, bytes_read, NULL};
                execv("./nonleaf_process", child_arr); // create a non-leaf process
            } else { // else entry is a file
                dup2(fd[0], STDIN_FILENO); // give read end to child (?)

                char *child_arr2[] = {"./leaf_process", directory_path, bytes_read, NULL};
                execv("./leaf_process", child_arr2); // create a leaf-process
            }

        } else { // parent - needs to write
            close(fd[0]);

            write(pipe_write_end, data_buffer, sizeof(data_buffer)); // write to pipe to pass through execv
        }
    }
    

    //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process
        
}
