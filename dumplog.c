#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

// Reads the file and summarizes it's parts
void readFile(FILE *file) {
    char numRecordsBuf[2];
    short int numRecords;
    
    // Reads the first 2 bytes (the number of records)
    fread(numRecordsBuf, 1, 2, file);
    
    numRecords = numRecordsBuf[0] | numRecordsBuf[1] << 8;
    printf("sizeOf int %lu", sizeof(numRecords));
    printf("numRecords = %d\n", numRecords);
    
    
    // Skips the next 2 bytes (as they are spare bytes)
    fseek(file, 2, SEEK_CUR);
    
    // Reads each record
    char recordsBuf[40];
    
}

int main(int argc, char* argv[]) {
    
    // Checks to see that there are two arguments, the first being -f
    if (argc != 3) {
        printf("Usage: -f FILENAME.EXTENSION\n");
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(argv[1], "-f")) {
        printf("Usage: -f FILENAME.EXTENSION\n");
        exit(EXIT_FAILURE);
    }
    
    char *filename;
    FILE *file;
    
    filename = argv[2];
    file = fopen(filename, "r");
    
    // File could not be found, exiting
    if (file == NULL) {
        printf("Could not find file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    readFile(file);
    
    fclose(file);
}
