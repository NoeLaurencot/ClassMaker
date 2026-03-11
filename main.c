#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "writeToFile.h"

unsigned int getAttNumber() {
    unsigned int n;
    printf("How many attributes?\n");
    scanf("%u", &n);
    return n;
}

char *getAttVis() {
    int vis;
    printf("What is the visibility of the attributes? (default: friendly)\n");
    printf("1. public\n");
    printf("2. private\n");
    printf("3. protected\n");
    scanf("%d", &vis);
    switch (vis) {
    case 1:
        return "public";
    case 2:
        return "private";
    case 3:
        return "protected";
    default:
        return "";
    }
}

char *getCustomType() {
    char *type = malloc(sizeof(char) * 256);
    printf("Custom data type:\n");
    scanf("%s", type);
    return type;
}

char *getAttType(char *attName) {
    int type;
    printf("Data type of attribute \"%s\"? (default: Object)\n", attName);
    printf("1. int\n");
    printf("2. double\n");
    printf("3. String\n");
    printf("4. boolean\n");
    printf("5. Custom\n");
    scanf("%d", &type);
    switch (type) {
    case 1:
        return strdup("int");
    case 2:
        return strdup("double");
    case 3:
        return strdup("String");
    case 4:
        return strdup("boolean");
    case 5:
        return getCustomType();
    default:
        return strdup("Object");
    }
}

char *getAttName(int i) {
    char *name = malloc(sizeof(char) * 256);
    printf("Name of the attribute n°%d:\n", i + 1);
    scanf("%s", name);
    name[0] = tolower((unsigned char)name[0]);
    return name;
}

void getClassName(char *className) {
    printf("What is the name of the class?\n");
    scanf("%s", className);
    className[0] = toupper((unsigned char)className[0]);
}

void getParentClassName(char *className) {
    printf("What is the name of the parent class?\n");
    scanf("%s", className);
    className[0] = toupper((unsigned char)className[0]);
}

int isClassInherited() {
    char boolean;
    printf("Is this class inherited (y/n)?\n");
    scanf(" %c", &boolean);
    switch (boolean) {
    case 'y':
        return 1;
    case 'n':
    default:
        return 0;
    }
}

FILE *createFile(char *className) {
    getClassName(className);
    char fullPath[2048] = "./";
    strcat(fullPath, className);
    strcat(fullPath, ".java");
    return fopen(fullPath, "wx");
}

void CLI(FILE *f, char *className) {
    char parentClassName[512];
    int isInherited = isClassInherited();
    if (isInherited == 1) {
        getParentClassName(parentClassName);
    }
    char *attVis;
    ;
    unsigned int attNumber = getAttNumber();

    if (attNumber > 0) {
        attVis = getAttVis();
    }

    char **attTypeArr = malloc(sizeof(char *) * attNumber);
    char **attNameArr = malloc(sizeof(char *) * attNumber);
    for (int i = 0; i < attNumber; i++) {
        attNameArr[i] = getAttName(i);
        attTypeArr[i] = getAttType(attNameArr[i]);
    }

    writeToFile(f, className, attVis, attNumber, attNameArr, attTypeArr, isInherited, parentClassName);

    for (int i = 0; i < attNumber; i++) {
        free(attNameArr[i]);
        free(attTypeArr[i]);
    }
    free(attNameArr);
    free(attTypeArr);
}

int main(void) {
    char className[512];
    FILE *f = createFile(className);
    if (f == NULL) {
        printf("Error: the file already exists or cannot be created.\n");
        return 1;
    }
    CLI(f, className);
    fclose(f);
    return 0;
}