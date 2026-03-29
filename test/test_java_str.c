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

void test_att_kind() {

    class_attribute_t *att1 = calloc(1, sizeof(class_attribute_t));
    att1->att_type = "int";

    class_attribute_t *att2 = calloc(1, sizeof(class_attribute_t));
    att2->att_type = "int[]";

    class_attribute_t *att3 = calloc(1, sizeof(class_attribute_t));
    att3->att_type = "int[][]";

    class_attribute_t *att4 = calloc(1, sizeof(class_attribute_t));
    att4->att_type = "Object";

    class_attribute_t *att5 = calloc(1, sizeof(class_attribute_t));
    att5->att_type = "Object[]";

    class_attribute_t *att6 = calloc(1, sizeof(class_attribute_t));
    att6->att_type = "ArrayList<int[]>";

    class_attribute_t *att7 = calloc(1, sizeof(class_attribute_t));
    att7->att_type = "ArrayList<int[]>[]";

    int n_att = 7;
    class_attribute_t *att_arr[] = {att1, att2, att3, att4, att5, att6, att7};

    for (int i = 0; i < n_att; i++) {
        printf("%s -> is primitive : %d\n", att_arr[i]->att_type, is_primitive(att_arr[i]));
        printf("%s -> is object : %d\n", att_arr[i]->att_type, is_simple_object(att_arr[i]));
        printf("%s -> is array : %d\n", att_arr[i]->att_type, is_simple_array(att_arr[i]));
        printf("%s -> is deep array : %d\n", att_arr[i]->att_type, is_deep_array(att_arr[i]));
        printf("\n");
    }
}

int main(void) {
    test_att_kind();
}