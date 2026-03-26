#ifndef CLASS_CREATOR_H
#define CLASS_CREATOR_H

int get_n_dir_to_check(char *path);

int is_file_in_dir(char *filename, char* dir_path);

char *search_filepath_in_dirs(char* filename, int search_depth);

void populate_empty_class_attribute(class_t *class);

void create_parent_class(class_t **class_arr, int class_arr_n_element);

void create_class(void);

#endif
