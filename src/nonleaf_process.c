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
    //char *file_path = argv[1];
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
            //close(fd[1]);
            //close(pipe_write_end);
            char write_buf[4098];
            memset(write_buf, 0, (sizeof(write_buf)));

            //char bytes_read[PATH_MAX];
            //read(fd[0], bytes_read, sizeof(bytes_read)); // read from pipe to pass through execv

            if (entry->d_type == DT_DIR) { // check if directory
                close(fd[0]);
                //dup2(fd[0], STDIN_FILENO); // give read end to child (?)

                //char *child_arr[] = {"./nonleaf_process", argv[1], argv[2], NULL};
                char final_path[4098];
                memset(final_path, 0, (sizeof(final_path)));
                sprintf(final_path, "%s/%s", directory_path, entry->d_name);
                sprintf(write_buf, "%d", fd[1]); //turning write end to string
                char *child_arr[] = {"./nonleaf_process", final_path, write_buf, NULL};
                //execv("./nonleaf_process", child_arr); // create a non-leaf process
                if (execv("./nonleaf_process", child_arr) == -1) {
                    printf("Error in exec %d", errno);
                    exit(-1);
                }
                //close(fd[1]);

            } else { // else entry is a file
                close(fd[0]);
                //dup2(fd[0], STDIN_FILENO); // give read end to child (?)

                //char *child_arr2[] = {"./leaf_process", argv[1], argv[2], NULL};
                char final_path[4098];
                memset(final_path, 0, (sizeof(final_path)));
                sprintf(final_path, "%s/%s", directory_path, entry->d_name);
                sprintf(write_buf, "%d", fd[1]); //turning write end to string
                printf("write buf: %s", write_buf);
                char *child_arr2[] = {"./leaf_process", final_path, write_buf, NULL};
                if (execv("./leaf_process", child_arr2) == -1) {
                    printf("Error in exec %d", errno);
                    exit(-1);
                } // create a leaf-process
                //close(fd[1]);
            }
           
        }
       
        close(fd[1]);       
        //} 
        // else { // parent - needs to write
        //     close(fd[0]);

        //     write(pipe_write_end, data_buffer, sizeof(data_buffer)); // write to pipe to pass through execv
        // }
    }
    //closedir(dir);
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

    char bytes_read[4098];
    memset(bytes_read, 0, (sizeof(bytes_read)));
    //char final_buffer[4098];
    int nbytes;
    char *final_buffer = (char *)malloc(4098 * sizeof(char));

    for (int i=0; i < counter; i++) {
        while((nbytes = read(read_ends[i], bytes_read, sizeof(bytes_read))) != 0) {
            strncat(final_buffer, bytes_read, nbytes);
            //printf("byte read: %s\n", bytes_read);
            memset(bytes_read, 0, (sizeof(bytes_read)));

            //printf("final buffer--------------: %s\n", final_buffer);
        };
        close(read_ends[i]);

    }
    
    write(pipe_write_end, final_buffer, strlen(final_buffer));
    closedir(dir);
    //printf("final buffer--------------: %s\n", final_buffer);
    close(pipe_write_end);

    //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process
        
}
