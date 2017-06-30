#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <inttypes.h>

// Sums all the bytes in the file looking for 255 or 0xFF
uint8_t fileChecksumCheck(FILE *file) {
    
    // Getting the size of the file and ignoring the last byte (the checksum)
    fseek(file, 0L, SEEK_END);
    int fileSize = ftell(file);
    
    rewind(file);
    
    uint8_t checksum = 0;
    uint8_t cur;
    
    int i;
    for (i = 0; i < fileSize; i++) {
        fread(&cur, 1, 1, file);
        checksum += cur;
    }
    
    return checksum;
}


// Sums all the bytes in a record, looking for 255 or 0xFF
// The file should be at the start position of the record
uint8_t recordChecksumCheck(FILE *file) {
    uint8_t checksum = 0;
    uint8_t cur;
    
    int i;
    for (i = 0; i < 40; i++) {
        fread(&cur, 1, 1, file);
        checksum += cur;
    }
    
    return checksum;
}

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
    
    fseek(file, -39, SEEK_CUR);
    uint8_t checksum = recordChecksumCheck(file);
    
    // Displaying the data
    if (checksum == 255) {
        printf("------------------------\nRecord %d\nValid Checksum: True\n------------------------\n", recordNum);
    }
    else {
        printf("------------------------\nRecord %d\nValid Checksum: False\n------------------------\n", recordNum);
        printf("Checksum = %"PRIu8"\n", checksum);
    }
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
        printf("Message:\n\t%s\n\n", message);
    }
    else {
        printf("%s", officialTime);
        if (dst == 1) {
            printf("Daylight Savings Time = true\n");
        }
        else {
            printf("Daylight Savings Time = false\n");
        }
        printf("Message:\n\t%s\n\n", message);
    }
}


// Reads the file and summarizes it's parts
void readFile(FILE *file) {
    uint16_t numRecords;
    
    // Reads the first 2 bytes (the number of records)
    fread(&numRecords, sizeof(numRecords), 1, file);
    
    // Skips the next 2 bytes (as they are spare bytes)
    fseek(file, 2, SEEK_CUR);
    
    // Reads each record
    int i;
    for (i = 0; i<numRecords; i++) {
        readRecord(file);
    }
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
    file = fopen(filename, "rb");
    
    // File could not be found, exiting
    if (file == NULL) {
        printf("Could not find file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    uint8_t checksum = fileChecksumCheck(file);
    rewind(file);
    
    if (checksum == 255) {
        printf("------------------------------------------------\n\tFilename: %s\n\tValid Checksum = True\n------------------------------------------------\n\n", filename);
    }
    else {
        printf("------------------------------------------------\n\tFilename: %s\n\tValid Checksum = False\n------------------------------------------------\n\n", filename);
    }
    
    readFile(file);
    
    fclose(file);
}
