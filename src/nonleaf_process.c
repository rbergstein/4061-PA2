#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./nonleaf_process <directory_path> <pipe_write_end> \n");
        return 1;
    }
    //TODO(overview): fork the child processes(non-leaf process or leaf process) each associated with items under <directory_path>

    //TODO(step1): get <file_path> <pipe_write_end> from argv[]
    char *directory_path = argv[1];
    // printf("Dir path: %s\n", directory_path);
    int pipe_write_end = atoi(argv[2]);

    //TODO(step2): malloc buffer for gathering all data transferred from child process as in root_process.c
    //char **child_data = (char **)malloc(sizeof(char *) * 20);
    //char *data_buffer = (char *) malloc(sizeof(char *) * 20);

    //TODO(step3): open directory
    //DIR *dir = opendir(file_path);
    DIR *dir = opendir(directory_path);
    if (dir == NULL) {
        perror("Failed to open directory\n");
        exit(1);
    }
    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process
  
    struct dirent* entry;
    char read_ends[100];
    int counter = 0;

    while ((entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }

        int fd[2];
        if (pipe(fd) == -1) {
            perror("Failed to create pipe\n");
            exit(-1);
        }
        read_ends[counter] = fd[0];
        counter++;

        pid_t child;
        child = fork();

        //if (child == 0) { // child
        if (child == 0) { // child - needs to read
            close(fd[1]);
            
            char final_path[PATH_MAX];
            char write_buf[PATH_MAX];

            sprintf(final_path, "%s/%s", directory_path, entry->d_name);
            sprintf(write_buf, "%d", fd[1]); //turning write end to string

            if (entry->d_type == DT_DIR) { // check if directory
                char *child_arr[] = {"./nonleaf_process", final_path, write_buf, NULL};
                if (execv("./nonleaf_process", child_arr) == -1) { // non-leaf process
                    printf("Error in exec %d", errno);
                    exit(-1);
                }

            } else { // else entry is a file    
                char *child_arr2[] = {"./leaf_process", final_path, write_buf, NULL};
                if (execv("./leaf_process", child_arr2) == -1) { // leaf process
                    printf("Error in exec %d", errno); 
                    exit(-1);
                }
            }

        } 
        close(fd[1]);       
    }

    closedir(dir);
    while (wait(NULL) > 0);

    // ssize_t bytes_read;
    // ssize_t pos;
    // char final_buffer[PATH_MAX];
    // memset(final_buffer, 0, sizeof(final_buffer));

    // for (int i=0; i < counter; i++) {
    //     while ((bytes_read = read(read_ends[i], final_buffer + pos, sizeof(final_buffer))) > 0) {
    //         pos += bytes_read;
    //     };
    //     close(read_ends[i]);
    // }
    // write(pipe_write_end, final_buffer, pos);

    char bytes_read[PATH_MAX];
    char final_buffer[PATH_MAX];
    memset(final_buffer, 0, sizeof(final_buffer));

    for (int i=0; i < counter; i++) {
        ssize_t bytes = 0;
        while ((bytes = read(read_ends[i], bytes_read, sizeof(bytes_read))) > 0) {
            strcat(final_buffer, bytes_read);  
        };
        close(read_ends[i]);

    }
    
    write(pipe_write_end, final_buffer, strlen(final_buffer));
    printf("  - final buffer: %s\n", final_buffer);
    close(pipe_write_end);

    //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process
        
}
