#ifndef WRITE_TO_FILE
#define WRITE_TO_FILE

#include <stdio.h>

void writeToFile(FILE *f, const char *className, const char *attVis, unsigned int attNumber, char **attNameArr,
                 char **attTypeArr);

#endif