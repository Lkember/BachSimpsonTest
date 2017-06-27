#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file; 
    char data = 1;
    file = fopen("test", "w");
    fwrite(&data, 1, 1, file);
    fclose(file);
}
