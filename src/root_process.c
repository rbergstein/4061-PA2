#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/utils.h"
#include "../include/sha256.h"
#define WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define PERM (S_IRUSR | S_IWUSR)
char *output_file_folder = "output/final_submission/";
void redirection(char **dup_list, int size, char* root_dir){
    // TODO(overview): redirect standard output to an output file in output_file_folder("output/final_submission/")
    
    // TODO(step1): determine the filename based on root_dir. e.g. if root_dir is "./root_directories/root1", the output file's name should be "root1.txt"
  
    char full_path[1024];
    char *root = extract_filename(root_dir);
    sprintf(full_path, "%s%s.txt", output_file_folder, root);

    int fp = open(full_path, WRITE, PERM);
    if (fp == -1){
        perror("Failed to open file\n");
        exit(-1); 
    }
    if (dup2(fp, STDOUT_FILENO) == -1){
        perror("Failed to redirect output\n");
        exit(-1);
    }

    close(fp);
    //TODO(step3): read the content each symbolic link in dup_list, write the path as well as the content of symbolic link to output file(as shown in expected)
    for (int i=0; i < size; i++) {
        char buf[PATH_MAX];
        
        ssize_t retained_path = readlink(dup_list[i], buf, sizeof(buf)-1); // prof suggested to use a linux func to get path on piazza, not sure if readlink is right
        if (retained_path == -1) {
            perror("Failed to read symbolic link\n");
            exit(-1);
        }
        buf[retained_path] = '\0'; 
        printf("[<path of symbolic link> --> <path of retained file>] : [%s --> %s]\n", dup_list[i], buf); 
        fflush(stdout);
    
    }
    
}
void create_symlinks(char **dup_list, char **retain_list, int size) {
    //TODO(): create symbolic link at the location of deleted duplicate file
    //TODO(): dup_list[i] will be the symbolic link for retain_list[i]
    for (int i = 0; i < size; i++) {
            if (symlink(retain_list[i], dup_list[i]) == -1) {
                perror("Failed to create a symbolic link\n");
                exit(-1);
            }
        }
}
void delete_duplicate_files(char **dup_list, int size) {
    //TODO(): delete duplicate files, each element in dup_list is the path of the duplicate file
    for (int i = 0; i < size; i++) {
        if (remove(dup_list[i]) == -1) {
            perror("Failed to delete duplicate file\n");
            exit(-1);
        }
    }
}
// ./root_directories <directory>
int main(int argc, char* argv[]) {
    if (argc != 2) {
        // dir is the root_directories directory to start with
        // e.g. ./root_directories/root1
        printf("Usage: ./root <dir> \n");
        return 1;
    }

    //TODO(overview): fork the first non_leaf process associated with root directory("./root_directories/root*")

    char* root_directory = argv[1];
    //printf("Root directory: %s\n", root_directory);

    char all_filepath_hashvalue[4098]; //buffer for gathering all data transferred from child process
    memset(all_filepath_hashvalue, 0, sizeof(all_filepath_hashvalue));// clean the buffer

    //TODO(step1): construct pipe
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1){
            printf("Error creating pipe...\n");
            exit(-1);
    }
    //TODO(step2): fork() child process & read data from pipe to all_filepath_hashvalue
    pid_t pid;
    pid = fork();
    if (pid != 0) {
        wait(NULL);
        int nbytes;
        char bytes_read[4098];
        memset(bytes_read, 0, sizeof(bytes_read));
        close(fd[1]);
  
        while((nbytes = read(fd[0], bytes_read, sizeof(bytes_read))) != 0) { 
                strncat(all_filepath_hashvalue, bytes_read, nbytes);
                memset(bytes_read, 0, sizeof(bytes_read));
        }

        close(fd[0]);
    }

    else {
        close(fd[0]);
        char write_buf[4098];
        memset(write_buf, 0, sizeof(write_buf));
        sprintf(write_buf, "%d", fd[1]); //turning write end to string

        char *arr[] = {"./nonleaf_process", root_directory, write_buf, NULL};
        execv("./nonleaf_process", arr);

        close(fd[1]);

    }
    //TODO(step3): malloc dup_list and retain list & use parse_hash() in utils.c to parse all_filepath_hashvalue
    // dup_list: list of paths of duplicate files. We need to delete the files and create symbolic links at the location
    // retain_list: list of paths of unique files. We will create symbolic links for those files

    char** dup_list = malloc(10 * sizeof(char*));
    char** retain_list = malloc(10 * sizeof(char*));

    if (!dup_list || !retain_list) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    
    // for (int i = 0; i < 10; i++) {
    //     dup_list[i] = NULL;
    //     retain_list[i] = NULL;
    // }
    //printf("All filepath hash: %s\n", all_filepath_hashvalue);
    int size = parse_hash(all_filepath_hashvalue, dup_list, retain_list);
    //printf("Size: %d\n", size);

    // Print the results for verification (optional)
    // printf("Duplicate files:\n");
    // for (int i = 0; i < size; i++) {
    //     printf("%s\n", dup_list[i]);
    // }

    // printf("\nRetained files:\n");
    // for (int i = 0; i < size; i++) {
    //     printf("%s\n", retain_list[i]);
    // }

    //TODO(step4): implement the functions
    delete_duplicate_files(dup_list,size);
    create_symlinks(dup_list, retain_list, size);
    redirection(dup_list, size, argv[1]);
    //TODO(step5): free any arrays that are allocated using malloc!!

    for (int i = 0; i < 10; i++) {
        dup_list[i] = NULL;
        retain_list[i] = NULL;
    }
    free(dup_list);
    free(retain_list);
    

    return 0;
}
