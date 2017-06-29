#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

// Writes a new record to the given file at the current position
void writeRecord(FILE *file, char *message, short int recordNum, int auxiliaryFlag) {
    printf("Adding record...\n");
    
    int epochTimestamp = 946684800;     //The amount of seconds from 1970-2000
    int dst;
    short int spareByte = 0;
    
    // Calculating the current time for the new record
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int time = (int)rawtime;
    time -= epochTimestamp;
    
    if (timeinfo->tm_isdst > 0) {
        dst = 1;
    }
    else {
        dst = 0;
    }
    
    // Write the record number
    fwrite(&recordNum, 2, 1, file);
    
    
    // Write the auxiliary and dst flags
    if (dst == 1 && auxiliaryFlag == 1) {
        uint8_t i = 192;
        fwrite(&i, 1, 1, file);
    }
    else if (dst == 0 && auxiliaryFlag == 1) {
        uint8_t i = 128;
        fwrite(&i, 1, 1, file);
    }
    else if (dst == 1 && auxiliaryFlag == 0) {
        uint8_t i = 64;
        fwrite(&i, 1, 1, file);
    }
    else {
        uint8_t i = 0;
        fwrite(&i, 1, 1, file);
    }
    
    // Write the time followed by 3 spare bytes
    fwrite(&time, 4, 1, file);
    fwrite(&spareByte, 2, 1, file);
    
    // Write the ASCII text to a file
    fwrite(message, 30, 1, file);
    
    // Write the checksum value
    // TODO NEED TO WRITE CHECKSUM VALUE
    fwrite(&spareByte, 1, 1, file);
    
    printf("Record added\n");
}


// Updates the file to include the latest message
void updateFile(FILE *file, char *message, int auxiliaryFlag) {
    printf("File found. Updating file.\n");
    
    unsigned char buf[2];
    short int numRecords;
    short int spareByte = 0;
    
    //Read the first 2 bytes which are the number of records
    fread(buf, 1, 2, file);
    
    // Convert to an int and add 1
    numRecords = (buf[1]<<8)+buf[0];
    numRecords += 1;
    
    rewind(file);
    
    // Update numRecords and then write two blank bytes (as they are spare)
    fwrite(&numRecords, 2, 1, file);
    fwrite(&spareByte, 2, 1, file);
    
    // Skip to the position where we need to add the new record
    int position = 0;
    position += 4;                  // Adding the sequence number + 2 spare bytes (4 bytes in total)
    position += (numRecords-1)*40;  // Adding 40 bytes per record
    
    fseek(file, position, SEEK_SET);
    
    // Write the new record
    writeRecord(file, message, numRecords, auxiliaryFlag);
    
    // Write the checksum
    //TODO: NEED TO CALCULATE CHECKSUM
    fwrite(&spareByte, 1, 1, file);
    
    printf("Successfully updated file\n");
}

// Creates the file with the given message
void createFile(FILE *file, char *message, int auxiliaryFlag) {
    printf("File not found. Creating file.\n");
    short int numRecords = 1;
    short int spareByte = 0;
    
    // Write the number of records followed by 2 bytes
    fwrite(&numRecords, 1, 2, file);
    fwrite(&spareByte, 2, 1, file);
    
    // Write the next record
    writeRecord(file, message, numRecords, auxiliaryFlag);
    
    fwrite(&spareByte, 1, 1, file);
}


//////////////MAIN/////////////////
int main(int argc, char* argv[]) {
    int auxiliaryFlag = 0;
    char *filename;
    char *message;
    FILE *file;
    
    
    // Checking arguments
    
    if (argc < 5) {
        printf("Error\nUsage: -f FILENAME -t LOG MESSAGE\n");
    }
    
    int i;
    for (i = 1; i < argc; i++) {
        
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "-F") == 0) {
            filename = argv[i+1];
            i += 1;
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "-A") == 0) {
            auxiliaryFlag = 1;
        }
        else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "-T") == 0) {
            message = argv[i+1];
            i += 1;
        }
        else {
            printf("Unknown argument %s\n", argv[i]);
        }
    }
    

    if (filename == NULL || message == NULL) {
        printf("Error\nUsage: -f FILENAME -t LOG MESSAGE\n");
        exit(EXIT_FAILURE);
    }
    
    // Attempt to read the file
    file = fopen(filename, "r");
    
    // If the file is null it needs to be created
    if (file == NULL) {
        file = fopen(filename, "a+");
        createFile(file, message, auxiliaryFlag);
        
        fclose(file);
    }
    // Otherwise we only need to update the file
    else {
        fclose(file);
        file = fopen(filename, "r+");
        updateFile(file, message, auxiliaryFlag);
        
        fclose(file);
    }
}
