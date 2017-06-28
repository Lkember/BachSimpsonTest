#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE *file; 
    short int data = 2;

    printf("sizeOf data = %lu\n", sizeof(data));
    printf("sizeOf time = %lu\n", sizeof(time_t));
    file = fopen("test", "w");
    fwrite(&data, 2, 1, file);
    fclose(file);
}
