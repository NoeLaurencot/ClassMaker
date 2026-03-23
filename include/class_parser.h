#ifndef CLASS_PARSER_H
#define CLASS_PARSER_H

int advance_n_lines(FILE *file, int n);

int get_class_line(FILE *file);

char *extract_nth_line(FILE *file, int n);

char *get_class_package(FILE *file);

int get_class_final(FILE *file, int class_line);

char *get_class_vis(FILE *file, int class_line);

char *get_class_name(FILE *file, int class_line);

char *get_parent_class_name(FILE *file, int class_line);

int get_n_attribute(FILE *file, int class_line);

class_attribute_t **get_class_attributes(FILE *file, int class_line, int n_att);

#endif
