#include "java_str.h"
#include "mem_check.h"
#include "string_util.h"
#include "structs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const int MAX_PATH_LENGTH = 512;

/**
 * @brief Create a file with .java extension
 * @param name `char *` Name of the file that needs .java extension
 * @return `char *` - File name with .java extension
 */
char *name_to_filename(char *name) {
    char *file_name = stralloc(strlen(name) + 5);
    malloc_check(file_name, "creating filename from name");
    snprintf(file_name, strlen(name) + 6, "%s.java", name);
    return file_name;
}

/**
 * @brief Gets current working directory truncated at src directory
 * @note Return "./" if couln't find src directory
 * @return `char *` - Current working directory from src directory
 */
char *get_cwd_src(void) {
    int size = MAX_PATH_LENGTH;
    char *full_cwd = stralloc(size);
    malloc_check(full_cwd, "creating full cwd");

    if (getcwd(full_cwd, size) == NULL) {
        fprintf(stderr, "Couldn't get current working directory\n");
        exit(1);
    }

    char *src_path = strstr(full_cwd, "/src");
    char *cwd = stralloc(size);
    malloc_check(cwd, "creating cwd from src");

    if (src_path) {
        strcpy(cwd, src_path);
    } else {
        strcpy(cwd, "./");
    }
    mem_check(cwd, "error: copying cwd from src to new buffer failed");

    free(full_cwd);
    return cwd;
}

/**
 * @brief Gets Java package name
 * @param path_from_src `char *` Path from src directory
 * @return `char *` - Java package name
 */
char *get_package_name(char *path_from_src) {
    if (!path_from_src || strcmp(path_from_src, "./") == 0) {
        return NULL;
    }
    char *pkg = path_from_src + 4;

    char *result = stralloc(strlen(pkg));
    malloc_check(result, "creating package name");

    strcpy(result, pkg);
    mem_check(result, "error: copying pkg to result failed");

    if (result[strlen(result) - 1] == '/') {
        result[strlen(result) - 1] = '\0';
    }

    for (int i = 0; i < strlen(result); i++) {
        if (result[i] == '/')
            result[i] = '.';
    }
    return result;
}

int is_deep_array(class_attribute_t *attribute) {
    char *type = attribute->att_type;
    return ((!strstr(type, ">") && strstr(type, "[][]"))) ||
           (strstr(type, "<") && strstr(type, ">") && strstr(strstr(type, ">"), "[][]"));
}

int is_simple_array(class_attribute_t *attribute) {
    char *type = attribute->att_type;
    return ((!strstr(type, ">") && strstr(type, "[][]")) ||
            (strstr(type, "<") && strstr(type, ">") && strstr(strstr(type, ">"), "[]") &&
             !strstr(strstr(type, ">"), "[][]")));
}

int is_primitive(class_attribute_t *attribute) {
    char *type = attribute->att_type;
    return (islower(type[0]) && !(strstr(type, "<") && strstr(type, ">")) || !(strstr(type, "[") && strstr(type, "]")));
}

int is_simple_object(class_attribute_t *attribute) {
    char *type = attribute->att_type;
    return (isupper(type[0]) && !is_deep_array(attribute) && !is_simple_array(attribute));
}