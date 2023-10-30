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

Ryan: Created a group github repository to collectively work out of and set up the project files and README document. Worked alongside group members to create and finalize leaf_process.c. Reorganized the Makefile to create the correct output/inter_submission/root# directories. Collectively worked to write nonleaf_process.c and root_process.c with group mates. Also spent a lot of time debugging to try and find errors with writing to the pipes/files.

Marwa: Worked on leaf_process, specifically extracting root and file names and writing hash to new file. Helped group members debug and finalize leaf_process.c. Wrote logic for nonleaf_process.c and wrote logic for constructing and reading/writing from pipes in root_process.c. Attended office hours and worked with group members to debug and finalize the code. 

Basma: Worked on final debugging as long as attending TA office hours for extra help, wrote the plan for completing the rest of the project. Consulted various online resources and engaged in in-person discussions to gather inspiration and ensure comprehensive documentation.

## Plan for how you are going to construct the pipes and inter-process communication

root_process.c:

The root_process.c primarily functions to manage the communication between processes, extract file hashes, and take appropriate actions such as deleting duplicates or creating symbolic links.

1. Parse command line inputs

2. Construct pipe for communication with root directory non-leaf node

3. Forking Child Process
Fork child process 
if Child:
    Execute non_leaf.c
else:
    Continue in root_process.c

4. Read concatenated hash strings from the pipe

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

2. Opening Directory

3. Iterating Through Directory Entries

4. Wait for all child processes to finish
Collect data from reading ends of all child pipes

5. Write aggregated data to pwrite_end

6. Free dynamically allocated memory and close any open files
