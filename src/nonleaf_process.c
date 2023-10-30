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
   
    char *final_buffer = (char *)malloc(4098 * sizeof(char));

    //TODO(step3): open directory
    DIR *dir = opendir(directory_path);
    if (dir == NULL) {
        perror("Failed to open directory\n");
        exit(1);
    }
    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process
  
    struct dirent* entry;
    char read_ends[4098];
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

        if (child == 0) { // child - needs to read
       
            char write_buf[4098];
            memset(write_buf, 0, (sizeof(write_buf)));

            if (entry->d_type == DT_DIR) { // check if directory
                close(fd[0]);
                
                char final_path[4098];
                memset(final_path, 0, (sizeof(final_path)));
                sprintf(final_path, "%s/%s", directory_path, entry->d_name);
                sprintf(write_buf, "%d", fd[1]); //turning write end to string

                char *child_arr[] = {"./nonleaf_process", final_path, write_buf, NULL};
                if (execv("./nonleaf_process", child_arr) == -1) {
                    printf("Error in exec %d", errno);
                    exit(-1);
                }
    
            } else if (entry->d_type == DT_REG){ // else entry is a file
                close(fd[0]);
                
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
            }
           
        }
       
        close(fd[1]);       
  
    }
    while (wait(NULL) > 0);

    char bytes_read[4098];
    memset(bytes_read, 0, (sizeof(bytes_read)));
    int nbytes;
    //char *final_buffer = (char *)malloc(4098 * sizeof(char));

    for (int i=0; i < counter; i++) {
        while((nbytes = read(read_ends[i], bytes_read, sizeof(bytes_read))) != 0) {
            strncat(final_buffer, bytes_read, nbytes);
            //printf("byte read: %s\n", bytes_read);
            memset(bytes_read, 0, (sizeof(bytes_read)));

            // printf("final buffer--------------: %s\n", final_buffer);
        };
        close(read_ends[i]);
        //printf("final buffer--------------: %s\n", final_buffer);
    }
    
    write(pipe_write_end, final_buffer, strlen(final_buffer));
    closedir(dir);
    //printf("final buffer--------------: %s\n", final_buffer);
    close(pipe_write_end);
    free(final_buffer);
    //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process
        
}
