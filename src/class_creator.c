#include "class_creator.h"
#include "class_parser.h"
#include "class_writer.h"
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

static const int MAX_class_name_upper_LENGTH = 256;
static const int MAX_CLASS = 256;
static const int MAX_FILEPATH_LENGTH = 256;
static const int MAX_ATT_NAME_LENGTH = 256;
static const int MAX_ATT_TYPE_LENGTH = 256;
static const int MAX_DIR_NAME_LENGTH = 256;
static const int MAX_IMPORTS = 32;

// Forward declerations

void create_class(class_t **class_arr, int n_class);

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

FILE *create_file(char *filename) {
    FILE *file = fopen(filename, "wx");
    mem_check(file, "error: couldn't create file");
    return file;
}

void init_class(void) {
    class_t **class_arr = calloc(MAX_CLASS, sizeof(class_t *));
    malloc_check(class_arr, "creating class array");

    create_class(class_arr, 0);
}

char *ask_class_name_upper(class_t **class_arr, int n_class) {

    if (n_class == 1)
        printf("What is the class name?\n");
    else
        printf("What is the name of the parent class of %s?\n", class_arr[n_class - 2]->class_name_upper);

    return scan_string(MAX_class_name_upper_LENGTH);
}

void set_import_arr(class_t *class) {
    char **import_arr = calloc(MAX_IMPORTS, sizeof(char *));
    malloc_check(import_arr, "creating class import array");
    int counter = 0;

    int arrays_imported = 0;
    int arraylist_imported = 0;
    int linkedlist_imported = 0;
    int list_imported = 0;
    int hashmap_imported = 0;
    int map_imported = 0;
    int hashset_imported = 0;
    int set_imported = 0;
    int deque_imported = 0;
    int queue_imported = 0;
    int optional_imported = 0;
    int localdatetime_imported = 0;
    int localdate_imported = 0;
    int date_imported = 0;

    for (int i = 0; i < class->n_attribute; i++) {
        class_attribute_t *att = class->class_attribute_arr[i];

        // TODO: redo the check but right now it works
        // Checks most of data type related import
        if ((att->is_simple_array || att->is_deep_array) && !arrays_imported) {
            import_arr[counter++] = str_dup("java.util.Arrays");
            arrays_imported = 1;
        } else if (strstr(att->att_type, "ArrayList") && !arraylist_imported) {
            import_arr[counter++] = str_dup("java.util.ArrayList");
            arraylist_imported = 1;
        } else if (strstr(att->att_type, "LinkedList") && !linkedlist_imported) {
            import_arr[counter++] = str_dup("java.util.LinkedList");
            linkedlist_imported = 1;
        } else if (strstr(att->att_type, "List") && !list_imported) {
            import_arr[counter++] = str_dup("java.util.List");
            list_imported = 1;
        } else if (strstr(att->att_type, "HashMap") && !hashmap_imported) {
            import_arr[counter++] = str_dup("java.util.HashMap");
            hashmap_imported = 1;
        } else if (strstr(att->att_type, "Map") && !map_imported) {
            import_arr[counter++] = str_dup("java.util.Map");
            map_imported = 1;
        } else if (strstr(att->att_type, "HashSet") && !hashset_imported) {
            import_arr[counter++] = str_dup("java.util.HashSet");
            hashset_imported = 1;
        } else if (strstr(att->att_type, "Set") && !set_imported) {
            import_arr[counter++] = str_dup("java.util.Set");
            set_imported = 1;
        } else if (strstr(att->att_type, "Deque") && !deque_imported) {
            import_arr[counter++] = str_dup("java.util.Deque");
            deque_imported = 1;
        } else if (strstr(att->att_type, "Queue") && !queue_imported) {
            import_arr[counter++] = str_dup("java.util.Queue");
            queue_imported = 1;
        } else if (strstr(att->att_type, "Optional") && !optional_imported) {
            import_arr[counter++] = str_dup("java.util.Optional");
            optional_imported = 1;
        } else if (strstr(att->att_type, "LocalDateTime") && !localdatetime_imported) {
            import_arr[counter++] = str_dup("java.time.LocalDateTime");
            localdatetime_imported = 1;
        } else if (strstr(att->att_type, "LocalDate") && !localdate_imported) {
            import_arr[counter++] = str_dup("java.time.LocalDate");
            localdate_imported = 1;
        } else if (strstr(att->att_type, "Date") && !date_imported) {
            import_arr[counter++] = str_dup("java.util.Date");
            date_imported = 1;
        }
    }
    class->n_import = counter;
    if (counter > 0)
        class->class_import_arr = import_arr;
    else
        free(import_arr);
}

void ask_create_parent_class(class_t **class_arr, int n_class) {
    class_t *class = class_arr[n_class - 1];

    printf("Is %s class inherited? (y/n)\n", class->class_name_upper);
    if (scan_boolean()) {
        class->parent_class_arr = calloc(MAX_CLASS, sizeof(class_t *));
        malloc_check(class->parent_class_arr, "creating parent class array");

        create_class(class_arr, n_class);

        printf("%s done, going back to %s\n", class->parent_class_arr[1]->class_name_upper, class->class_name_upper);
    } else { // stop recursion, fill each children parent class array with every parent class
        // each children class
        for (int i = 0; i < n_class - 1; i++) {
            // fill child parent class array
            for (int j = 1; j < n_class; j++) {
                class_arr[i]->parent_class_arr[j] = class;
                class_arr[i]->n_parent_class++;
            }
        }
    }
}

void set_class_vis(class_t *class) {
    char *vis_lookup_arr[] = {"public", "private", "protected"};

    printf("Visibility of \"%s\"\n"
        "1. Public\n"
        "2. Private\n"
        "3. Protected\n",
        class->class_name_upper);
    class->class_vis = str_dup(vis_lookup_arr[scan_min_max_int(1, 3) - 1]);
}

void set_attributes_flag(class_t *class) {
    for (int i = 0; i < class->n_attribute; i++) {
        class_attribute_t *att = class->class_attribute_arr[i];

        att->is_deep_array = is_deep_array(att);

        att->is_simple_array = is_simple_array(att);

        att->is_primitive = is_primitive(att);
    }
}

void create_attribute(class_t *class, int i, int vis_pref) {

    char *type_lookup_arr[] = {"int", "double", "String", "boolean"};
    char *vis_lookup_arr[] = {"public", "private", "protected"};

    class->class_attribute_arr[i] = calloc(1, sizeof(class_attribute_t));

    class_attribute_t *att = class->class_attribute_arr[i];
    malloc_check(att, "creating class attribute");

    printf("Name of attribute n°%d of %s\n", i + 1, class->class_name_upper);
    att->att_name = scan_string(MAX_ATT_NAME_LENGTH);

    printf("Type of attribute \"%s\"\n"
           "1. int\n"
           "2. double\n"
           "3. String\n"
           "4. boolean\n"
           "5. Custom\n",
           att->att_name);
    int att_type_answer = scan_min_max_int(1, TYPE_COUNT) - 1;
    if (att_type_answer <= TYPE_BOOLEAN) { // option 1 -> 4
        att->att_type = str_dup(type_lookup_arr[att_type_answer]);
    } else if (att_type_answer == TYPE_CUSTOM) {
        // set custom type
        printf("Custom type\n");
        att->att_type = scan_string(MAX_ATT_TYPE_LENGTH);
    } else { // default
        att->att_type = str_dup("Object");
    }
    malloc_check(att->att_type, "creating attribute type");

    if (vis_pref <= VIS_PROTECTED) { // global visibility option 1 -> 3
        att->att_vis = str_dup(vis_lookup_arr[vis_pref]);
    } else if (vis_pref == VIS_CHOOSE_EACH) {
        // set visibility for specific attribute
        printf("Visibility of attribute \"%s\"\n"
               "1. Public\n"
               "2. Private\n"
               "3. Protected\n",
               att->att_name);
        att->att_vis = str_dup(vis_lookup_arr[scan_min_max_int(1, 3) - 1]);
    } else {
        att->att_vis = NULL;
    }
    malloc_check(att->att_vis, "creating attribute visibility");
}

/**
 * @brief Creates and populates the attributes of an empty class that's just been created
 * @param class `class_t *` Class to populate
 * @note Populates class in place
 */
void create_class_attributes(class_t *class) {
    // set attribute count
    printf("How many attributes for %s?\n", class->class_name_upper);
    class->n_attribute = scan_unsigned_int();
    if (class->n_attribute == 0)
        return;
    class->class_attribute_arr = calloc(class->n_attribute, sizeof(class_attribute_t *));
    malloc_check(class->class_attribute_arr, "creating attribute array");

    // set global visibility
    printf("Attributes visibility of %s\n"
           "1. Private\n"
           "2. Public\n"
           "3. Protected\n"
           "4. Decide for each attribute\n",
           class->class_name_upper);
    int vis_pref = scan_min_max_int(1, VIS_COUNT) - 1;

    // main attribute creating loop
    for (int i = 0; i < class->n_attribute; i++) {
        create_attribute(class, i, vis_pref);
    }
}

void set_class_from_user(class_t **class_arr, int n_class) {
    class_t **parent_class_arr;

    class_t *class = class_arr[n_class - 1];

    ask_create_parent_class(class_arr, n_class);

    printf("Is %s class final? (y/n)\n", class->class_name_upper);
    if (scan_boolean()) {
        class->is_final = 1;
    } else {
        class->is_final = 0;
    }

    set_class_vis(class);

    create_class_attributes(class);
    set_attributes_flag(class);
    set_import_arr(class);
}

void create_class(class_t **class_arr, int n_class) {
    class_t *class = calloc(1, sizeof(class_t));
    malloc_check(class, "creating class");

    class_arr[n_class] = class;
    n_class++;

    class->java_type = JAVA_TYPE_CLASS;

    class->class_name_lower = ask_class_name_upper(class_arr, n_class);
    class->class_name_upper = first_letter_to_upper_dup(class->class_name_lower);

    class->filename = name_to_filename(class->class_name_upper);
    char *cwd_src = get_cwd_src();

    if (n_class == 1) {
        class->class_package = get_package_name(cwd_src);

        class->relative_filename = concat_str("./", class->filename);
        if (is_file_in_dir(class->filename, "./")) {
            fprintf(stderr, "error: file already exists\n");
            // TODO: free everything before exiting for cleaner exit
            exit(1);
        }
    } else {
        class->relative_filename = search_filepath_in_dirs(class->filename, get_n_dir_to_check(cwd_src));
    }

    free(cwd_src);

    if (n_class > 1) {
        if (class->relative_filename) { // file found
            // TODO: read class
        } else { // file not found
            printf("%s doesn't exist or wasn't found, do you want to create it? (y/n)", class->class_name_upper);
            if (!scan_boolean()) { // parent class not created so undo class structure
                if (class->class_package)
                    free(class->class_package);
                free(class->class_name_upper);
                free(class);
                class_arr[n_class - 1] = '\0';
                n_class--;
                return;
            }

            set_class_from_user(class_arr, n_class);
        }
    } else {
        set_class_from_user(class_arr, n_class);
    }

    class->file = create_file(class->relative_filename);

    // DEBUG //

    printf(
        "package: %s, is final: %d, name: %s, class vis: %s, filename: %s, relative filename: %s\nimport count: %d\n",
        class->class_package, class->is_final, class->class_name_upper, class->class_vis, class->filename,
        class->relative_filename, class->n_import);
    for (int i = 0; i < class->n_import; i++) {
        printf("import %d: %s\n", i, class->class_import_arr[i]);
    }
    printf("attribute count: %d\n", class->n_attribute);
    for (int i = 0; i < class->n_attribute; i++) {
        printf("attribute %d: %s %s %s\n", i+1, class->class_attribute_arr[i]->att_vis,
               class->class_attribute_arr[i]->att_type, class->class_attribute_arr[i]->att_name);
    }
    printf("parent class count: %d\n", class->n_parent_class);
    for (int i = 0; i < class->n_parent_class; i++) {
        printf("parent class %d: %s\n", i+1, class->parent_class_arr[i]->filename);
    }

    ///////////

    write_to_class_file(class);
}
