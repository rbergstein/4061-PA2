#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/hash.h"
#include "../include/utils.h"

char *output_file_folder = "output/inter_submission/";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: Inter Submission --> ./leaf_process <file_path> 0\n");
        printf("Usage: Final Submission --> ./leaf_process <file_path> <pipe_write_end>\n");
        return -1;
    }
    //TODO(): get <file_path> <pipe_write_end> from argv[]
    char *file_path = argv[1];
    int pipe_write_end = atoi(argv[2]);

    //TODO(): create the hash of given file
    char file_hash[SHA256_BLOCK_SIZE * 2 + 1];
    hash_data_block(file_hash, file_path);

    //TODO(): construct string write to pipe. The format is "<file_path>|<hash_value>"
    int length = 2 * sizeof(file_path) + sizeof(file_hash);
    char message[length];
    sprintf(message, "%s|%s", file_path, file_hash);


    if () {
        //TODO(inter submission)
        //TODO(overview): create a file in output_file_folder("output/inter_submission/root*") and write the constructed string to the file
        //TODO(step1): extract the file_name from file_path using extract_filename() in utils.c
        //TODO(step2): extract the root directory(e.g. root1 or root2 or root3) from file_path using extract_root_directory() in utils.c
        //TODO(step3): get the location of the new file (e.g. "output/inter_submission/root1" or "output/inter_submission/root2" or "output/inter_submission/root3")
        //TODO(step4): create and write to file, and then close file
        //TODO(step5): free any arrays that are allocated using malloc!! Free the string returned from extract_root_directory()!! It is allocated using malloc in extract_root_directory()
        char file_name = extract_filename(file_path);
        char root_dir = extract_root_directory(file_path);

        char new_location[PATH_MAX];
        sprintf(new_location, "%s%s", output_file_folder, root_dir);

        FILE *fp = fopen(new_location, "w");

        if (fp == NULL) {
            printf("Cannot open file in leaf process\n");
            exit(-1);
        }

       fprintf(fp, "%s", file_hash);
       fclose(fp);

       free(root_dir);

    } else {
        //TODO(final submission): write the string to pipe

        exit(0);

    }

    exit(0);
}
