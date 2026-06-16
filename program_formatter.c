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

        // 1. Skip existing whitespaces/newlines
        if (isspace(current)) {
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }
            continue;
        }

        // 2. Alphanumeric characters (words/numbers)
        if (isalnum(current)) {
            // Fixed safety check: look at output[j-1] rather than rawContent[i-1] 
            // to safely know if the last written char was an alphanumeric.
            if (j > 0 && output[j - 1] != ' ' && !isalnum(output[j - 1])) {
                output[j++] = ' ';
            }
            output[j++] = current;
        } 
        // 3. Symbols and Operators
        else {
            // Ensure there's a space before the symbol
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }

            // Check for 2-character operators (==, !=, <=, >=, ++, --, &&, ||)
            if (i + 1 < len) {
                char next = rawContent[i + 1];
                if ((current == '=' && next == '=') ||
                    (current == '!' && next == '=') ||
                    (current == '<' && next == '=') ||
                    (current == '>' && next == '=') ||
                    (current == '+' && next == '+') ||
                    (current == '-' && next == '-') ||
                    (current == '&' && next == '&') ||
                    (current == '|' && next == '|')) {
                    
                    // Copy both characters
                    output[j++] = current;
                    output[j++] = next;
                    i++; // Skip the next character in the loop
                    
                    // Force a space AFTER the 2-char operator
                    output[j++] = ' ';
                    continue; 
                }
            }

            // Fallback for single-character symbols
            output[j++] = current;
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