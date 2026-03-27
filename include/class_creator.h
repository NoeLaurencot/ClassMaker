#ifndef CLASS_CREATOR_H
#define CLASS_CREATOR_H

#include "structs.h"

int get_n_dir_to_check(char *path);

int is_file_in_dir(char *filename, char *dir_path);

char *search_filepath_in_dirs(char *filename, int search_depth);

FILE *create_file(char* filename);

void init_class(void);

char *ask_class_name(class_t **class_arr, int n_class);

void set_import_arr(class_t *class);

void ask_create_parent_class(class_t **class_arr, int n_class);

void set_attributes_flag(class_t *class);

void create_attribute(class_t *class, int i, int vis_pref);

void create_class_attributes(class_t *class);

void set_class_from_user(class_t **class_arr, int n_class);

void create_class(class_t **class_arr, int n_class);

#endif
