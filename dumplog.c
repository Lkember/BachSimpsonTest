#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

// Reads the file and summarizes it's parts
void readFile(FILE *file) {
    char numRecordsBuf[2];
    short int numRecords;
    
    // Reads the first 2 bytes (the number of records)
    fread(numRecordsBuf, 1, 2, file);
    
    printf("numRecordsBuf[0] = %d\n", numRecordsBuf[0]);
    printf("numRecordsBuf[1] = %d\n", numRecordsBuf[1]);
    
    numRecords = numRecordsBuf[0] | numRecordsBuf[1] << 8;
    printf("sizeOf int %lu\n", sizeof(numRecords));
    printf("sizeOf buf %lu\n", sizeof(numRecordsBuf));
    printf("numRecords = %d\n", numRecords);
    
    
    // Skips the next 2 bytes (as they are spare bytes)
    fseek(file, 2, SEEK_CUR);
    
    // Reads each record
    char recordsBuf[40];
    
    int i;
    for (i = 0; i<numRecords; i++) {
        fread(recordsBuf, 1, 40, file);
        
        readRecord(recordsBuf);
        
        // TODO: Need to look at each byte of the records buffer and print out information
    }
    
    
    
}

// Reads and summarizes a record
void readRecord(char record[]) {
    int recordNum;
    int aux;
    int dst;
    
    // Gets the record number
    recordNum = record[0] | record[1] << 8;
    
    // Gets the auxiliary flag and the Daylight Savings Time flag
    if ((int)record[2] == 3) {
        aux = 1;
        dst = 1;
    }
    else if ((int)record[2] == 1) {
        aux = 1;
        dst = 0;
    }
    else if ((int)record[2] == 2) {
        aux = 0;
        dst = 1;
    }
    else {
        aux = 0;
        dst = 0;
    }
    
    // Gets the timestamp
    
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
