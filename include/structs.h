#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include "enums.h"

typedef struct class_attribute_t {
    char *att_name;
    char *att_type;
    char *att_vis;
    int is_final;
    int is_primitive;
    int is_simple_array;
    int is_deep_array;
} class_attribute_t;

typedef struct class_t {
    java_type_t java_type;
    int is_final;
    char *class_name;
    char *class_vis;
    char *relative_filepath;
    class_attribute_t **class_attribute_arr;
    int n_attributes;
    struct class_t **parent_class_arr;
    int n_parent_class;
    FILE *file;
} class_t;

#endif
