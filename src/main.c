#include "enums.h"
#include "help.h"
#include "structs.h"
#include "class_creator.h"
#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

java_type_t parse_arguments_for_type(int argc, char *argv[]) {
    static struct option long_options[] = {{"class", no_argument, 0, 'c'},      {"interface", no_argument, 0, 'i'},
                                          {"record", no_argument, 0, 'r'},     {"enum", no_argument, 0, 'e'},
                                          {"annotation", no_argument, 0, 'a'}, {"exception", no_argument, 0, 'E'},
                                          {"help", no_argument, 0, 'h'},       {0, 0, 0, 0}};

    int opt;
    int opt_index = 0;
    int opt_count = 0;
    int opt_found = 0;
    char stored_opt;
    while ((opt = getopt_long(argc, argv, "cireaEh", long_options, &opt_index)) != -1) {
        switch (opt) {
        case 'c':
        case 'i':
        case 'r':
        case 'e':
        case 'a':
        case 'E':
        case 'h':
            if (opt_found) {
                fprintf(stderr, "error: too many options specified\n");
                fprintf(stderr, HELP_TRY);
                exit(1);
            }
            stored_opt = opt;
            opt_found = 1;
            opt_count++;
            break;
        case '?':
            fprintf(stderr, HELP_MESSAGE, 'c', "class", "class_t");
            fprintf(stderr, HELP_MESSAGE, 'i', "interface", "Interface");
            fprintf(stderr, HELP_MESSAGE, 'r', "record", "Record");
            fprintf(stderr, HELP_MESSAGE, 'e', "enum", "Enum");
            fprintf(stderr, HELP_MESSAGE, 'a', "annotation", "Annotation");
            fprintf(stderr, HELP_MESSAGE, 'E', "exception", "Exception");
            fprintf(stderr, HELP_DISPLAY, 'h', "help");
            exit(1);
        }
    }

    if (opt_count == 0) {
        fprintf(stderr, HELP_USAGE);
        fprintf(stderr, HELP_TRY);
        exit(1);
    }

    switch (stored_opt) {
    case 'c':
        return JAVA_TYPE_CLASS;
    case 'i':
        return JAVA_TYPE_INTERFACE;
    case 'r':
        return JAVA_TYPE_RECORD;
    case 'e':
        return JAVA_TYPE_ENUM;
    case 'a':
        return JAVA_TYPE_ANNOTATION;
    case 'E':
        return JAVA_TYPE_EXCEPTION;
    case 'h':
        fprintf(stderr, HELP_USAGE);
        fprintf(stderr, HELP_DESCRIPTION);
        printf(HELP_MESSAGE, 'c', "class", "Class");
        printf(HELP_MESSAGE, 'i', "interface", "Interface");
        printf(HELP_MESSAGE, 'r', "record", "Record");
        printf(HELP_MESSAGE, 'e', "enum", "Enum");
        printf(HELP_MESSAGE, 'a', "annotation", "Annotation");
        printf(HELP_MESSAGE, 'E', "exception", "Exception");
        printf(HELP_DISPLAY, 'h', "help");
        return JAVA_TYPE_NONE;
    }
}

void free_all(void *) {

}

int main(int argc, char *argv[]) {
    java_type_t java_type = parse_arguments_for_type(argc, argv);
    if (java_type == JAVA_TYPE_NONE) {
        exit(0);
    }
    switch (java_type) {
    case JAVA_TYPE_CLASS:
        create_class();
        break;
    case JAVA_TYPE_INTERFACE:
        break;
    case JAVA_TYPE_RECORD:
        break;
    case JAVA_TYPE_ENUM:
        break;
    case JAVA_TYPE_ANNOTATION:
        break;
    case JAVA_TYPE_EXCEPTION:
        break;
    case JAVA_TYPE_NONE:
        exit(0); // the user used help option
    default:
        fprintf(stderr, "error: file type not recognised");
        fprintf(stderr, HELP_TRY);
    }
}
