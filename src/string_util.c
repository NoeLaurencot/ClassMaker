#include "mem_check.h"
#include "string_util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int word_count(char *string);

// Allocates memory for a string of given size
void *stralloc(int size) {
    return calloc(size + 1, 1);
}

void strip_comment(char *line) {
    char *comment = strstr(line, "//");
    if (comment) {
        *comment = '\0';
    }
}

char *str_dup(const char *string) {
    char *cpy = stralloc(strlen(string));
    malloc_check(cpy, "creating string duplicate");
    strcpy(cpy, string);
    return cpy;
}

// Checks if a character is an end of line/string character
int isendline(char c) {
    return (c == '\n' || c == '\r' || c == '\f' || c == '\v' || c == '\0');
}

// Creates a formatted string using format
char *create_str_formated_str(char *format, char *string) {
    char *full_string = stralloc(strlen(string) + strlen(format));
    malloc_check(full_string, "creating formated string");
    snprintf(full_string, strlen(string) + strlen(format) + 1, format, string);
    return full_string;
}

// Converts first character to uppercase (camelCase to PascalCase)
void camel_to_pascal(char *string) {
    if (string) {
        string[0] = toupper(string[0]);
    }
}

// Concatenates two strings and returns new allocated string
char *concat_str(char *dest, char *src) {
    char *str = stralloc(strlen(src) + strlen(dest));
    snprintf(str, strlen(src) + strlen(dest) + 1, "%s%s", dest, src);
    return str;
}

// Duplicates a substring starting from needle position
char *strstrdup(char *haystack, char *needle) {
    char *substring = strstr(haystack, needle);
    if (!substring) {
        return NULL;
    }
    char *string = stralloc(strlen(substring));
    malloc_check(string, "tried strstrdup");
    return strcpy(string, substring);
}

// Duplicates and truncates substring to needle length
char *strstrdup_trunc(char *haystack, char *needle) {
    char *string = strstrdup(haystack, needle);
    if (!string) {
        return NULL;
    }
    string[strlen(needle)] = '\0';
    return string;
}

// Gets the starting index of the first word
static int get_word_i(char *string) {
    int i = 0;
    int len = strlen(string);

    while (!isalpha(string[i])) {
        if (string[i] == '\0') {
            return -1;
        }
        if (isdigit(string[i])) {
            int is_word = 0;
            int j = i + 1;
            while (!isspace(string[j]) && !ispunct(string[j])) {
                if (string[j] == '\0' || j >= len) {
                    return -1;
                }
                if (isalpha(string[j])) {
                    is_word = 1;
                    break;
                }
                j++;
            }
            if (is_word) {
                break;
            }
        }
        i++;
    }
    return i;
}

// Finds the closing bracket/angle position
static int search_closing_i(char *string, char start) {
    if (start != '[' && start != '<') {
        return -1;
    }
    int i = 1;
    char end = start + 2;
    while (string[i] != end && !isendline(string[i])) {
        if (string[i] == '[' || string[i] == '<') {
            int res = search_closing_i(string + i, string[i]);
            if (res == -1) {
                return -1;
            }
            i += res;
        } else {
            i++;
        }
        
    }
    if (isendline(string[i])) {
        return -1;
    }
    return i + 1;
}

// Gets the ending index of the current word
static int get_word_end_i(char *string) {
    int i = 0;

    while (!isspace(string[i]) && !ispunct(string[i])) {
        if (string[i] == '\0') {
            return i;
        }
        i++;
    }
    if (string[i] == '[' || string[i] == '<') {
        char start = string[i];
        int res = search_closing_i(string + i, start);
        if (res == -1) {
            return i; 
        }
        i += res;
    }
    return i;
}

// Gets the starting index of the nth word
int get_nth_word_i(char *string, int n_word) {
    if (n_word == 0) {
        return -1;
    }
    int i = 0;
    int res;
    for (int j = 0; j < n_word; j++) {
        
        res = get_word_i(string + i);
        if (res == -1) {
            return -1;
        }
        i += res;

        if (j != n_word - 1) {
            i += get_word_end_i(string + i);
        }
    }
    return i;
}

// Gets the starting index of the last word
static int get_last_word_i(char *string) {
    return get_nth_word_i(string, word_count(string));
}

static int get_nth_last_word_i(char *string, int n_word) {
    int count = word_count(string);
    if (n_word > count) {
        return -1;
    }
    return get_nth_word_i(string, count - n_word + 1) ;
}

// Gets the ending index of the last word
static int get_last_word_end_i(char *string) {
    int i = 0;

    int res = get_last_word_i(string);
    if (res == -1) {
        return -1;
    }
    i += res;

    i += get_word_end_i(string + i);

    return i;
}

// Gets the ending index of the nth word from the end
static int get_nth_last_word_end_i(char *string, int n_word) {
    int i = 0;

    int res = get_nth_last_word_i(string, n_word);
    if (res == -1) {
        return -1;
    }
    i += res;

    i += get_word_end_i(string + i);

    return i;
}

// Gets the starting index of the next word
static int get_next_word_i(char *string) {
    int i = 0;

    i += get_word_end_i(string);

    int res = get_word_i(string + i);
    if (res == -1) {
        return -1;
    }
    i += res;
    return i;
}

// Gets the ending index of the next word
static int get_next_word_end_i(char *string) {
    int i = 0;

    int res = get_next_word_i(string);
    if (res == -1) {
        return -1;
    }
    i += res;

    i += get_word_end_i(string + i);

    return i;
}

// Gets the ending index of the nth word
static int get_nth_word_end_i(char *string, int n_word) {
    int i = 0;

    int res = get_nth_word_i(string, n_word);
    if (res == -1) {
        return -1;
    }
    i += res;

    i += get_word_end_i(string + i);
    
    return i;
}

// Extracts the first word from a string
char *get_first_word(char *string) {
    int i = get_word_i(string);
    if (i == -1) {
        return NULL;
    }
    int j = get_nth_word_end_i(string, 1);
    
    int len = j - i;
    char *word = stralloc(len);
    malloc_check(word, "creating first word");

    for (int letter = 0; letter < len; letter ++) {
        word[letter] = string[i + letter];
    }
    return word;
}

// Extracts the nth word from a string
char *get_nth_word(char *string, int n_word) {
    int i = get_nth_word_i(string, n_word);
    if (i == -1) {
        return NULL;
    }
    int j = get_nth_word_end_i(string, n_word);
    if (j == -1) {
        return NULL;
    }
    int len = j - i;
    char *word = stralloc(len);
    malloc_check(word, "creating nth word");

    for (int letter = 0; letter < len; letter ++) {
        word[letter] = string[i + letter];
    }
    return word;
}

// Extracts the next word after current position
char *get_next_word(char *string) {
    int i = get_next_word_i(string);
    if (i == -1) {
        return NULL;
    }
    int j = get_next_word_end_i(string);
    if (j == -1) {
        return NULL;
    }
    int len = j - i;
    char *word = stralloc(len);
    malloc_check(word, "creating next word");

    for (int letter = 0; letter < len; letter ++) {
        word[letter] = string[i + letter];
    }
    return word;
}

// Extracts the nth word from the end
char *get_nth_last_word(char *string, int n_word) {
    int i = get_nth_last_word_i(string, n_word);
    if (i == -1) {
        return NULL;
    }
    int j = get_nth_last_word_end_i(string, n_word);
    if (j == -1) {
        return NULL;
    }
    int len = j - i;
    char *word = stralloc(len);
    malloc_check(word, "creating nth last word");

    for (int letter = 0; letter < len; letter ++) {
        word[letter] = string[i + letter];
    }
    return word;
}

// Extracts the last word from a string
char *get_last_word(char *string) {
    int i = get_last_word_i(string);
    if (i == -1) {
        return NULL;
    }
    int j = get_last_word_end_i(string);
    if (j == -1) {
        return NULL;
    }
    int len = j - i;
    char *word = stralloc(len);
    malloc_check(word, "creating last word");

    for (int letter = 0; letter < len; letter ++) {
        word[letter] = string[i + letter];
    }
    return word;
}

// Counts the number of words in a string
int word_count(char *string) {
    int count = 0;
    int i = 0;
    int word_start;

    while ((word_start = get_word_i(string + i)) != -1) {
        count++;
        i += word_start + get_word_end_i(string + i + word_start);
    }

    return count;
}