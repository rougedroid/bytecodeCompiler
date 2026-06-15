#include "program_formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char* readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'.\n", filename);
        return NULL;
    }

    // Find file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory
    char *buffer = malloc(fileSize + 1);
    if (buffer == NULL) {
        printf("Memory allocation failed while reading file.\n");
        fclose(file);
        return NULL;
    }

    // Read contents and null-terminate
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    
    fclose(file);
    return buffer;
}

char* formatProgramString(const char *rawContent) {
    if (rawContent == NULL) return NULL;
    
    int len = strlen(rawContent);
    
    // Allocate 3x the size + extra padding for " EOF\0"
    char *output = malloc(len * 3 + 10);
    if (output == NULL) {
        printf("Memory allocation failed during formatting.\n");
        return NULL;
    }

    int j = 0; // Index for output string

    for (int i = 0; i < len; i++) {
        char current = rawContent[i];

        // Handle whitespaces/newlines from the original file
        if (isspace(current)) {
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }
            continue;
        }

        // Space before character
        if (j > 0 && output[j - 1] != ' ') {
            output[j++] = ' ';
        }

        // Insert character
        output[j++] = current;

        // Space after character
        output[j++] = ' ';
    }

    // Clean up any trailing space right before EOF to prevent double spaces
    if (j > 0 && output[j - 1] == ' ') {
        j--;
    }

    // Append the EOF marker cleanly
    strcpy(&output[j], " EOF");

    return output;
}