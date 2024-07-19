#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
void extract_pis_pos(char *text, char **pis, char **pos) {
    char *line = strtok(text, "\n");
    int i = 0;

    // Extract PIs
    while (line != NULL && *line != '\0') {
        pis[i++] = strdup(line);
        line = strtok(NULL, "\n");
    }

    // Extract POs
    if (line != NULL) {
        *pos = strdup(line);
    } else {
        *pos = NULL; // Handle case where POs are missing
    }
}

int main() {
    
    char line[MAX_LINE_LENGTH];
    FILE *parseredFile = fopen("parseredFile.txt", "w");
    FILE *fp = fopen("cut_ex3.test", "r");
    if (fp == NULL) {
        printf("Could not open file.\n");
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        // If a line starts with '*' or has length less than or equal to 3, skip it
        if (line[0] != '*' && strlen(line) > 3) {
            // Extract required information from the line
            // Implement your parsing logic here
            printf("%s", line); // Print the line as an example
            
            if (parseredFile == NULL) {
                perror("Error creating output file");
            }
            fputs(line, parseredFile);
        }
    }
    
    fclose(fp);
    
    return 0;
}