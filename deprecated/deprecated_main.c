// deprecated
#include "attribute.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *createFile(char *className);
void CLI(FILE *f, char *className);
int getNDirToCheck(char *path);

unsigned int getAttNumber(char *className) {
    unsigned int n;
    char question[256];
    snprintf(question, sizeof(question), "How many attributes for %s?\n", className);
    printf("%s", question);
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
    if (!type) {
        fprintf(stderr, "Memory allocation failed for customType\n");
        exit(1);
    }
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
    if (!name) {
        fprintf(stderr, "Memory allocation failed for attributeName\n");
        exit(1);
    }
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

int askBoolean(const char *text) {
    char boolean;
    printf("%s (y/n)\n", text);
    scanf(" %c", &boolean);
    switch (boolean) {
    case 'y':
        return 1;
    case 'n':
        return 0;
    default:
        return 0;
    }
}

char *getPathFromSrc() {
    int size = 1024;
    char cwd[size];
    if (getcwd(cwd, size) == NULL) {
        return NULL;
    }
    char *p = strstr(cwd, "src/");
    if (!p) {
        return NULL;
    }
    return strdup(p);
}

int getExtendsLine(char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("Could not open file\n");
        exit(1);
    }

    char line[256];
    int lineNumber = 1;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "class") && strstr(line, "{")) {
            fclose(file);
            return lineNumber;
        }
        lineNumber++;
    }

    fclose(file);
    return -1;
}

char *parseParentExtends(char *filePath) {
    int extendsLine = getExtendsLine(filePath);
    if (extendsLine == -1) {
        return NULL;
    }

    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("Could not open file\n");
        exit(1);
    }

    char line[256];
    int currentLine = 1;
    while (fgets(line, sizeof(line), file)) {
        if (currentLine == extendsLine) {
            break;
        }
        currentLine++;
    }

    if (currentLine != extendsLine) {
        fclose(file);
        return NULL;
    }

    char *parentClass = malloc(256);
    if (!parentClass) {
        fprintf(stderr, "Memory allocation failed for parentClass\n");
        fclose(file);
        exit(1);
    }

    char *extendsPos = strstr(line, "extends");
    if (!extendsPos) {
        fclose(file);
        free(parentClass);
        return NULL;
    }
    char *afterExtends = extendsPos + 7;

    sscanf(afterExtends, "%s", parentClass);
    fclose(file);
    return parentClass;
}

char *stripAttLine(char *line) {
    char *modifiers[] = {"private ", "protected ", "public ", "final "};
    int nModifiers = 4;
    int found = 1;
    while (found) {
        found = 0;
        for (int i = 0; i < nModifiers; i++) {
            int len = strlen(modifiers[i]);
            if (strncmp(line, modifiers[i], len) == 0) {
                memmove(line, line + len, strlen(line + len) + 1);
                found = 1;
            }
        }
    }
    return line;
}

char *parseParentAttributes(char *filePath, int startLine) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("Could not open file\n");
        exit(1);
    }

    char *parentAtt = malloc(256);
    if (!parentAtt) {
        fprintf(stderr, "Memory allocation failed for parentAtt\n");
        fclose(file);
        exit(1);
    }
    int currentLine = 1;
    while (fgets(parentAtt, 256, file)) {
        if (currentLine == startLine) {
            break;
        }
        currentLine++;
    }

    if (currentLine != startLine) {
        fclose(file);
        free(parentAtt);
        return NULL;
    }

    int len = strlen(parentAtt);
    while (len > 0 && (parentAtt[len - 1] == '\n' || parentAtt[len - 1] == '\r' || parentAtt[len - 1] == ';')) {
        len--;
        parentAtt[len] = '\0';
    }

    while (*parentAtt == ' ' || *parentAtt == '\t') {
        memmove(parentAtt, parentAtt + 1, strlen(parentAtt));
    }

    // Skip static attributes
    if (strstr(parentAtt, "static ")) {
        fclose(file);
        free(parentAtt);
        return strdup("__SKIP__");
    }

    stripAttLine(parentAtt);

    // Not an attribute line
    if (parentAtt[0] == '\0' || parentAtt[0] == '}' || strstr(parentAtt, "(")) {
        fclose(file);
        free(parentAtt);
        return NULL;
    }

    fclose(file);
    return parentAtt;
}

classAttribute getParentAttribute(char *line) {
    struct classAttribute attribute;
    char *attType = malloc(256);
    if (!attType) {
        fprintf(stderr, "Memory allocation failed for attType\n");
        exit(1);
    }
    char *attName = malloc(256);
    if (!attName) {
        fprintf(stderr, "Memory allocation failed for attName\n");
        exit(1);
    }

    sscanf(line, "%s %s", attType, attName);

    attribute.attType = attType;
    attribute.attName = attName;
    return attribute;
}

char *findFileInDirs(char *path, int nDirToCheck, char *parent) {
    char *classFileName = malloc(strlen(parent) + 6);
    if (!classFileName) {
        fprintf(stderr, "Memory allocation failed for classFileName\n");
        exit(1);
    }
    strcpy(classFileName, parent);
    strcat(classFileName, ".java");
    if (nDirToCheck < 1) {
        free(classFileName);
        int isCreated = askBoolean("The parent class dosen't exist or wasn't found, do you want to create it?");
        if (isCreated == 1) {
            FILE *f = createFile(parent);
            if (f == NULL) {
                printf("Error: the file already exists or cannot be created.\n");
                return NULL;
            }
            char *p = getPathFromSrc();
            if (!p) {
                p = strdup("./");
            }
            CLI(f, parent);
            fclose(f);
            printf("Parent ok, going back to child\n");
            char *result = findFileInDirs("./", getNDirToCheck(p), parent);
            free(p);
            return result;
        } else {
            return NULL;
        }
    }
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Error while opening directory %s", path);
        exit(1);
    }
    struct dirent *file;

    file = readdir(dir);
    while (file) {
        if (strcmp(file->d_name, classFileName) == 0) {
            break;
        }
        file = readdir(dir);
    }
    closedir(dir);

    if (file) {
        char *filePath = malloc(strlen(path) + strlen(classFileName) + 1);
        if (!filePath) {
            fprintf(stderr, "Memory allocation failed for filePath");
            exit(1);
        }
        strcpy(filePath, path);
        strcat(filePath, classFileName);
        free(classFileName);
        return filePath;
    } else {
        free(classFileName);
        char backPath[] = "../";
        char *newPath = malloc(strlen(path) + strlen(backPath) + 1);
        if (!newPath) {
            fprintf(stderr, "Memory allocation failed for newPath");
            exit(1);
        }
        strcpy(newPath, backPath);
        strcat(newPath, path);
        char *result = findFileInDirs(newPath, nDirToCheck - 1, parent);
        free(newPath);
        return result;
    }
}

char **searchParentsFilePaths(char *path, char *parent, int nDirToCheck, int *nFiles) {
    char **filePaths = malloc(sizeof(char *) * 256);
    if (!filePaths) {
        fprintf(stderr, "Memory allocation failed for filePaths\n");
        exit(1);
    }
    *nFiles = 0;

    char *filePath = findFileInDirs(path, nDirToCheck, parent);

    if (!filePath) {
        free(filePaths);
        return NULL;
    }

    // Check if this parent extends another class
    char *grandParent = parseParentExtends(filePath);
    if (grandParent) {
        int nAncestorFiles = 0;
        char **ancestorPaths = searchParentsFilePaths(path, grandParent, nDirToCheck, &nAncestorFiles);
        free(grandParent);

        if (ancestorPaths) {
            for (int i = 0; i < nAncestorFiles; i++) {
                int dst = *nFiles;
                filePaths[dst] = ancestorPaths[i];
                *nFiles = dst + 1;
            }
            free(ancestorPaths);
        }
    }

    // Add current parent after ancestors
    filePaths[(*nFiles)++] = filePath;

    return filePaths;
}

int getNDirToCheck(char *path) {
    int counter = 0;
    for (int i = 0; path[i]; i++) {
        if (path[i] == '/')
            counter++;
    }
    return counter;
}

classAttribute *getParentsAttributes(char *parent, int *nParentAtt) {
    char *p = getPathFromSrc();
    if (!p) {
        p = strdup("./");
    }
    struct classAttribute *attributeList = malloc(sizeof(classAttribute) * 256);
    if (!attributeList) {
        fprintf(stderr, "Memory allocation failed for attributeList\n");
        exit(1);
    }

    int nFiles = 0;
    char *startPath = strdup("./");
    char **filePaths = searchParentsFilePaths(startPath, parent, getNDirToCheck(p), &nFiles);
    free(startPath);
    free(p);

    if (!filePaths) {
        free(attributeList);
        return NULL;
    }

    int totalAtt = 0;
    for (int f = 0; f < nFiles; f++) {
        int startLine = getExtendsLine(filePaths[f]) + 1;
        int j = 0;
        char *line;
        while ((line = parseParentAttributes(filePaths[f], startLine + j)) != NULL) {
            if (strcmp(line, "__SKIP__") == 0) {
                free(line);
                j++;
                continue;
            }
            attributeList[totalAtt] = getParentAttribute(line);
            free(line);
            totalAtt++;
            j++;
        }
        free(filePaths[f]);
    }
    free(filePaths);

    *nParentAtt = totalAtt;
    return attributeList;
}

char *getPackageName() {
    char *p = getPathFromSrc();
    if (p) {
        char *pkg = p + 4;
        if (*pkg == '\0') {
            free(p);
            return NULL;
        }

        char *result = malloc(sizeof(char) * 256);
        if (!result) {
            fprintf(stderr, "Memory allocation failed for packageNameResult\n");
            free(p);
            exit(1);
        }
        strcpy(result, pkg);

        free(p);
        for (int i = 0; result[i]; i++) {
            if (result[i] == '/')
                result[i] = '.';
        }
        return result;
    } else {
        return NULL;
    }
}

FILE *createFile(char *className) {
    char fullPath[2048] = "./";
    strcat(fullPath, className);
    strcat(fullPath, ".java");
    return fopen(fullPath, "wx");
}

void CLI(FILE *f, char *className) {
    char *packageName = getPackageName();
    char parentClassName[512] = "";
    char question[512];
    snprintf(question, sizeof(question), "Is %s class inherited?", className);
    int isInherited = askBoolean(question);

    classAttribute *parentAttList = NULL;
    int parentAttCount = 0;
    if (isInherited == 1) {
        getParentClassName(parentClassName);
        parentAttList = getParentsAttributes(parentClassName, &parentAttCount);
    }
    char *attVis = "";
    unsigned int attNumber = getAttNumber(className);

    if (attNumber > 0) {
        attVis = getAttVis();
    }

    classAttribute *classAttList = malloc(sizeof(classAttribute) * attNumber);
    if (!classAttList) {
        fprintf(stderr, "Memory allocation failed for attributeList\n");
        exit(1);
    }

    for (unsigned int i = 0; i < attNumber; i++) {
        classAttList[i].attName = getAttName(i);
        classAttList[i].attType = getAttType(classAttList[i].attName);
    }

    writeToFile(f, className, packageName, attVis, attNumber, classAttList, isInherited, parentClassName, parentAttList,
                parentAttCount);

    for (unsigned int i = 0; i < attNumber; i++) {
        free(classAttList[i].attName);
        free(classAttList[i].attType);
    }
    free(classAttList);
    free(packageName);
}

int main(void) {
    char className[512];
    getClassName(className);
    FILE *f = createFile(className);
    if (f == NULL) {
        printf("Error: the file already exists or cannot be created.\n");
        return 1;
    }
    CLI(f, className);
    fclose(f);
    return 0;
}