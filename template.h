#ifndef TEMPLATE
#define TEMPLATE

#define CLASS \
"public class %s {\n"

#define CLASS_WI \
"public class %s extends %s {\n"

#define ATTRIBUTE_WVIS \
"    %s %s %s;\n"

#define ATTRIBUTE \
"    %s %s;\n"

#define EMPTY_CONSTR \
"    public %s() {}\n"

#define CONSTR_OPEN \
"    public %s("

#define CONSTR_PARAMETER \
"%s %s"

#define CONSTR_SUPER_OPEN \
"       super("

#define CONSTR_SUPER_PARAMETER \
"%s"

#define CONSTR_CONTENT \
"        this.%s = %s;\n"

#define GETTER \
"    public %s get%s() {\n        return this.%s;\n    }\n"

#define SETTER \
"    public void set%s(%s %s) {\n        this.%s = %s;\n    }\n"

#define TO_STRING_OPEN \
"    public String toString() {\n"

#define TO_STRING \
"        return this.getClass().getSimpleName()"

#define TO_STRING_WI \
"        return super.toString()"

#define TO_STRING_CONTENT \
"\" %s = \" + this.%s"

#endif