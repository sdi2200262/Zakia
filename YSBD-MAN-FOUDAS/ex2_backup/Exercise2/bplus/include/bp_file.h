#ifndef BP_FILE_H
#define BP_FILE_H

// Structure to store B+ tree file metadata
typedef struct {
    char filename[255];    // Filename of the B+ tree file
    int root_block;        // Block number of the root node
    int height;            // Height of the B+ tree
    int total_records;     // Total number of records in the tree
} BPLUS_INFO;

// Record structure (assuming this is defined elsewhere or in another header)
typedef struct {
    int id;                // Primary key
    char name[64];         // Example additional field
    // Add other fields as needed
} Record;

// Function prototypes for B+ tree file operations

// Create a new B+ tree file
// Returns 0 on success, -1 on failure
int BP_CreateFile(char *fileName);

// Open an existing B+ tree file
// Returns pointer to B+ tree metadata on success, NULL on failure
BPLUS_INFO* BP_OpenFile(char *fileName, int *file_desc);

// Close a B+ tree file
// Returns 0 on success, -1 on failure
int BP_CloseFile(int file_desc, BPLUS_INFO* info);

// Insert a record into the B+ tree
// Returns block number where record was inserted, or -1 on failure
int BP_InsertEntry(int file_desc, BPLUS_INFO* bplus_info, Record record);

// Retrieve a record from the B+ tree by ID
// Returns 0 on success, -1 on failure
// If found, sets *result to point to the record
int BP_GetEntry(int file_desc, BPLUS_INFO* header_info, int id, Record** result);

#endif // BP_FILE_H