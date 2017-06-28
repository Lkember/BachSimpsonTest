#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

// Writes a new record to the given file at the current position
void writeRecord(FILE *file, char *message, short int recordNum, int auxiliaryFlag) {
    printf("Adding record...\n");
    
    int dst;
    short int spareByte = 0;
    
    // Calculating the current time for the new record
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int time = (int)rawtime;
    // NEED TO SUBTRACT THE AMOUNT OF SECONDS FROM 1970-2000 FROM time
    
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
        int i = 3;
        fwrite(&i, 1, 1, file);
    }
    else if (dst == 1 && auxiliaryFlag == 0) {
        int i = 2;
        fwrite(&i, 1, 1, file);
    }
    else if (dst == 0 && auxiliaryFlag == 1) {
        int i = 1;
        fwrite(&i, 1, 1, file);
    }
    else {
        int i = 0;
        fwrite(&i, 1, 1, file);
    }
    
    // Write the time followed by 3 spare bytes
    fwrite(&time, 4, 1, file);
    fwrite(&spareByte, 3, 1, file);
    
    // Write the ASCII text to a file
    fprintf(file, "%s", message);
    
    // Write the checksum value
    // TODO NEED TO WRITE CHECKSUM VALUE
    fwrite(&spareByte, 1, 1, file);
    
    printf("Successfully added record\n");
}


// Updates the file to include the latest message
void updateFile(FILE *read, FILE *write, char *message, int auxiliaryFlag) {
    printf("File found. Updating file.\n");
    
    unsigned char buf[2];
    short int numRecords;
    short int spareByte = 0;
    
    //Read the first 2 bytes which are the number of records
    fread(buf, 1, 2, read);
    
    // Convert to an int and add 1
    numRecords = (buf[1]<<8)+buf[0];
    numRecords += 1;
    
    // Update numRecords and then write two blank bytes (as they are spare)
    fwrite(&numRecords, 2, 1, write);
    fwrite(&spareByte, 2, 1, write);
    
    // Skip to the position where we need to add the new record
    fseek(write, numRecords*30, SEEK_CUR);
    
    // Write the new record
    writeRecord(write, message, numRecords, auxiliaryFlag);
    
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
    FILE *read_fptr;
    FILE *write_fptr;
    
    // This is used for reading arguments
    static struct option long_options[] = {
        {"auxiliary", optional_argument, 0, 'a'},
        {"file", required_argument, 0, 'f'},
        {"textMessage", required_argument, 0, 't'},
        {0,0,0,0}
    };
    
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "af:t:", long_options, &option_index)) != -1) {
        
        switch (c) {
            
            case 'a':
                auxiliaryFlag = 1;
                break;
                
            case 'f':
                filename = optarg;
                break;
                
            case 't':
                message = optarg;
                break;
                
        }
    }
    
    if (filename == NULL || message == NULL) {
        printf("Error\nUsage: -f FILENAME -t LOG MESSAGE\n");
        exit(EXIT_FAILURE);
    }
    
    
    printf("auxiliary flag: %d\nfile: %s\nmessage: %s\n", auxiliaryFlag, filename, message);
    
    // Attempt to read the file
    read_fptr = fopen(filename, "r");
    
    // If the file is null it needs to be created
    if (read_fptr == NULL) {
        write_fptr = fopen(filename, "a+");
        createFile(write_fptr, message, auxiliaryFlag);
        
        fclose(write_fptr);
    }
    // Otherwise we only need to update the file
    else {
        write_fptr = fopen(filename, "r+");
        updateFile(read_fptr, write_fptr, message, auxiliaryFlag);
        
        fclose(read_fptr);
        fclose(write_fptr);
    }
}
