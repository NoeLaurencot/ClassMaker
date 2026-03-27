#include "class_writer.h"
#include "enums.h"
#include "java_str.h"
#include "mem_check.h"
#include "scan_input.h"
#include "string_util.h"
#include "structs.h"
#include "template.h"
#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void write_to_class_file(class_t *class) {

    // unpacking file
    FILE *file = class->file;

    // package
    if (class->class_package)
        fprintf(file, PACKAGE, class->class_package);

    // imports
    for (int i = 0; i < class->n_import; i++)
        fprintf(file, IMPORT, class->class_import_arr[i]);

    // class
    if (class->is_final != 0)
        fprintf(file, "final ");
    if (class->class_vis)
        fprintf(file, "%s ", class->class_vis);
    if (class->parent_class_arr)
        fprintf(file, CLASS_WI, class->class_name_upper, class->parent_class_arr[1]->class_name_upper);
    else
        fprintf(file, CLASS, class->class_name_upper);

    // attributes
    for (unsigned int i = 0; i < class->n_attribute; i++) {
        class_attribute_t *att = class->class_attribute_arr[i];
        if (att->att_vis)
            fprintf(file, ATTRIBUTE_WVIS, att->att_vis, att->att_type, att->att_name_lower);
        else
            fprintf(file, ATTRIBUTE, att->att_type, att->att_name_lower);
    }

    // empty constructor
    fprintf(file, "\n");
    fprintf(file, EMPTY_CONSTR, class->class_name_upper);

    // full constructor
    if (class->n_attribute > 0 || class->n_parent_class > 0) {
        fprintf(file, "\n");
        fprintf(file, CONSTR_OPEN, class->class_name_upper);

        if (class->n_parent_class > 0) {
            for (int i = class->n_parent_class; i > 0; i--) {
                class_t *parent_class = class->parent_class_arr[i];
                for (int j = 0; j < parent_class->n_attribute; j++) {
                    class_attribute_t *att = parent_class->class_attribute_arr[i];
                    fprintf(file, CONSTR_PARAMETER, att->att_type, att->att_name_lower);
                    if (i != class->total_parent_att - 1 || class->n_attribute > 0) {
                        fprintf(file, ", ");
                    }
                }
            }
        }

        for (int i = 0; i < class->n_attribute; i++) {
            fprintf(file, CONSTR_PARAMETER, class->class_attribute_arr[i]->att_type,
                    class->class_attribute_arr[i]->att_name_lower);
            if (i != class->n_attribute - 1) {
                fprintf(file, ", ");
            }
        }
        fprintf(file, ") {\n");

        if (class->n_parent_class > 0) {
            fprintf(file, CONSTR_SUPER_OPEN);
            for (int i = class->n_parent_class - 1; i >= 0; i--) {
                class_t *parent_class = class->parent_class_arr[i];
                for (int j = 0; j < parent_class->n_attribute; j++) {
                    class_attribute_t *att = parent_class->class_attribute_arr[i];
                    fprintf(file, CONSTR_SUPER_PARAMETER, parent_class->class_attribute_arr[i]->att_name_lower);
                    if (i != class->total_parent_att - 1) {
                        fprintf(file, ", ");
                    }
                }
            }
            fprintf(file, ");\n");
        }

        for (int i = 0; i < class->n_attribute; i++) {
            fprintf(file, CONSTR_CONTENT, class->class_attribute_arr[i]->att_name_lower,
                    class->class_attribute_arr[i]->att_name_lower);
        }

        fprintf(file, "\t}\n");
    }

    // copy constructor
    if (class->n_attribute > 0 || class->n_parent_class > 0) {
        fprintf(file, "\n");
        fprintf(file, CONSTR_OPEN, class->class_name_upper);
        fprintf(file, CONSTR_PARAMETER_COPY, class->class_name_upper, class->class_name_lower[0]);
        fprintf(file, ") {\n");
        if (class->n_parent_class > 0) {
            fprintf(file, CONSTR_SUPER_OPEN);
            fprintf(file, "%c);\n", class->class_name_lower[0]);
        }
        for (int i = 0; i < class->n_attribute; i++) {
            class_attribute_t *att = class->class_attribute_arr[i];
            fprintf(file, CONSTR_CONTENT_COPY, att->att_name_lower, class->class_name_lower[0], att->att_name_upper);
        }
        fprintf(file, "\t}\n\n");
    }

    // getters and setters
    if (class->n_attribute > 0) {
        for (int i = 0; i < class->n_attribute; i++) {
            class_attribute_t *att = class->class_attribute_arr[i];
            fprintf(file, GETTER, att->att_type, att->att_name_upper, att->att_name_lower);
            fprintf(file, "\n");
            fprintf(file, SETTER, att->att_name_upper, att->att_type, att->att_name_lower, att->att_name_lower,
                    att->att_name_lower);
            if (i != class->n_attribute - 1)
                fprintf(file, "\n");
        }
    }

    // // toString
    // fprintf(file,"\n\t@Override\n");
    // fprintf(file, TO_STRING_OPEN);
    // if (isInherited == 0) {
    //     fprintf(file, TO_STRING);
    // } else {
    //     fprintf(file, TO_STRING_WI);
    // }
    // if (attNumber > 0) {
    //     fprintf(file, " + ");
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         if (is2DArray(classAttList[i].attType) == 1) {
    //             fprintf(file, TO_STRING_CONTENT_DEEP_ARR, classAttList[i].attName, classAttList[i].attName);
    //         } else if (isArray(classAttList[i].attType) == 1) {
    //             fprintf(file, TO_STRING_CONTENT_ARR, classAttList[i].attName, classAttList[i].attName);
    //         } else {
    //             fprintf(file, TO_STRING_CONTENT, classAttList[i].attName, classAttList[i].attName);
    //         }
    //         if (i != attNumber - 1) {
    //             fprintf(file, " + ");
    //         }
    //     }
    // }
    // fprintf(file, ";\n\t}\n");

    // // equals
    // if (attNumber > 0 || isInherited == 1) {
    //     fprintf(file,"\n\t@Override\n");
    //     fprintf(file, EQUALS, className, firstLetterLower);
    //     if (isInherited == 1) {
    //         fprintf(file, EQUALS_SUPER, firstLetterLower);
    //         if (attNumber > 0) {
    //             fprintf(file, " && ");
    //         }
    //     }
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         char *attNameUp = strdup(classAttList[i].attName);
    //         attNameUp[0] = toupper((unsigned char)attNameUp[0]);
    //         int isFirstLetterLower = islower((unsigned char)classAttList[i].attType[0]) != 0;
    //         if (is2DArray(classAttList[i].attType) == 1) {
    //             fprintf(file, EQUALS_CONTENT_DEEP_ARR, classAttList[i].attName, firstLetterLower, attNameUp);
    //         } else if (isArray(classAttList[i].attType) == 1) {
    //             fprintf(file, EQUALS_CONTENT_ARR, classAttList[i].attName, firstLetterLower, attNameUp);
    //         } else if (isFirstLetterLower == 1) {
    //             fprintf(file, EQUALS_CONTENT, classAttList[i].attName, firstLetterLower, attNameUp);
    //         } else {
    //             fprintf(file, EQUALS_CONTENT_OBJ, classAttList[i].attName, firstLetterLower, attNameUp);
    //         }
    //         if (i != attNumber - 1) {
    //             fprintf(file," && ");
    //         }
    //     }
    //     fprintf(file,";\n\t}\n");
    // }

    // // hashCode
    // if (attNumber > 0 || isInherited == 1) {
    //     fprintf(file,"\n\t@Override\n");
    //     fprintf(file,HASH_CODE);

    //     if (hasNonPrim == 1) {
    //         fprintf(file, HASH_CODE_CONTENT_NONPRIM);
    //     } else {
    //         fprintf(file, HASH_CODE_CONTENT);
    //     }
    //     if (isInherited == 1) {
    //         fprintf(file, HASH_CODE_SUPER);
    //         if (nPrim > 0) {
    //             fprintf(file, ", ");
    //         }
    //     }

    //     int nPrimCounter = 0;
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         if (isArray(classAttList[i].attType) == 0 && is2DArray(classAttList[i].attType) == 0) {
    //             fprintf(file, HASH_CODE_PARAM, classAttList[i].attName);
    //             if (++nPrimCounter != nPrim) {
    //                 fprintf(file, ", ");
    //             }
    //         }
    //     }
    //     fprintf(file, ");\n");
    //     if (hasArr) {
    //         fprintf(file, HASH_CODE_NONPRIM, "hashCode(");
    //         int arrCounter = 0;
    //         for (unsigned int i = 0; i < attNumber; i++) {
    //             if (isArray(classAttList[i].attType) == 1) {
    //                 fprintf(file, HASH_CODE_PARAM, classAttList[i].attName);
    //                 if (++arrCounter != nArr) {
    //                     fprintf(file, ", ");
    //                 }
    //             }
    //         }
    //         fprintf(file, ");\n");
    //     }
    //     if (has2DArr) {
    //         fprintf(file, HASH_CODE_NONPRIM, "deepHashCode(");
    //         int arr2DCounter = 0;
    //         for (unsigned int i = 0; i < attNumber; i++) {
    //             if (is2DArray(classAttList[i].attType) == 1) {
    //                 fprintf(file, HASH_CODE_PARAM, classAttList[i].attName);
    //                 if (++arr2DCounter != nArr) {
    //                     fprintf(file, ", ");
    //                 }
    //             }
    //         }
    //         fprintf(file, ");\n");
    //     }
    //     if (hasNonPrim == 1) {
    //         fprintf(file, "\t\treturn hashResult;\n");
    //     }
    //     fprintf(file, "\t}\n");

    // }
    fprintf(file, "}");
}