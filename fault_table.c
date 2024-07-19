#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the maximum size constants
#define MAX_FAULT_NAME 100
#define MAX_LINE_LENGTH 100
#define MAX_TEST_VECTORS 100
#define MAX_UNIQUE_VECTORS 1000

// Define a structure to store the test vectors for a fault
typedef struct {
    char faultName[MAX_FAULT_NAME];
    char testVectors[MAX_TEST_VECTORS][MAX_LINE_LENGTH];
    int count;
} Fault;

// Function to trim whitespace from both ends of a string
char* trimWhitespace(char* str) {
    char* end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    // Trim trailing space
    if (*str == 0)  // All spaces?
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

// Function to convert fault name with slash
void convertFaultName(char* dest, const char* src) {
    while (*src) {
        if (*src != ' ' && *src != '/') {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

void replace_x(char *s, int index, char **result, int *count, int *capacity) {
    if (s[index] == '\0') {
        if (*count >= *capacity) {
            *capacity *= 2;
            result = realloc(result, (*capacity) * sizeof(char *));
            if (result == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(1);
            }
        }
        result[*count] = strdup(s);
        if (result[*count] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        (*count)++;
        return;
    }
    
    if (s[index] == 'x') {
        s[index] = '0';
        replace_x(s, index + 1, result, count, capacity);
        s[index] = '1';
        replace_x(s, index + 1, result, count, capacity);
        s[index] = 'x';  // revert back for further recursion
    } else {
        replace_x(s, index + 1, result, count, capacity);
    }
}

// Function to generate combinations for the entire array
char** generate_combinations(char testVectors[][MAX_LINE_LENGTH], int arrSize, int *resultSize) {
    int capacity = 100;
    char **result = malloc(capacity * sizeof(char *));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    *resultSize = 0;

    for (int i = 0; i < arrSize; i++) {
        replace_x(testVectors[i], 0, result, resultSize, &capacity);
    }

    return result;
}

void replace_testVectors(Fault *fault, char **newValues, int newValuesSize) {
    // Ensure the new values fit within the MAX_TEST_VECTORS limit
    int limit = (newValuesSize < MAX_TEST_VECTORS) ? newValuesSize : MAX_TEST_VECTORS;
    for (int i = 0; i < limit; i++) {
        strncpy(fault->testVectors[i], newValues[i], MAX_LINE_LENGTH - 1);
        fault->testVectors[i][MAX_LINE_LENGTH - 1] = '\0';  // Ensure null termination
    }
    fault->count = limit;
}

// Function to check if a vector is in a fault
int vector_in_fault(Fault *fault, const char *vector) {
    for (int i = 0; i < fault->count; i++) {
        if (strcmp(fault->testVectors[i], vector) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    Fault faults[MAX_FAULT_NAME];
    int faultIndex = -1;

    // Initialize faults
    for (int i = 0; i < MAX_FAULT_NAME; i++) {
        faults[i].count = 0;
    }

    // Read input lines from file
    while (fgets(line, sizeof(line), file)) {
        // Trim the line
        char* trimmedLine = trimWhitespace(line);

        // Ignore lines that start with '*'
        if (trimmedLine[0] == '*') {
            continue;
        }

        // Check if the line represents a new fault
        if (isalpha(trimmedLine[0]) || trimmedLine[0] == 'e') {
            faultIndex++;
            char convertedName[MAX_FAULT_NAME];
            convertFaultName(convertedName, trimmedLine);
            snprintf(faults[faultIndex].faultName, MAX_FAULT_NAME, "%s", convertedName);
        } else if (isdigit(trimmedLine[0])) {
            // Add the test vector to the current fault
            char* testVector = strchr(trimmedLine, ':');
            if (testVector) {
                testVector++;
                while (*testVector == ' ' || *testVector == '\t') testVector++;
                char* space = strchr(testVector, ' ');
                if (space) *space = '\0';
                snprintf(faults[faultIndex].testVectors[faults[faultIndex].count], MAX_LINE_LENGTH, "%s", testVector);
                faults[faultIndex].count++;
            }
        }
    }

    fclose(file);

    // Collect all unique test vectors across all faults
    char uniqueTestVectors[MAX_UNIQUE_VECTORS][MAX_LINE_LENGTH];
    int uniqueCount = 0;

    for (int i = 0; i <= faultIndex; i++) {
        int output_size;
        char **output = generate_combinations(faults[i].testVectors, faults[i].count, &output_size);
        replace_testVectors(&faults[i], output, output_size);

        for (int j = 0; j < faults[i].count; j++) {
            int found = 0;
            for (int k = 0; k < uniqueCount; k++) {
                if (strcmp(uniqueTestVectors[k], faults[i].testVectors[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strncpy(uniqueTestVectors[uniqueCount], faults[i].testVectors[j], MAX_LINE_LENGTH - 1);
                uniqueTestVectors[uniqueCount][MAX_LINE_LENGTH - 1] = '\0';  // Ensure null termination
                uniqueCount++;
            }
        }

        // Free the allocated memory for the generated combinations
        for (int j = 0; j < output_size; j++) {
            free(output[j]);
        }
        free(output);
    }

    // Print the table header
    printf("%-6s", "tv");
    for (int i = 0; i <= faultIndex; i++) {
        printf("%-6s", faults[i].faultName);
    }
    printf("\n");

    // Print the table rows
    for (int i = 0; i < uniqueCount; i++) {
        printf("%-6s", uniqueTestVectors[i]);
        for (int j = 0; j <= faultIndex; j++) {
            if (vector_in_fault(&faults[j], uniqueTestVectors[i])) {
                printf("%-6s", "x");
            } else {
                printf("%-6s", "");
            }
        }
        printf("\n");
    }

    //save taable to fault_table.txt file
    FILE *fault_table = fopen("fault_table.csv", "w");

    // Write the header row 
    fprintf(fault_table, "TestVector,");

    for (int j = 0; j <= faultIndex; j++) {
        fprintf(fault_table,"%s,", faults[j].faultName);
    }
    fprintf(fault_table, "\n");
    // datas
    for (int i = 0; i < uniqueCount; i++) {
        fprintf(fault_table, "%s,", uniqueTestVectors[i]);
        for (int j = 0; j <= faultIndex; j++) {
            fprintf(fault_table, "%s,", vector_in_fault(&faults[j], uniqueTestVectors[i]) ? "x" : " ");
        }
        // Remove the trailing comma
        fseek(fault_table, -1, SEEK_CUR);
        fprintf(fault_table, "\n");
    }

    return 0;
}
