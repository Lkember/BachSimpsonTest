#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>


// Reads and summarizes a record
void readRecord(char record[]) {
    int recordNum;
    int aux;
    int dst;
    
    // Getting the record number
    recordNum = record[0] | record[1] << 8;
    
    // Getting the auxiliary flag and the Daylight Savings Time flag
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
    
    // Getting the timestamp
    // TODO: NEEDS TO BE TESTED, MAY NEED TO REVERSE ARRAY
    int timestamp = (record[3] << 24) | (record[4]<<16) | (record[5] << 8) | record[6];
    
    struct tm tim;
    char time[30];
    
    time_t t = (time_t)timestamp;
    tim = *localtime(&t);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S %Z", &tim);
    
    
    //Skip the next 3 bytes since they are spare bytes
    // The next bytes from 9 to 38 are ASCII characters (30 bytes)
    char message[30];
    
    int i;
    for (i=9; i<39; i++) {
        message[i-9] = record[i];
    }
    
    // Getting the checksum
    int checksum = record[39];
    
    printf("------------------------\nRecord %d\n------------------------\n", recordNum);
    
    if (aux == 1) {
        printf("Auxiliary = true\n");
    }
    else {
        printf("Auxiliary = false\n");
    }
    
    if (dst == 1) {
        printf("Daylight Savings Time = true\n");
    }
    else {
        printf("Daylight Savings Time = false\n");
    }
    
    //TODO: NEED TO ADD 30 YEARS TO THE TIME
    printf("%s\n\n", time);
    
    // TODO: NEED TO CHECK THE CHECKSUM
}


// Reads the file and summarizes it's parts
void readFile(FILE *file) {
    char numRecordsBuf[2];
    short int numRecords;
    
    // Reads the first 2 bytes (the number of records)
    fread(numRecordsBuf, 1, 2, file);
    numRecords = numRecordsBuf[0] | numRecordsBuf[1] << 8;
    
    // Skips the next 2 bytes (as they are spare bytes)
    fseek(file, 2, SEEK_CUR);
    
    // Reads each record
    char recordsBuf[40];
    
    int i;
    for (i = 0; i<numRecords; i++) {
        fread(recordsBuf, 1, 40, file);
        
        readRecord(recordsBuf);
    }
    
    
    // TODO: NEED TO CHECK THAT THE FILE IS VALID
    
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
