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

    // // attributes
    for (unsigned int i = 0; i < class->n_attribute; i++) {
        class_attribute_t *att = class->class_attribute_arr[i];
        if (att->att_vis) 
            fprintf(file, ATTRIBUTE_WVIS, att->att_vis, att->att_type, att->att_name);
        else
            fprintf(file, ATTRIBUTE, att->att_type, att->att_name);
    }

    // empty constructor
    fprintf(file, "\n");
    fprintf(file, EMPTY_CONSTR, class->class_name_upper);

    // // full constructor
    // if (attNumber > 0 || isInherited == 1) {
    //     fprintf(f, "\n");
    //     fprintf(f, CONSTR_OPEN, className);

    //     if (isInherited == 1) {
    //         for (int i = 0; i < parentAttCount; i++) {
    //             fprintf(f, CONSTR_PARAMETER, parentAttList[i].attType, parentAttList[i].attName);
    //             if (i != parentAttCount - 1 || attNumber > 0) {
    //                 fprintf(f, ", ");
    //             }
    //         }
    //     }

    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         fprintf(f, CONSTR_PARAMETER, classAttList[i].attType, classAttList[i].attName);
    //         if (i != attNumber - 1) {
    //             fprintf(f, ", ");
    //         }
    //     }
    //     fprintf(f, ") {\n");

    //     if (isInherited == 1) {
    //         fprintf(f,CONSTR_SUPER_OPEN);
    //         for (int i = 0; i < parentAttCount; i++) {
    //             fprintf(f, CONSTR_SUPER_PARAMETER, parentAttList[i].attName);
    //             if (i != parentAttCount - 1) {
    //                 fprintf(f,", ");
    //             }
    //         }
    //         fprintf(f,");\n");
    //     }

    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         fprintf(f, CONSTR_CONTENT, classAttList[i].attName, classAttList[i].attName);
    //     }

    //     fprintf(f, "\t}\n");
    // }

    // // copy constructor
    // if (attNumber > 0 || isInherited == 1) {
    //     fprintf(f, "\n");
    //     fprintf(f, CONSTR_OPEN, className);
    //     fprintf(f, CONSTR_PARAMETER_COPY, className, firstLetterLower);
    //     fprintf(f, ") {\n");
    //     if (isInherited) {
    //         fprintf(f, CONSTR_SUPER_OPEN);
    //         fprintf(f, "%c);\n", firstLetterLower);
    //     }
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         char *attNameUp = strdup(classAttList[i].attName);
    //         attNameUp[0] = toupper((unsigned char)attNameUp[0]);
    //         fprintf(f,CONSTR_CONTENT_COPY, classAttList[i].attName, firstLetterLower, attNameUp);
    //     }
    //     fprintf(f,"\t}\n\n");
    // }

    // // getters and setters
    // if (attNumber > 0) {
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         char *attNameUp = strdup(classAttList[i].attName);
    //         attNameUp[0] = toupper((unsigned char)attNameUp[0]);
    //         fprintf(f, GETTER, classAttList[i].attType, attNameUp, classAttList[i].attName);
    //         fprintf(f, "\n");
    //         fprintf(f, SETTER, attNameUp, classAttList[i].attType, classAttList[i].attName, classAttList[i].attName,
    //         classAttList[i].attName); if (i != attNumber - 1)
    //             fprintf(f, "\n");
    //     }
    // }

    // // toString
    // fprintf(f,"\n\t@Override\n");
    // fprintf(f, TO_STRING_OPEN);
    // if (isInherited == 0) {
    //     fprintf(f, TO_STRING);
    // } else {
    //     fprintf(f, TO_STRING_WI);
    // }
    // if (attNumber > 0) {
    //     fprintf(f, " + ");
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         if (is2DArray(classAttList[i].attType) == 1) {
    //             fprintf(f, TO_STRING_CONTENT_DEEP_ARR, classAttList[i].attName, classAttList[i].attName);
    //         } else if (isArray(classAttList[i].attType) == 1) {
    //             fprintf(f, TO_STRING_CONTENT_ARR, classAttList[i].attName, classAttList[i].attName);
    //         } else {
    //             fprintf(f, TO_STRING_CONTENT, classAttList[i].attName, classAttList[i].attName);
    //         }
    //         if (i != attNumber - 1) {
    //             fprintf(f, " + ");
    //         }
    //     }
    // }
    // fprintf(f, ";\n\t}\n");

    // // equals
    // if (attNumber > 0 || isInherited == 1) {
    //     fprintf(f,"\n\t@Override\n");
    //     fprintf(f, EQUALS, className, firstLetterLower);
    //     if (isInherited == 1) {
    //         fprintf(f, EQUALS_SUPER, firstLetterLower);
    //         if (attNumber > 0) {
    //             fprintf(f, " && ");
    //         }
    //     }
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         char *attNameUp = strdup(classAttList[i].attName);
    //         attNameUp[0] = toupper((unsigned char)attNameUp[0]);
    //         int isFirstLetterLower = islower((unsigned char)classAttList[i].attType[0]) != 0;
    //         if (is2DArray(classAttList[i].attType) == 1) {
    //             fprintf(f, EQUALS_CONTENT_DEEP_ARR, classAttList[i].attName, firstLetterLower, attNameUp);
    //         } else if (isArray(classAttList[i].attType) == 1) {
    //             fprintf(f, EQUALS_CONTENT_ARR, classAttList[i].attName, firstLetterLower, attNameUp);
    //         } else if (isFirstLetterLower == 1) {
    //             fprintf(f, EQUALS_CONTENT, classAttList[i].attName, firstLetterLower, attNameUp);
    //         } else {
    //             fprintf(f, EQUALS_CONTENT_OBJ, classAttList[i].attName, firstLetterLower, attNameUp);
    //         }
    //         if (i != attNumber - 1) {
    //             fprintf(f," && ");
    //         }
    //     }
    //     fprintf(f,";\n\t}\n");
    // }

    // // hashCode
    // if (attNumber > 0 || isInherited == 1) {
    //     fprintf(f,"\n\t@Override\n");
    //     fprintf(f,HASH_CODE);

    //     if (hasNonPrim == 1) {
    //         fprintf(f, HASH_CODE_CONTENT_NONPRIM);
    //     } else {
    //         fprintf(f, HASH_CODE_CONTENT);
    //     }
    //     if (isInherited == 1) {
    //         fprintf(f, HASH_CODE_SUPER);
    //         if (nPrim > 0) {
    //             fprintf(f, ", ");
    //         }
    //     }

    //     int nPrimCounter = 0;
    //     for (unsigned int i = 0; i < attNumber; i++) {
    //         if (isArray(classAttList[i].attType) == 0 && is2DArray(classAttList[i].attType) == 0) {
    //             fprintf(f, HASH_CODE_PARAM, classAttList[i].attName);
    //             if (++nPrimCounter != nPrim) {
    //                 fprintf(f, ", ");
    //             }
    //         }
    //     }
    //     fprintf(f, ");\n");
    //     if (hasArr) {
    //         fprintf(f, HASH_CODE_NONPRIM, "hashCode(");
    //         int arrCounter = 0;
    //         for (unsigned int i = 0; i < attNumber; i++) {
    //             if (isArray(classAttList[i].attType) == 1) {
    //                 fprintf(f, HASH_CODE_PARAM, classAttList[i].attName);
    //                 if (++arrCounter != nArr) {
    //                     fprintf(f, ", ");
    //                 }
    //             }
    //         }
    //         fprintf(f, ");\n");
    //     }
    //     if (has2DArr) {
    //         fprintf(f, HASH_CODE_NONPRIM, "deepHashCode(");
    //         int arr2DCounter = 0;
    //         for (unsigned int i = 0; i < attNumber; i++) {
    //             if (is2DArray(classAttList[i].attType) == 1) {
    //                 fprintf(f, HASH_CODE_PARAM, classAttList[i].attName);
    //                 if (++arr2DCounter != nArr) {
    //                     fprintf(f, ", ");
    //                 }
    //             }
    //         }
    //         fprintf(f, ");\n");
    //     }
    //     if (hasNonPrim == 1) {
    //         fprintf(f, "\t\treturn hashResult;\n");
    //     }
    //     fprintf(f, "\t}\n");

    // }
    fprintf(file, "}");
}