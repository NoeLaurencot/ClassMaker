#ifndef STRING_UTIL_H
#define STRING_UTIL_H

void *stralloc(int size);

void strip_comment(char *line);

char *str_dup(const char *string);

int isendline(char c);

char *create_str_formated_str(char *format, char *string);

void first_letter_to_upper(char *string);

char *first_letter_to_upper_dup(char *string);

void first_letter_to_lower(char *string);

char *first_letter_to_lower_dup(char *string);

char *concat_str(char *dest, char *src);

char *strstrdup(char *haystack, char *needle);

char *strstrdup_trunc(char *haystack, char *needle);

int search_closing_i(char *string, char start);

int get_nth_word_i(char *string, int n_word);

int get_nth_word_end_i(char *string, int n_word);

char *get_first_word(char *string);

char *get_nth_word(char *string, int n_word);

char *get_next_word(char *string);

char *get_nth_last_word(char *string, int n_word);

char *get_last_word(char *string);

int word_count(char *string);

#endif
