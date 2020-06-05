// headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// typedefs
typedef uint8_t BYTE;

// globals
bool jpeg_block_found = false;
int file_name_counter = 0;

// prototypes
void recover_jpeg(BYTE *file_block, char *file_name);
bool is_jpeg(BYTE *file_block);
void write_jpeg(BYTE *file_block, char *file_name);


// code logic
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s image\n", argv[0]);
    }

    // open our file for reading
    FILE *file = fopen(argv[1], "rb");

    if (!file)
    {
        printf("An error occurred when attempting to read the %s image file\n", argv[1]);
    }

    // initialize vars for use in program
    BYTE *file_block = malloc(512 * sizeof(BYTE));
    char *file_name = malloc(10 * sizeof(char));

    if (file_block == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
    }

    // read the file into our file_block buffer until all is read
    while(fread(file_block, 512 * sizeof(BYTE), 1, file) > 0)
    {
        recover_jpeg(file_block, file_name);
    }

    free(file_block);
    free(file_name);

    // close file
    fclose(file);

    return 0;
}

void recover_jpeg(BYTE *file_block, char *file_name)
{
    // check first four bytes (use bitwise & to null last 4)
    if (is_jpeg(file_block))
    {
        // set our global to true
        jpeg_block_found = true;

        // update filename value
        sprintf(file_name, "%03i.jpg", file_name_counter);

        printf("new file to be created: %s\n", file_name);

        // increment out filename counter
        file_name_counter ++;
    }

    // we're writing to a jpeg that spans multiple file blocks
    if (jpeg_block_found)
    {
        // write to the current fd
        write_jpeg(file_block, file_name);
    }

    return;
}

// check if jpeg
bool is_jpeg(BYTE *file_block)
{
    // check first 4 bytes to see if block starts with jpeg header
    if (file_block[0] == 0xff && file_block[1] == 0xd8 && file_block[2] == 0xff && (file_block[3] & 0xf0) == 0xe0)
    {
        return true;
    }

    return false;
}

void write_jpeg(BYTE *file_block, char *file_name)
{
    // write the file_block to our file in append mode
    printf("now writing to file: %s\n", file_name);
    FILE *jpeg = fopen(file_name, "ab");

    fwrite(file_block, 512 * sizeof(BYTE), 1, jpeg);
    fclose(jpeg);
}
