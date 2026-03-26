#include "enums.h"
#include "java_str.h"
#include "mem_check.h"
#include "scan_input.h"
#include "string_util.h"
#include "structs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int LINE_LENGTH = 512;

/**
 * @brief Advances by n lines in given file
 * @note The next fgets(void) call will return line n+1
 * @param file `FILE` File to read from
 * @param n `int` Number of line to advance
 * @retval `0` - Failed
 * @retval `1` - Succeeded
 */
int advance_n_lines(FILE *file, int n) {
    if (n < 0) {
        return 0;
    }

    char line[LINE_LENGTH];

    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), file)) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Finds the line number containing the class declaration
 * @param file `FILE` File to read from
 * @note Line number starts at 1
 * @return `int` - Line number of the class declaration, -1 if didn't find it
 */
int get_class_line(FILE *file) {
    rewind(file);

    char line[LINE_LENGTH];
    int line_number = 1;

    while (fgets(line, sizeof(line), file)) {
        strip_comment(line);
        if (strstr(line, "class") && strstr(line, "{") && !strstr(line, ";")) {
            return line_number;
        }
        line_number++;
    }
    return -1;
}

/**
 * @brief Extracts the nth line of a file
 * @param file `FILE` File to read from
 * @param n `int` Line number to extract
 * @note Line number starts at 1
 * @return `char *` - Line extracted, NULL if failed
 */
char *extract_nth_line(FILE *file, int n) {
    rewind(file);

    char *line = stralloc(LINE_LENGTH);
    malloc_check(line, "creating file line");

    if (!advance_n_lines(file, n)) {
        free(line);
        return NULL;
    }
    if (!fgets(line, LINE_LENGTH, file)) {
        free(line);
        return NULL;
    }

    return line;
}

/**
 * @brief Extracts the class package
 * @param file `FILE` File to read from
 * @return `char *` - Package name
 */
char *get_class_package(FILE *file) {
    rewind(file);

    char line[LINE_LENGTH];

    for (int i = 0; i < 2; i++) {
        if (fgets(line, sizeof(line), file)) {
            strip_comment(line);
            if (strstr(line, "package")) {
                char *package = str_dup(line + get_nth_word_i(line, 2));
                for (int j = 0; j < strlen(package); j++) {
                    if (isendline(package[j]) || package[j] == ';') {
                        package[j] = '\0';
                    }
                }
                return package;
            }
            
        }
    }
    return NULL;
}

/**
 * @brief Checks if the class is final
 * @param file `FILE` File to read from
 * @param class_line `int` Line number of the class definition
 * @retval `0` - Is not final
 * @retval `1` - Is final
 */
int get_class_final(FILE *file, int class_line) {
    rewind(file);

    char *line = extract_nth_line(file, class_line - 1);
    mem_check(line, "Couldn't find class line");
    strip_comment(line);
    if (strstr(line, "final")) {
        free(line);
        return 1;
    }
    free(line);
    return 0;
}

/**
 * @brief Extracts the class visiblity
 * @param file `FILE` File to read from
 * @param class_line `int` Line number of the class definition
 * @return `char *` - The class visibility
 */
char *get_class_vis(FILE *file, int class_line) {
    rewind(file);

    char *line = extract_nth_line(file, class_line - 1);
    mem_check(line, "Couldn't find class line");
    strip_comment(line);
    if (strstr(line, "public")) {
        free(line);
        return str_dup("public");
    } else if (strstr(line, "protected")) {
        free(line);
        return str_dup("protected");
    } else if (strstr(line, "private")) {
        free(line);
        return str_dup("private");
    } else {
        free(line);
        return str_dup("");
    }
}

/**
 * @brief Extracts the class name
 * @param file `FILE` File to read from
 * @param class_line `int` Line number of the class definition
 * @return `char *` - The class name
 */
char *get_class_name_upper(FILE *file, int class_line) {
    rewind(file);

    char *line = extract_nth_line(file, class_line - 1);
    mem_check(line, "Couldn't find class line");
    strip_comment(line);
    char *str = strstr(line, "class");
    if (!str) {
        free(line);
        return NULL;
    }
    char *parent_name = get_next_word(str);
    free(line);
    return parent_name;
}

/**
 * @brief Extracts the parent class name based on keyword "extends"
 * @param file `FILE` File to read from
 * @param class_line `int` Line number of the class definition
 * @return `char *` - The parent class name
 */
char *get_parent_class_name_upper(FILE *file, int class_line) {
    rewind(file);

    char *line = extract_nth_line(file, class_line - 1);
    mem_check(line, "Couldn't find class line");
    strip_comment(line);
    char *str = strstr(line, "extends");
    if (!str) {
        free(line);
        return NULL;
    }
    char *parent_name = get_next_word(str);
    free(line);
    return parent_name;
}

/**
 * @brief Returns the count of attributes
 * @param file `FILE` File to read from
 * @param class_line `int` Line number of the class definition
 * @return `int` - Count of attributes of a class
 */
int get_attribute_count(FILE *file, int class_line) {
    rewind(file);

    if (!advance_n_lines(file, class_line)) {
        return -1;
    }

    char line[LINE_LENGTH];
    int counter = 0;

    while (fgets(line, sizeof(line), file) && !strstr(line, "{")) {
        strip_comment(line);
        if (isendline(line[0]) || strstr(line, "static")) {
            continue;
        }
        if (strstr(line, ";") && word_count(line) >= 2) {
            counter++;
        }
    }
    return counter;
}