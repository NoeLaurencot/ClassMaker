#ifndef JAVA_STR_H
#define JAVA_STR_H

#include "structs.h"

char *name_to_filename(char *name);

char *get_cwd_src(void);

char *get_package_name(char *path_from_src);

int is_deep_array(class_attribute_t *attribute);

int is_simple_array(class_attribute_t *attribute);

int is_primitive(class_attribute_t *attribute);

int is_simple_object(class_attribute_t *attribute);

#endif
