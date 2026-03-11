#ifndef WRITE_TO_FILE
#define WRITE_TO_FILE

#include <stdio.h>
#include "attribute.h"

void writeToFile(FILE *f, const char *className, char *packageName, const char *attVis, unsigned int attNumber, char **attNameArr,
                 char **attTypeArr, int isInherited, char *parentClassName, classAttribute *parentAttList, int parentAttCount);

#endif