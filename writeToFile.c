#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "writeToFile.h"
#include "template.h"

void writeToFile(FILE *f, const char *className, char *packageName, const char *attVis, unsigned int attNumber, char **attNameArr,
                 char **attTypeArr, int isInherited, char *parentClassName, classAttribute *parentAttList, int parentAttCount) {
    // class init
    if (packageName) {
        fprintf(f, PACKAGE, packageName);
    }

    if (isInherited == 1) {
        fprintf(f, CLASS_WI, className, parentClassName);
    } else {
        fprintf(f, CLASS, className);
    }

    // attributes
    for (int i = 0; i < attNumber; i++) {
        if (strcmp(attVis, "") == 0) {
            fprintf(f, ATTRIBUTE, attTypeArr[i], attNameArr[i]);
        } else {
            fprintf(f, ATTRIBUTE_WVIS, attVis, attTypeArr[i], attNameArr[i]);
        }
    }

    // empty constructor
    fprintf(f, "\n");
    fprintf(f, EMPTY_CONSTR, className);

    // full constructor
    if (attNumber > 0) {
        fprintf(f, "\n");
        fprintf(f, CONSTR_OPEN, className);

        if (isInherited == 1) {
            for (int i = 0; i < parentAttCount; i++) {
                fprintf(f, CONSTR_PARAMETER, parentAttList[i].attType, parentAttList[i].attName);
                fprintf(f, ", ");
            }
        }

        for (int i = 0; i < attNumber; i++) {
            fprintf(f, CONSTR_PARAMETER, attTypeArr[i], attNameArr[i]);
            if (i != attNumber - 1) {
                fprintf(f, ", ");
            } else {
                fprintf(f, ") {\n");
            }
        }

        if (isInherited == 1) {
            fprintf(f,CONSTR_SUPER_OPEN);
            for (int i = 0; i < parentAttCount; i++) {
                fprintf(f, CONSTR_SUPER_PARAMETER, parentAttList[i].attName);
                if (i != parentAttCount - 1) {
                    fprintf(f,", ");
                }
            }
            fprintf(f,");\n");
        }

        for (int i = 0; i < attNumber; i++) {
            fprintf(f, CONSTR_CONTENT, attNameArr[i], attNameArr[i]);
        }

        fprintf(f, "    }\n\n");
    }

    // getters and setters
    if (attNumber > 0 && strcmp(attVis, "public") != 0) {
        for (int i = 0; i < attNumber; i++) {
            char *attNameUp = strdup(attNameArr[i]);
            attNameUp[0] = toupper((unsigned char)attNameUp[0]);
            fprintf(f, GETTER, attTypeArr[i], attNameUp, attNameArr[i]);
            fprintf(f, "\n");
            fprintf(f, SETTER, attNameUp, attTypeArr[i], attNameArr[i], attNameArr[i], attNameArr[i]);
            if (i != attNumber - 1)
                fprintf(f, "\n");
        }
    }

    // toString
    fprintf(f,"\n");
    fprintf(f, TO_STRING_OPEN);
    if (isInherited == 0) {
        fprintf(f, TO_STRING);
    } else {
        fprintf(f, TO_STRING_WI);
    }
    if (attNumber > 0) {
        fprintf(f, " + ");
        for (int i = 0; i < attNumber; i++) {
            fprintf(f, TO_STRING_CONTENT, attNameArr[i], attNameArr[i]);
            if (i != attNumber - 1) {
                fprintf(f, " + ");
            }
        }
    }
    fprintf(f, ";\n    }\n");



    fprintf(f, "}");
}
