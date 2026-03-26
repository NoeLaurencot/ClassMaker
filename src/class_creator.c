#include "class_creator.h"
#include "class_parser.h"
#include "enums.h"
#include "java_str.h"
#include "mem_check.h"
#include "scan_input.h"
#include "string_util.h"
#include "structs.h"
#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const int MAX_CLASS_NAME_LENGTH = 256;
static const int MAX_CLASS = 256;
static const int MAX_FILEPATH_LENGTH = 256;
static const int MAX_ATT_NAME_LENGTH = 256;
static const int MAX_ATT_TYPE_LENGTH = 256;
static const int MAX_DIR_NAME_LENGTH = 256;

/**
 * @brief Counts the number of directory levels in a path based on '/' count
 * @param path `char *` Pathname
 * @return `int` - Count of directory levels
 */
int get_n_dir_to_check(char *path) {
    int counter = 0;
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == '/')
            counter++;
    }
    return counter;
}

/**
 * @brief Checks if a file exists in a given directory
 * @param filename `char *` Name of the file to search
 * @param dir_path `char *` Directory in which to search the file
 * @retval `0` - Fail
 * @retval `1` - Success
 */
int is_file_in_dir(char *filename, char *dir_path) {
    DIR *dir = opendir(dir_path);
    mem_check(dir, "Coulnd't open directory");

    struct dirent *file = readdir(dir);
    file = readdir(dir);
    while (file) {
        if (strcmp(file->d_name, filename) == 0) {
            closedir(dir);
            return 1;
        }
        file = readdir(dir);
    }
    closedir(dir);
    return 0;
}

/**
 * @brief Searches for a file in parent directories up to n levels
 * @param filename `char *` Name of the file to search
 * @param search_depth `int` Depth of directories to search - 0 doesn't search at all
 * @return `char *` - Filename found, including the path. NULL if not found.
 */
char *search_filepath_in_dirs(char *filename, int search_depth) {
    mem_check(filename, "Invalid filename");
    char *filepath = stralloc(MAX_FILEPATH_LENGTH);
    malloc_check(filepath, "creating filepath");

    char *dir_to_check = stralloc(MAX_DIR_NAME_LENGTH);
    malloc_check(dir_to_check, "creating dir to check");
    strcpy(dir_to_check, "./");

    while (search_depth > 0 && is_file_in_dir(filename, dir_to_check) != 1) {
        search_depth--;
        strcat(dir_to_check, "../");
    }

    if (search_depth == 0) {
        free(filepath);
        free(dir_to_check);
        return NULL;
    }

    snprintf(filepath, 256, "%s%s", dir_to_check, filename);
    free(dir_to_check);
    return filepath;
}

void ask_create_parent_class(class_t *class, class_t **class_arr, int n_class) {
    printf("Is %s class inherited? (y/n)\n", class->class_name);
    if (scan_boolean()) {
        class->parent_class_arr = calloc(64, sizeof(class_t *));
        malloc_check(class->parent_class_arr, "creating parent class array");

        create_parent_class(class_arr, n_class);

        printf("%s done, going back to %s\n", class->parent_class_arr[1]->class_name, class->class_name);
    }
}

/**
 * @brief Creates and populates the attributes of an empty class that's just been created
 * @param class `class_t *` Class to populate
 * @note Populates class in place
 */
void create_class_attributes(class_t *class) {
    // TODO: refactor this function -> too big

    char *type_lookup_arr[] = {"int", "double", "String", "boolean"};
    char *vis_lookup_arr[] = {"private", "public", "protected"};

    printf("Attributes visibility of %s (default: Friendly)\n"
           "1. Private"
           "2. Public"
           "3. Protected"
           "4. Decide for each attribute",
           class->class_name);
    int vis_pref = scan_min_max_int(1, VIS_COUNT) - 1;

    printf("How many attributes for %s?\n", class->class_name);
    class->n_attributes = scan_unsigned_int();
    class->class_attribute_arr = calloc(class->n_attributes, sizeof(class_attribute_t *));
    malloc_check(class->class_attribute_arr, "creating attribute array");

    for (int i = 0; i < class->n_attributes; i++) {
        class_attribute_t *att = class->class_attribute_arr[i];

        att = calloc(1, sizeof(class_attribute_t));
        malloc_check(att, "creating class attribute");

        printf("Name of attribute n°%d of %s\n", i, class->class_name);
        att->att_name = scan_string(MAX_ATT_NAME_LENGTH);

        printf("Type of attribute n°%d of %s (default: Object)\n"
               "1. int\n"
               "2. double\n"
               "3. String\n"
               "4. boolean\n"
               "5. Custom\n",
               i, class->class_name);
        int answer = scan_min_max_int(1, TYPE_COUNT) - 1;
        if (answer <= TYPE_BOOLEAN) { // every standard type
            att->att_type = str_dup(type_lookup_arr[answer - 1]);
        } else if (answer == TYPE_CUSTOM) {
            printf("Custom type\n");
            att->att_type = scan_string(MAX_ATT_TYPE_LENGTH);
        } else { // default
            att->att_type = str_dup("Object");
        }
        malloc_check(att->att_type, "creating attribute type");

        if (vis_pref <= VIS_PROTECTED) { // every standard visibility
            att->att_vis = str_dup(vis_lookup_arr[vis_pref - 1]);
        } else if (vis_pref == VIS_CHOOSE_EACH) {
            printf("Visibility of attribute n°%d of %s (default: Friendly)\n"
                   "1. Private"
                   "2. Public"
                   "3. Protected",
                   i, class->class_name);
            att->att_vis = vis_lookup_arr[scan_min_max_int(1, 3) - 1];
        } else {
            att->att_vis = str_dup("");
        }
        malloc_check(att->att_vis, "creating attribute visibility");

        att->is_deep_array = is_deep_array(att);

        att->is_simple_array = is_simple_array(att);

        att->is_primitive = is_primitive(att);

        class->class_import_arr = calloc(32, sizeof(char *));
        malloc_check(class->class_import_arr, "creating class import array");

        if (att->is_simple_array || att->is_deep_array) {
            class->class_import_arr[i] = str_dup("java.util.Arrays");
        } else if (strstr(att->att_type, "ArrayList")) {
            class->class_import_arr[i] = str_dup("java.util.ArrayList");
        } else if (strstr(att->att_type, "List")) {
            class->class_import_arr[i] = str_dup("java.util.List");
        }
    }
}

void read_class(FILE *file, class_t *class) {
}

void create_parent_class(class_t **class_arr, int class_arr_n_element) {
    class_t **parent_class_arr;

    class_t *class = calloc(1, sizeof(class_t));
    malloc_check(class, "creating parent class");

    class->java_type = JAVA_TYPE_CLASS;

    class_arr[class_arr_n_element] = class;
    class_arr_n_element++;

    printf("What is the name of the parent class of %s?\n", class_arr[class_arr_n_element - 2]->class_name);
    class->class_name = scan_string(MAX_CLASS_NAME_LENGTH);
    first_letter_to_upper(class->class_name);

    char *filename = name_to_filename(class->class_name);
    char *cwd_src = get_cwd_src();
    class->class_package = get_package_name(cwd_src);
    class->relative_filepath = search_filepath_in_dirs(filename, get_n_dir_to_check(cwd_src));
    free(cwd_src);
    free(filename);

    if (class->relative_filepath) { // file exists
        FILE *file = fopen(class->relative_filepath, "r");
        mem_check(file, "error: couldn't open parent class file");

        int class_line = get_class_line(file);
        // TODO: handle different packages and friendly class
        if (get_class_final(file, class_line) || strstr(get_class_vis(file, class_line), "private")) {
            fclose(file);
            fprintf(stderr, "error: couldn't extend parent class as it was final or private");
            return;
        }
        create_class_attributes_from_file(file, class);
    } else { // file doesn't exist
        // TODO: ask user if wants to re enter class name
        printf("%s doesn't exist or wasn't found, do you want to create it? (y/n)", class->class_name);
        if (scan_boolean()) {
            class->relative_filepath = str_dup("./");

            ask_create_parent_class(class, class_arr, class_arr_n_element);

            create_class_attributes(class);
        } else {
            free(class->class_name);
            free(class->java_type);
            free(class);
            class_arr[class_arr_n_element - 1] = '\0';
            return;
        }
    }

    for (int i = 0; i < class_arr_n_element - 1; i++) {
        for (int j = 1; j < class_arr_n_element; j++) {
            class_arr[i]->parent_class_arr[j] = class;
            class_arr[i]->n_parent_class++;
        }
    }
}

/**
 * @brief Creates a new Java class and manages inheritance hierarchy
 */
void populate_class_from_user(void) {
    class_t **parent_class_arr;
    class_t **class_arr = calloc(MAX_CLASS, sizeof(class_t *));
    malloc_check(class_arr, "creating class array");

    class_t *class = calloc(1, sizeof(class_t));
    malloc_check(class, "creating class");

    class->java_type = JAVA_TYPE_CLASS;

    class_arr[0] = class;

    printf("What is the class name?\n");
    class->class_name = scan_string(MAX_CLASS_NAME_LENGTH);

    char *filename = name_to_filename(class->class_name);
    class->relative_filepath = concat_str("./", filename);
    free(filename);
    malloc_check(class->relative_filepath, "creating relative filepath");
    first_letter_to_upper(class->class_name);

    FILE *tmp_file = fopen(class->relative_filepath, "r");
    if (tmp_file) {
        fprintf(stderr, "error: file already exists");
        fclose(tmp_file);
        exit(1); // TODO: free everything before exiting for cleaner exit
    }

    char *cwd_src = get_cwd_src();
    class->class_package = get_package_name(cwd_src);
    free(cwd_src);

    printf("Is %s class final? (y/n)\n", class->class_name);
    if (scan_boolean()) {
        class->is_final = 1;
    } else {
        class->is_final = 0;
    }

    ask_create_parent_class(class, class_arr, 1); // 1 because only current class in class array

    create_class_attributes(class);
}

void init_class() {
    class_t **class_arr = calloc(MAX_CLASS, sizeof(class_t *));
    malloc_check(class_arr, "creating class array");

    create_class(class_arr, 0);
}

void create_class(class_t **class_arr, int n_class) {
    class_t *class = calloc(1, sizeof(class_t));
    malloc_check(class, "creating class");

    class_arr[n_class] = class;
    n_class++;

    class->java_type = JAVA_TYPE_CLASS;

    if (n_class == 1)
        printf("What is the class name?\n");
    else
        printf("What is the name of the parent class of %s?\n", class_arr[n_class - 2]->class_name);

    class->class_name = scan_string(MAX_CLASS_NAME_LENGTH);
    first_letter_to_upper(class->class_name);

    char *filename = name_to_filename(class->class_name);
    char *cwd_src = get_cwd_src();
    class->class_package = get_package_name(cwd_src);
    class->relative_filepath = search_filepath_in_dirs(filename, get_n_dir_to_check(cwd_src));
    free(cwd_src);

    if (n_class == 1) {
        if (is_file_in_dir(filename, "./")) {
            fprintf(stderr, "error: file already exists");
            exit(1);
        }
    } else {
        if (class->relative_filepath) {
            // TODO: read class
        } else {
            printf("%s doesn't exist, do you want to create it? (y/n)", class->class_name);
            if (!scan_boolean()) { // parent class not created
                free(class->java_type);
                if (class->class_package)
                    free(class->class_package);
                free(class->class_name);
                free(class);
                class_arr[n_class - 1] = '\0';
                n_class--;
                return;
            }

            create_class(class_arr, n_class);
        }
    }
}