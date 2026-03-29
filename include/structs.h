#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include "enums.h"

// TODO: add att_type strcut for better hashCode handling

typedef struct class_attribute_t {
    char *att_name_upper;
    char *att_name_lower;
    char *att_type;
    char *att_vis;
    int is_final;
    int is_primitive;
    int is_simple_object;
    int is_simple_array;
    int is_deep_array;
} class_attribute_t;

typedef struct class_t {
    java_type_t java_type;
    int is_final;
    char *class_name_upper;
    char *class_name_lower;
    char *class_vis;
    char *filename;
    char *relative_filename;
    char **class_import_arr;
    int n_import;
    char *class_package;
    class_attribute_t **class_attribute_arr;
    int n_attribute;
    struct class_t **parent_class_arr;
    int n_parent_class;
    int total_parent_att;
    FILE *file;
} class_t;

#endif
