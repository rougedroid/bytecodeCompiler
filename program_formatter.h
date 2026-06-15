#ifndef PROGRAM_FORMATTER_H
#define PROGRAM_FORMATTER_H

/**
 * Reads an entire file into a dynamically allocated string.
 * Returns NULL on failure. Caller must free() the returned string.
 */
char* readFile(const char *filename);

/**
 * Pads every token with spaces, ensures no double spaces,
 * and appends "EOF" at the very end.
 * Returns NULL on failure. Caller must free() the returned string.
 */
char* formatProgramString(const char *rawContent);

#endif // PROGRAM_FORMATTER_H