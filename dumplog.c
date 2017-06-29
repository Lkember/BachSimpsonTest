#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <inttypes.h>


// reads and summarizes files
void readRecord(FILE *file) {
    uint16_t recordNum;         // Used to hold the record number
    uint8_t auxdstByte;         // Holds the byte that determines auxiliary flag and dst
    int aux;
    int dst;
    int epochTimestamp = 946684800;     //The amount of seconds from 1970-2000
    
    // Reads the record number
    fread(&recordNum, sizeof(recordNum), 1, file);
    
    
    // Reads the auxiliary flag and dst byte
    fread(&auxdstByte, sizeof(auxdstByte), 1, file);
    
    
    // Getting the auxiliary flag and the Daylight Savings Time flag
    if (auxdstByte == 0b11000000) {
        aux = 1;
        dst = 1;
    }
    else if (auxdstByte == 0b10000000) {
        aux = 1;
        dst = 0;
    }
    else if (auxdstByte == 0b01000000) {
        aux = 0;
        dst = 1;
    }
    else {
        aux = 0;
        dst = 0;
    }
    
    
    // Getting the timestamp
    uint32_t timestamp;
    fread(&timestamp, sizeof(timestamp), 1, file);
    fseek(file, 2, SEEK_CUR);                       // the next 2 bytes are spare
    
    timestamp += epochTimestamp;
    
    struct tm timeStruct;
    char *officialTime;
    
    time_t time = (time_t)timestamp;
    timeStruct = *localtime(&time);
    
    officialTime = asctime(&timeStruct);
    
    
    // Getting the message
    char message[30];
    fread(&message, sizeof(message), 1, file);
    
    
    //Getting the checksum
    uint8_t checksum;
    fread(&checksum, 1, 1, file);
    
    
    // Displaying the data
    printf("------------------------\nRecord %d\n------------------------\n", recordNum);
    if (aux == 1) {
        
        if (aux == 1) {
            printf("Auxiliary = true\n");
        }
        else {
            printf("Auxiliary = false\n");
        }
        
        printf("%s", officialTime);
        if (dst == 1) {
            printf("Daylight Savings Time = true\n");
        }
        else {
            printf("Daylight Savings Time = false\n");
        }
        printf("Message:\n  %s\n", message);
        
        // TODO: NEED TO CHECK THE CHECKSUM
        printf("Checksum: %" PRIu8 "\n\n", checksum);
    }
    else {
        printf("%s", officialTime);
        if (dst == 1) {
            printf("Daylight Savings Time = true\n");
        }
        else {
            printf("Daylight Savings Time = false\n");
        }
        printf("Message:\n  %s\n\n", message);
    }
}


// Reads the file and summarizes it's parts
void readFile(FILE *file) {
//    char numRecordsBuf[2];
    uint16_t numRecords;
    
    // Reads the first 2 bytes (the number of records)
//    fread(&numRecords, sizeof(numRecords), 1, file);
//    numRecords = numRecordsBuf[0] | numRecordsBuf[1] << 8;
    fread(&numRecords, sizeof(numRecords), 1, file);
    
    // Skips the next 2 bytes (as they are spare bytes)
    fseek(file, 2, SEEK_CUR);
    
    // Reads each record
//    char recordsBuf[40];
    
    int i;
    for (i = 0; i<numRecords; i++) {
//        fread(recordsBuf, 1, 40, file);
//        readRecord(recordsBuf);
        
        readRecord(file);
    }
    
    
    // TODO: NEED TO CHECK THAT THE FILE IS VALID
    
}

int main(int argc, char* argv[]) {
    
    // Checks to see that there are two arguments, the first being -f
    if (argc != 3) {
        printf("Usage: -f FILENAME.EXTENSION\n");
        exit(EXIT_FAILURE);
    }
    
    printf("%d\n", strcmp(argv[1], "-f"));
    
    if (strcmp(argv[1], "-f")) {
        printf("Usage: -f FILENAME.EXTENSION\n");
        exit(EXIT_FAILURE);
    }
    
    char *filename;
    FILE *file;
    
    filename = argv[2];
    file = fopen(filename, "rb");
    
    // File could not be found, exiting
    if (file == NULL) {
        printf("Could not find file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    readFile(file);
    
    fclose(file);
}
