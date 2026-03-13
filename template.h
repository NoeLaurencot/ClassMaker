#ifndef TEMPLATE
#define TEMPLATE

#define PACKAGE \
"package %s;\n\n"

#define IMPORT \
"import %s;\n"

#define CLASS \
"public class %s {\n"

#define CLASS_WI \
"public class %s extends %s {\n"

#define ATTRIBUTE_WVIS \
"\t%s %s %s;\n"

#define ATTRIBUTE \
"\t%s %s;\n"

#define EMPTY_CONSTR \
"\tpublic %s() {}\n"

#define CONSTR_OPEN \
"\tpublic %s("

#define CONSTR_PARAMETER \
"%s %s"

#define CONSTR_PARAMETER_COPY \
"%s %c"

#define CONSTR_SUPER_OPEN \
"\t\tsuper("

#define CONSTR_SUPER_PARAMETER \
"%s"

#define CONSTR_SUPER_COPY \
"%c);\n\t\tif (%c == null) {\n\t\t\tthrow new IllegalArgumentException(\"%s cannot be null\");\n\t\t}\n"

#define CONSTR_CONTENT \
"\t\tthis.%s = %s;\n"

#define CONSTR_CONTENT_COPY \
"\t\tthis.%s = %c.get%s();\n"

#define GETTER \
"\tpublic %s get%s() {\n\t\treturn this.%s;\n\t}\n"

#define SETTER \
"\tpublic void set%s(%s %s) {\n\t\tthis.%s = %s;\n\t}\n"

#define TO_STRING_OPEN \
"\tpublic String toString() {\n"

#define TO_STRING \
"\t\treturn this.getClass().getSimpleName()"

#define TO_STRING_WI \
"\t\treturn super.toString()"

#define TO_STRING_CONTENT \
"\" %s = \" + this.%s"

#define TO_STRING_CONTENT_ARR \
"\" %s = \" + Arrays.toString(this.%s)"

#define TO_STRING_CONTENT_DEEP_ARR \
"\" %s = \" + Arrays.deepToString(this.%s)"

#define EQUALS \
"\tpublic boolean equals(Object o) {\n\t\tif (o == null) {\n\t\t\treturn false;\n\t\t}\n\t\tif (!(o instanceof %s %c)) {\n\t\t\treturn false;\n\t\t}\n\t\treturn "

#define EQUALS_SUPER \
"super.equals(%c)"

#define EQUALS_CONTENT \
"this.%s == %c.get%s()"

#define EQUALS_CONTENT_OBJ \
"this.%s.equals(%c.get%s())"

#define EQUALS_CONTENT_ARR \
"Arrays.equals(this.%s, %c.get%s())"

#define EQUALS_CONTENT_DEEP_ARR \
"Arrays.deepEquals(this.%s, %c.get%s())"

#define HASH_CODE \
"\tpublic int hashCode() {\n"

#define HASH_CODE_CONTENT \
"\t\treturn Objects.hash("

#define HASH_CODE_CONTENT_NONPRIM \
"\t\tint hashResult = Objects.hash("

#define HASH_CODE_SUPER \
"super.hashCode()"

#define HASH_CODE_PARAM \
"this.%s"

#define HASH_CODE_NONPRIM \
"\t\thashResult = 31 * hashResult + Arrays.%s"

#endif