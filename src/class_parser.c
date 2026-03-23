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

#define LINE_LENGTH 512

/**
 * @brief Advances by n lines in given file
 * @note The next fgets() call will return line n+1
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

    char *line = stralloc(512);
    malloc_check(line, "creating file line");

    if (!advance_n_lines(file, n)) {
        free(line);
        return NULL;
    }
    if (!fgets(line, 512, file)) {
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
char *get_class_name(FILE *file, int class_line) {
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
char *get_parent_class_name(FILE *file, int class_line) {
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
int get_n_attribute(FILE *file, int class_line) {
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

/**
 * @brief Get array containing the class attribute pointers
 * @param file `FILE` File to read from
 * @param class_line `int` Line number of the class definition
 * @param n_att `int` Count of attributes
 * @return `class_attribute_t **` - Class attribute pointer array, NULL if failed
 */
class_attribute_t **get_class_attributes(FILE *file, int class_line, int n_att) {
    // TODO: refactor this function -> should be in class_creator instead

    rewind(file);
    
    char line[LINE_LENGTH];
    if (!advance_n_lines(file, class_line)) {
        return NULL;
    }

    class_attribute_t **attribute_arr = calloc(n_att, sizeof(class_attribute_t *));
    malloc_check(attribute_arr, "creating class attribute array");

    int i = 0;
    while (fgets(line, sizeof(line), file) && !strstr(line, "{")) {
        strip_comment(line);
        if (isendline(line[0]) || strstr(line, "static")) {
            continue;
        }
        if (strstr(line, ";") && word_count(line) >= 2) {
            class_attribute_t *attribute = calloc(1, sizeof(class_attribute_t));
            malloc_check(attribute, "creating class attribute");

            attribute_arr[i] = attribute;

            if (strstr(line, "final")) {
                attribute->is_final = 1;
            } else {
                attribute->is_final = 0;
            }
            if (strstr(line, "public")) {
                attribute->att_vis = str_dup("public");
            } else if (strstr(line, "private")) {
                attribute->att_vis = str_dup("private");
            } else if (strstr(line, "protected")) {
                attribute->att_vis = str_dup("protected");
            } else {
                attribute->att_vis = str_dup("");
            }

            attribute->att_name = get_last_word(line);
            attribute->att_type = get_nth_last_word(line, 2);
            attribute->is_deep_array = 0;
            attribute->is_simple_array = 0;
            attribute->is_primitive = 0;

            if (strstr(attribute->att_type, "[][]") &&
                !(strstr(attribute->att_type, "<") || strstr(attribute->att_type, ">"))) {
                attribute->is_deep_array = 1;
            } else if (strstr(attribute->att_type, "[]") &&
                       !(strstr(attribute->att_type, "<") || strstr(attribute->att_type, ">"))) {
                attribute->is_simple_array = 1;
            } else if (islower(attribute->att_type[0])) {
                attribute->is_primitive = 1;
            }
            i++;
        }
    }
    return attribute_arr;
}