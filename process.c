#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 9
#define MAX_COLS 40

const char *testvectors[] = {
    "111", "001", "101", "000", "010", "100", "110", "011"
};

int select_test_vector(char data[MAX_ROWS][MAX_COLS]) {

    int m = 0;
    int n = 0;
    
    int selected_tv = 0;
    // گردش بر روی تمام تست وکتورها

    //نیازمند اصلاحیه است تا برای هر ارایه دوبعدی ای کار کند
    for (int i = 1; i < 9; i++) { 
        for (int j = 3; j < 40; j++) {
            m = m + (data[i][j]- '0');
            //printf("%d\n", (data[i][j]- '0'));
        }
        if (m > n){
            n = m;
            selected_tv = i;
        }
        m = 0;
    }
    return selected_tv;
}

const char *testvector(int i) {
    if (i < 1 || i > 8) {
        // Handle invalid input
        return NULL;
    }

    return testvectors[i - 1];
}

/*
char ** convert_to_dynamic(char data[MAX_ROWS][MAX_COLS]) {
    // Allocate memory for the dynamic array
    char **dynamic_array= (char **)malloc(MAX_ROWS * sizeof(char *));
    for (int i = 0; i < MAX_ROWS; i++) {
        (*dynamic_array)[i] = (char *)malloc(MAX_COLS * sizeof(char));
    }

    // Copy elements from static to dynamic array
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            (dynamic_array)[i][j] = data[i][j];
        }
    }
    return dynamic_array;
}

char** update_fault_table( char **dynamic_array, int tv){
    int i, j, k;
    int new_cols = MAX_COLS - 1;
 //نیازمند اصلاحیه است تا برای هر ارایه دوبعدی ای کار کند
   
    printf("s1");
    for (j = 3; j < 40; j++) {
        if (dynamic_array[i][j] == "x") {
            // Shift columns to the left to remove the current column
            for (k = j; k < new_cols - 1; k++) {
                dynamic_array[tv][k] = dynamic_array[tv][k + 1];
            }
            printf("s");
            new_cols--;
            j--; // Check the same column again after shifting
        }
    }

    return dynamic_array;
}

*/

int check_fault_Coverage(char data[MAX_ROWS][MAX_COLS]){
    //calculate
    int fault_coverage = 0;
    for (int i = 1; i < MAX_ROWS; ++i) {
        for (int j = 3; j < MAX_COLS; ++j) {
            fault_coverage = fault_coverage + (int)(data[i][j]);
        }
    }
    return fault_coverage;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    char data[MAX_ROWS][MAX_COLS];
    int rows = 0, cols = 0;


    // Read data from the file
    //120-> 'x'
    //48->  ' '
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        cols = 0;
        char *token = strtok(line, ",");
        while (token != NULL) {
            if (strcmp(token, " ") == 0 || strcmp(token, " \n") == 0) {
                data[rows][cols++] = '0';
            } 
            else if (strcmp(token, "x") == 0 || strcmp(token, "x\n") == 0) {
                data[rows][cols++] = '1';
            } else {
                // Handle invalid data (e.g., print an error message)
                //fprintf(stderr, "Invalid data: %s\n", token);
                data[rows][cols++] = ' ';
            }
            token = strtok(NULL, ",");
        }
        rows++;
    }
    fclose(fp);

    // Now you have the data in the 2D array 'data'
    // ...
    for (int i = 1; i < rows; ++i) {
        for (int j = 3; j < cols; ++j) {
            printf("%d ", data[i][j]);
        }
        printf("\n");
    }
    
    //فرض برای 1 بار بگردیم دنبال بیشترین همپوشانی
    //در واقع مساله np عه و باید هیورستیک بزنیم
    //اینجا ساده ترین فرض کنیم
    int selected_vectors[MAX_COLS];
    int perfect_fault_coverage = 48*(rows-1)*(cols-3);
    int new_fault_coverage;
    int s = 0;
    //update_fault_table
    do
    {
        int x = select_test_vector(data);
        selected_vectors[s] = x;
        for (int j = 3; j < MAX_COLS; ++j) {
            if (data[x][j] == '1'){
                //printf("%d ", m++);
                for (int i = 1; i < MAX_ROWS; ++i){
                    data[i][j] = '0';
                }
                // data[x][j] = ' ' ;
            }
        }
        // printf("\n");
        // for (int i = 1; i < rows; ++i) {
        //     for (int j = 3; j < cols; ++j) {
        //         printf("%d ", (int)(data[i][j]));
        //     }
        //     printf("\n");
        // }
        printf(testvector(selected_vectors[s]));
        printf("\t");
        new_fault_coverage = check_fault_Coverage(data);
        printf("%d ", new_fault_coverage);
        //هرچی کمتر باشه بهتره
        printf("\n");
        s++;
    } while ((new_fault_coverage != perfect_fault_coverage) || s == 10);
    //اگه بعد 10 بار هم نتونست پیدا کنه دیگه خارج شه و مثلا محدودیت تکرار داریم
    for (int i = 0; i < s; ++i) {
        printf(testvector(selected_vectors[i]));
        printf("\t");
    }

    //چرینت بهترین حالت پوشش اشکال

    printf("%d ", perfect_fault_coverage);

    return 0;
}
