#include <stdlib.h>
#include "mem_check.h"
#include <stdio.h>

// Checks if pointer is valid, exits with error message if not
void mem_check(void *ptr, char *string) {
    if (!ptr) {
        fprintf(stderr, "%s\n", string);
        exit(1);
    }
}

// Checks if memory allocation succeeded, exits with error message if not
void malloc_check(void *ptr, char *string) {
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed when %s\n", string);
        exit(1);
    }
}