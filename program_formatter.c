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
    // Allocate plenty of space
    char *output = malloc(len * 3 + 10);
    if (output == NULL) return NULL;

    int j = 0; // Index for output string

    for (int i = 0; i < len; i++) {
        char current = rawContent[i];

        // 1. Skip existing whitespaces/newlines, just use them to separate tokens
        if (isspace(current)) {
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }
            continue;
        }

        // 2. If it's an alphanumeric character (letter or number), it's part of a word!
        if (isalnum(current)) {
            // If the previous character was a symbol, we need a space before this word
            if (j > 0 && output[j - 1] != ' ' && !isalnum(rawContent[i - 1])) {
                output[j++] = ' ';
            }
            
            // Just copy the letter/digit directly (NO trailing space yet!)
            output[j++] = current;
        } 
        // 3. Otherwise, it's a symbol (like ;, +, (, {, etc.)
        else {
            // Ensure there's a space before the symbol
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }

            // Copy the symbol
            output[j++] = current;

            // Force a space AFTER the symbol
            output[j++] = ' ';
        }
    }

    // Clean up any trailing space right before EOF
    if (j > 0 && output[j - 1] == ' ') {
        j--;
    }

    // Append the EOF marker cleanly
    strcpy(&output[j], " EOF");

    return output;
}