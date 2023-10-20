# 4061-PA2

## PA Group 98 members

Ryan Bergstein - bergs643

Marwa Osman - osman320

Basma Elaraby - elara006

## CSE computer used

csel-kh1250-02.cselabs.umn.edu

## Changes to Makefile

Added to the inter: piece of the Makefile so that the correct output/inter_submission/root# directories as well as the output/final_submission directory match with what is expected.

## Individual Contributions: 

Ryan: Created a group github repository to collectively work out of and set up the project files and README document. Worked alongside group members to create and finalize leaf_process.c. Reorganized the Makefile to create the correct output/inter_submission/root# directories.

Marwa: Worked on leaf_process, specifically extracting root and file names and writing hash to new file. Helped group members debug and finalize leaf_process.c. 

Basma: Worked on final debugging as long as attending TA office hours for extra help, wrote the plan for completing the rest of the project

## Plan for how you are going to construct the pipes and inter-process communication. (high-level pseudocode would be acceptable/preferred for this part)

root_process.c:

The root_process.c primarily functions to manage the communication between processes, extract file hashes, and take appropriate actions such as deleting duplicates or creating symbolic links.


1. Parsing Command-Line Inputs
Parse command line inputs


2. Pipe Setup
Construct pipe for communication with root directory non-leaf node


3. Forking Child Process
Fork child process 
if Child:
    Execute non_leaf.c
else:
    Continue in root_process.c


4. Reading Hash Strings
Read concatenated hash strings from the pipe


5. Parsing Hashes and Identifying Duplicates
Use parse_hash() to parse the string
Determine dup_list and retain_list variables


6. Handling Duplicates
For each file in dup_list:
    Delete file
    Create symbolic link to original


7. Outputting Results
Redirect standard output to an output file
Print details about symbolic links and paths


---

nonleaf_process.c

The nonleaf_process.c digs into the directory structures, separating files from directories. It also manages child processes for sub-directories and files.


1. Initializing Variables and Memory Allocation
char *dir_path = argv[1]
char* pwrite_end = argv[2]
Allocate memory for child_process_data


2. Opening Directory
Open directory using dir_path


3. Iterating Through Directory Entries
while reading each directory entry:
    Create a new pipe
    If entry is a directory:
        Fork child process
        Child executes nonleaf_process.c with dir entry
    Else if entry is a file:
        Fork child process
        Child executes leaf_process.c with file
    Close appropriate pipe ends


4. Waiting and Gathering Data
Wait for all child processes to finish
Collect data from reading ends of all child pipes


5. Writing Data Upstream
Write aggregated data to pwrite_end


6. Cleanup
Release dynamically allocated memory and close any open resources

