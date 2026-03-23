#include "scan_input.h"
#include "mem_check.h"
#include "string_util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Scans and returns a non-negative integer from input
int scan_unsigned_int() {
    int result;
    int n;
    while ((result = scanf("%d", &n)) != 1 || n < 0) {
        if (result == EOF) {
            fprintf(stderr, "error: input stream closed\n");
            exit(1);
        }
        if (result != 1) {
            while (getchar() != '\n');
        }
        fprintf(stderr, "Invalid input, try again\n");
    }
    while (getchar() != '\n');
    return n;
}

// Scans an integer within min and max range
int scan_min_max_int(int n_min, int n_max) {
    int result;
    int n;
    while ((result = scanf("%d", &n)) != 1 || n < n_min || n > n_max) {
        if (result == EOF) {
            fprintf(stderr, "error: input stream closed\n");
            exit(1);
        }
        if (result != 1) {
            while (getchar() != '\n');
        }
        fprintf(stderr, "Invalid input, try again\n");
    }
    while (getchar() != '\n');
    return n;
}

// Scans an unsigned integer within maximum range
int scan_n_unsigned_int(int n_max) {
    return scan_min_max_int(0, n_max);
}

// Scans and returns any integer from input
int scan_int() {
    int result;
    int n;
    while ((result = scanf("%d", &n)) != 1) {
        if (result == EOF) {
            fprintf(stderr, "error: input stream closed\n");
            exit(1);
        }
        while (getchar() != '\n');
        fprintf(stderr, "Invalid input, try again\n");
    }
    return n;
}

// Scans and returns a boolean from input (y/n)
int scan_boolean() {
    int result;
    char c;
    while ((result = scanf(" %c", &c)) != 1 || (c != 'y' && c != 'n')) {
        if (result == EOF) {
            fprintf(stderr, "error: input stream closed\n");
            exit(1);
        }
        if (result != 1) {
            while (getchar() != '\n');
        }
        fprintf(stderr, "Invalid input, try again\n");
    }
    return c == 'y';
}

// Checks if a string contains at least one letter
static int string_contains_letter(char *string) {
    for (int i = 0; i < strlen(string); i++) {
        if (isalpha(string[i])) {
            return 1;
        }
    }
    return 0;
}

// Scans a string that must contain at least one letter
char *scan_string(int size) {
    int result;
    char *string = stralloc(size);
    malloc_check(string, "scanning string");
    char tmp[16];
    snprintf(tmp, 16, "%%%ds", size - 1);
    while ((result = scanf(tmp, string)) != 1 || !string_contains_letter(string)) {
        if (result == EOF) {
            fprintf(stderr, "error: input stream closed\n");
            exit(1);
        }
        while (getchar() != '\n');
        fprintf(stderr, "Invalid input, try again\n");
    }
    while (getchar() != '\n');
    return string;
}