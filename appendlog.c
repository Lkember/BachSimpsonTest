#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// Updates the file to include the latest message
void updateFile(FILE *read, FILE *write, char *message) {
    unsigned char buf[2];
    int numRecords;         // THIS MAY NEED TO BE short int since int is 4 bytes
    
    //Read the first 2 bytes which are the number of records
    fread(buf, 1, 2, read);
    
    // Convert to an int and add 1
    numRecords = (buf[1]<<8)+buf[0];
    numRecords += 1;
    
    
}

// Creates the file with the given message
void createFile(FILE *file, char *message) {
    
}

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
    
        if (c == -1) {
            break;
        }
        
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
    
    // Attempt to read the file
    read_fptr = fopen(filename, "r");
    
    // If the file is null it needs to be created
    if (read_fptr == NULL) {
        write_fptr = fopen(filename, "w+");
        createFile(write_fptr, message);
        
        fclose(write_fptr);
    }
    // Otherwise we only need to update the file
    else {
        write_fptr = fopen(filename, "r+");
        updateFile(read_fptr, write_fptr, message);
        
        fclose(read_fptr);
        fclose(write_fptr);
    }
}
