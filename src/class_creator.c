#include "class_creator.h"
#include "enums.h"
#include "structs.h"
#include "scan_input.h"
#include "mem_check.h"
#include "string_util.h"
#include "java_str.h"
#include "class_parser.h"
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
 * @retval `0` - Failed
 * @retval `1` - Success
 */
int is_file_in_dir(char *filename, char* dir_path) {
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
char *search_filepath_in_dirs(char* filename, int search_depth) {
    mem_check(filename, "Invalid filename");
    char *filepath = stralloc(MAX_FILEPATH_LENGTH);
    malloc_check(filepath, "creating filepath");

    char *dir_to_check = stralloc(128);
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

/**
 * @brief Populates the attributes of an empty class that's just been created
 * @param class `class_t *` Class to populate
 * @note Populates class in place
 */
void populate_empty_class_attribute(class_t *class) {
    // TODO: refactor this function -> too big
    // TODO: fix attribute_arr[i] has to be initialized

    char *type_lookup_arr[] = {"int", "double", "String", "boolean"};
    char *vis_lookup_arr[] = {"private", "public", "protected"};

    printf(
        "Attributes visibility of %s (default: Friendly)\n"
        "1. Private"
        "2. Public"
        "3. Protected"
        "4. Decide for each attribute",
        class->class_name
    );
    int vis_pref = scan_min_max_int(1, VIS_COUNT) - 1;

    printf("How many attributes for %s?\n", class->class_name);
    class->n_attributes = scan_unsigned_int();
    class->class_attribute_arr = calloc(class->n_attributes, sizeof(class_attribute_t *));
    malloc_check(class->class_attribute_arr, "creating attribute array");

    for (int i = 0; i < class->n_attributes; i++) {
        printf("Name of attribute n°%d of %s\n", i, class->class_name);
        class->class_attribute_arr[i]->att_name = scan_string(MAX_ATT_NAME_LENGTH);

        printf(
            "Type of attribute n°%d of %s (default: Object)\n"
            "1. int\n"
            "2. double\n"
            "3. String\n"
            "4. boolean\n"
            "5. Custom\n",
            i, class->class_name
        );
        int answer = scan_min_max_int(1, TYPE_COUNT) - 1;
        if (answer <= TYPE_BOOLEAN) { // every standard type
            class->class_attribute_arr[i]->att_type = str_dup(type_lookup_arr[answer - 1]);
        } else if (answer == TYPE_CUSTOM) {
            printf("Custom type\n");
            class->class_attribute_arr[i]->att_type = scan_string(MAX_ATT_TYPE_LENGTH);
        } else { // default
            class->class_attribute_arr[i]->att_type = str_dup("Object");
        }
        malloc_check(class->class_attribute_arr[i]->att_type, "creating attribute type");

        if (vis_pref <= VIS_PROTECTED) { // every standard visibility
            class->class_attribute_arr[i]->att_vis = str_dup(vis_lookup_arr[vis_pref - 1]);
        } else if (vis_pref == VIS_CHOOSE_EACH) {
            printf(
                "Visibility of attribute n°%d of %s (default: Friendly)\n"
                "1. Private"
                "2. Public"
                "3. Protected",
                i, class->class_name
            );
            class->class_attribute_arr[i]->att_vis = vis_lookup_arr[scan_min_max_int(1, 3) - 1];
        } else {
            class->class_attribute_arr[i]->att_vis = str_dup("");
        }
        malloc_check(class->class_attribute_arr[i]->att_vis, "creating attribute visibility");


    }
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
    camel_to_pascal(class->class_name);

    char *filename = name_to_filename(class->class_name);
    char *cwd_src = get_cwd_src();
    class->relative_filepath = search_filepath_in_dirs(filename, get_n_dir_to_check(cwd_src));
    free(cwd_src);
    free(filename);

    if (!class->relative_filepath) {
        printf("%s doesn't exist, do you want to create it?", class->class_name);
         if (scan_boolean()) {
            class->relative_filepath = str_dup("./");
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
void create_class() {
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
    camel_to_pascal(class->class_name);

    FILE *tmp = fopen(class->relative_filepath, "wx");
    mem_check(tmp, "Error: the file already exists or cannot be created.");
    fclose(tmp);

    printf("Is %s class final? (y/n)\n", class->class_name);
    if (scan_boolean()) {
        class->is_final = 1;
    } else {
        class->is_final = 0;
    }

    printf("Is %s class inherited? (y/n)\n", class->class_name);
    if (scan_boolean()) {
        class->parent_class_arr = calloc(64, sizeof(class_t *));
        malloc_check(class->parent_class_arr, "creating parent class array");
        create_parent_class(class_arr, 1); // 1 because only current class in class array
    }

    populate_empty_class_attribute(class);
}