#ifndef ENUMS_H
#define ENUMS_H

typedef enum visibility_pref_t {
    VIS_PUBLIC,
    VIS_PRIVATE,
    VIS_PROTECTED,
    VIS_CHOOSE_EACH,
    VIS_COUNT
} visibility_pref_t;

typedef enum java_type_t {
    JAVA_TYPE_CLASS,
    JAVA_TYPE_INTERFACE,
    JAVA_TYPE_RECORD,
    JAVA_TYPE_ENUM,
    JAVA_TYPE_ANNOTATION,
    JAVA_TYPE_EXCEPTION,
    JAVA_TYPE_NONE
} java_type_t;

typedef enum data_type_t {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_BOOLEAN,
    TYPE_CUSTOM,
    TYPE_COUNT
} data_type_t;

#endif
