#include "data_desk.h"

static FILE *global_generation_file = 0;

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    global_generation_file = fopen("generated.c", "w");
}

void
GeneratePrintCode(FILE *file, DataDeskASTNode *first_member, char *access_string)
{
    for(DataDeskASTNode *member = first_member; member; member = member->next)
    {
        if(!DataDeskNodeHasTag(member, "NoPrint"))
        {
            if(DataDeskStructMemberIsType(member, "int"))
            {
                fprintf(file, "    printf(\"%%i, \", %s%s);\n", access_string, member->string);
            }
            else if(DataDeskStructMemberIsType(member, "char"))
            {
                fprintf(file, "    printf(\"%%c, \", %s%s);\n", access_string, member->string);
            }
            else if(DataDeskStructMemberIsType(member, "float"))
            {
                fprintf(file, "    printf(\"%%f, \", %s%s);\n", access_string, member->string);
            }
            else if(DataDeskStructMemberIsType(member, "*char"))
            {
                fprintf(file, "    printf(\"%%s, \", %s%s);\n", access_string, member->string);
            }
            else if(member->declaration.type->type_usage.type_definition &&
                    member->declaration.type->type_usage.pointer_count <= 1)
            {
                DataDeskASTNode *sub_type = member->declaration.type->type_usage.type_definition;
                DataDeskASTNode *sub_type_first_member = 0;
                
                if(sub_type->type == DATA_DESK_AST_NODE_TYPE_struct_declaration)
                {
                    sub_type_first_member = sub_type->struct_declaration.first_member;
                }
                else if(sub_type->type == DATA_DESK_AST_NODE_TYPE_union_declaration)
                {
                    sub_type_first_member = sub_type->union_declaration.first_member;
                }
                
                if(sub_type_first_member)
                {
                    char *access_operator = member->declaration.type->type_usage.pointer_count == 1 ? "->" : ".";
                    char new_access_string[256] = {0};
                    snprintf(new_access_string, sizeof(new_access_string), "%s%s%s", access_string, member->string, access_operator);
                    GeneratePrintCode(file, sub_type_first_member, new_access_string);
                }
            }
        }
    }
}

DATA_DESK_FUNC void
DataDeskCustomStructCallback(DataDeskStruct struct_info, char *filename)
{
    FILE *file = global_generation_file;
    DataDeskFWriteStructAsC(file, struct_info);
    
    if(DataDeskStructHasTag(struct_info, "Printable"))
    {
        DataDeskASTNode *root = struct_info.root;
        fprintf(file, "void\n");
        fprintf(file, "%sPrint(%s *object)\n", struct_info.name, struct_info.name);
        fprintf(file, "{\n");
        fprintf(file, "    printf(\"{ \");\n");
        GeneratePrintCode(file, struct_info.root->struct_declaration.first_member, "object->");
        fprintf(file, "    printf(\"}\");\n");
        fprintf(file, "}\n\n");
    }
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
    fclose(global_generation_file);
}
