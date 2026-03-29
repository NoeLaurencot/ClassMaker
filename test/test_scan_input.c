#include "class_creator.h"
#include "class_parser.h"
#include "class_writer.h"
#include "enums.h"
#include "java_str.h"
#include "mem_check.h"
#include "scan_input.h"
#include "string_util.h"
#include "structs.h"
#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void test_scan_boolean() {
    int n_test = 3;
    for (int i = 0; i < n_test; i ++) {
        printf("test input:\n");
        printf("%d\n", scan_boolean());
    }
}

int main(void) {
    test_scan_boolean();
}