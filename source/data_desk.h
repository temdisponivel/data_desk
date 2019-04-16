#ifndef DATA_DESK_H_INCLUDED_
#define DATA_DESK_H_INCLUDED_

#ifndef DATA_DESK_NO_CRT
#include <stdio.h>
#endif

#if defined(_MSC_VER)
#define DATA_DESK_EXPORT __declspec(dllexport)
#else
#define DATA_DESK_EXPORT
#endif

#if defined(__cplusplus)
#define DATA_DESK_EXTERN_C extern "C"
#else
#define DATA_DESK_EXTERN_C
#endif

#define DATA_DESK_FUNC DATA_DESK_EXPORT DATA_DESK_EXTERN_C





/*
| /////////////////////////////////////////////////////////////////
 |  Callback Structs/Functions
 | /////////////////////////////////////////////////////////////////
 |
 | These are structures and function typedefs that correspond to
 | the callbacks that a custom layer can receive. A custom layer
 | doesn't need to define every possible callback. The symbol
 | names that Data Desk expects for each callback are listed
 | next to the corresponding function typedef.
 */

typedef struct DataDeskASTNode DataDeskASTNode;

typedef struct DataDeskStruct
{
    char *name;
    char *name_lowercase_with_underscores;
    char *name_uppercase_with_underscores;
    char *name_lower_camel_case;
    char *name_upper_camel_case;
    DataDeskASTNode *root;
}
DataDeskStruct;

typedef struct DataDeskDeclaration
{
    char *name;
    char *name_lowercase_with_underscores;
    char *name_uppercase_with_underscores;
    char *name_lower_camel_case;
    char *name_upper_camel_case;
    DataDeskASTNode *root;
}
DataDeskDeclaration;

/* DataDeskCustomInitCallback */
typedef void DataDeskInitCallback(void);

/* DataDeskCustomFileCallback */
typedef void DataDeskFileCallback(char *filename);

/* DataDeskCustomStructCallback */
typedef void DataDeskStructCallback(DataDeskStruct parsed_struct, char *filename);

/* DataDeskCustomDeclarationCallback */
typedef void DataDeskDeclarationCallback(DataDeskDeclaration declaration, char *filename);

/* DataDeskCustomCleanUpCallback */
typedef void DataDeskCleanUpCallback(void);





/*
| /////////////////////////////////////////////////////////////////
 |  Abstract Syntax Tree Representation
 | /////////////////////////////////////////////////////////////////
 |
 | The following code outlines the general structure for the
 | abstract syntax trees that Data Desk generates. Each sub-struct
 | contained inside of the DataDeskASTNode struct is only safe to
 | access if the "int type" variable in the struct is set to the
 | corresponding type value. All of the type constants are defined
 | in the following enum.
 */

enum
{
    DATA_DESK_AST_NODE_TYPE_invalid,
    DATA_DESK_AST_NODE_TYPE_identifier,
    DATA_DESK_AST_NODE_TYPE_numeric_constant,
    DATA_DESK_AST_NODE_TYPE_string_constant,
    DATA_DESK_AST_NODE_TYPE_char_constant,
    DATA_DESK_AST_NODE_TYPE_binary_operator,
    DATA_DESK_AST_NODE_TYPE_struct_declaration,
    DATA_DESK_AST_NODE_TYPE_declaration,
    DATA_DESK_AST_NODE_TYPE_type_usage,
    DATA_DESK_AST_NODE_TYPE_tag,
};

// NOTE(rjf): These must be in order of precedence
enum
{
    DATA_DESK_BINARY_OPERATOR_TYPE_invalid,
    DATA_DESK_BINARY_OPERATOR_TYPE_add,
    DATA_DESK_BINARY_OPERATOR_TYPE_subtract,
    DATA_DESK_BINARY_OPERATOR_TYPE_multiply,
    DATA_DESK_BINARY_OPERATOR_TYPE_divide,
    DATA_DESK_BINARY_OPERATOR_TYPE_MAX
};

typedef struct DataDeskASTNode
{
    int type;
    DataDeskASTNode *next;
    
    char *string;
    int string_length;
    
    DataDeskASTNode *first_tag;
    
    union
    {
        struct BinaryOperator
        {
            int type;
            DataDeskASTNode *left;
            DataDeskASTNode *right;
        }
        binary_operator;
        
        struct StructDeclaration
        {
            DataDeskASTNode *first_member;
        }
        struct_declaration;
        
        struct Declaration
        {
            DataDeskASTNode *type;
            DataDeskASTNode *initialization;
        }
        declaration;
        
        struct TypeUsage
        {
            int pointer_count;
            DataDeskASTNode *first_array_size_expression;
            DataDeskASTNode *struct_declaration;
        }
        type_usage;
    };
}
DataDeskASTNode;





/*
| /////////////////////////////////////////////////////////////////
 |  Introspection Helper Functions
 | /////////////////////////////////////////////////////////////////
 |
 | The following function prototypes are convenience utility
 | functions that can be helpful when introspecting upon abstract
 | syntax trees.
 */

int DataDeskStringHasSubString(char *tag, char *substring);
int DataDeskNodeHasTag(DataDeskASTNode *root, char *tag);
int DataDeskStructHasTag(DataDeskStruct struct_info, char *tag);
int DataDeskDeclarationHasTag(DataDeskDeclaration declaration_info, char *tag);
int DataDeskDeclarationIsType(DataDeskASTNode *root, char *type);
int DataDeskStructMemberIsType(DataDeskASTNode *root, char *type);

#ifndef DATA_DESK_NO_CRT
void DataDeskFWriteStructAsC(FILE *file, DataDeskStruct struct_info);
#endif





/*
| /////////////////////////////////////////////////////////////////
 |  Helper Function Implementation Code
 | /////////////////////////////////////////////////////////////////
 */

int
DataDeskStringHasSubString(char *string, char *substring)
{
    int matches = 0;
    
    if(string)
    {
        for(int i = 0; string[i]; ++i)
        {
            if(string[i] == substring[0])
            {
                matches = 1;
                
                int string_i = i+1;
                int substring_i = 1;
                for(;;)
                {
                    if(!substring[substring_i])
                    {
                        break;
                    }
                    
                    if(string[string_i] != substring[substring_i])
                    {
                        matches = 0;
                        break;
                    }
                    
                    ++string_i;
                    ++substring_i;
                }
            }
            
            if(matches)
            {
                break;
            }
        }
    }
    
    return matches;
}

int
DataDeskNodeHasTag(DataDeskASTNode *node, char *tag)
{
    int has = 0;
    for(DataDeskASTNode *tag_node = node->first_tag;
        tag_node;
        tag_node = tag_node->next)
    {
        if(DataDeskStringHasSubString(tag_node->string, tag))
        {
            has = 1;
            break;
        }
    }
    return has;
}

int
DataDeskStructHasTag(DataDeskStruct struct_info, char *tag)
{
    return DataDeskNodeHasTag(struct_info.root, tag);
}

int
DataDeskDeclarationHasTag(DataDeskDeclaration declaration_info, char *tag)
{
    return DataDeskNodeHasTag(declaration_info.root, tag);
}

int
DataDeskDeclarationIsType(DataDeskASTNode *root, char *type)
{
    int pointer_count = 0;
    char *type_name = type;
    
    for(int i = 0; type[i]; ++i)
    {
        if(type[i] == '*')
        {
            ++pointer_count;
        }
        else
        {
            type_name = type+i;
            break;
        }
    }
    
    int matches = pointer_count == root->declaration.type->type_usage.pointer_count;
    if(matches)
    {
        for(int i = 0; type_name[i] && root->declaration.type->string[i]; ++i)
        {
            if(type_name[i] != root->declaration.type->string[i])
            {
                matches = 0;
                break;
            }
        }
    }
    
    return matches;
}

int
DataDeskStructMemberIsType(DataDeskASTNode *root, char *type)
{
    return DataDeskDeclarationIsType(root, type);
}

#ifndef DATA_DESK_NO_CRT
void
DataDeskFWriteASTFromRootAsC(FILE *file, DataDeskASTNode *root, int follow_next)
{
    if(root)
    {
        switch(root->type)
        {
            case DATA_DESK_AST_NODE_TYPE_identifier:
            case DATA_DESK_AST_NODE_TYPE_numeric_constant:
            case DATA_DESK_AST_NODE_TYPE_string_constant:
            case DATA_DESK_AST_NODE_TYPE_char_constant:
            {
                fprintf(file, "%s", root->string);
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_binary_operator:
            {
                fprintf(file, "(");
                DataDeskFWriteASTFromRootAsC(file, root->binary_operator.left, 0);
                char *binary_operator_string = "";
                
                switch(root->binary_operator.type)
                {
                    case DATA_DESK_BINARY_OPERATOR_TYPE_add: { binary_operator_string = "+"; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_subtract: { binary_operator_string = "-"; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_multiply: { binary_operator_string = "*"; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_divide: { binary_operator_string = "/"; break; }
                    default: break;
                }
                
                fprintf(file, "%s", binary_operator_string);
                DataDeskFWriteASTFromRootAsC(file, root->binary_operator.right, 0);
                fprintf(file, ")");
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_struct_declaration:
            {
                fprintf(file, "typedef struct %s\n{\n", root->string);
                for(DataDeskASTNode *member = root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    DataDeskFWriteASTFromRootAsC(file, member, 0);
                    fprintf(file, ";\n");
                }
                fprintf(file, "}\n%s;\n\n", root->string);
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_declaration:
            {
                DataDeskFWriteASTFromRootAsC(file, root->declaration.type, 0);
                fprintf(file, "%s", root->string);
                
                for(DataDeskASTNode *array = root->declaration.type->type_usage.first_array_size_expression;
                    array;
                    array = array->next)
                {
                    fprintf(file, "[");
                    DataDeskFWriteASTFromRootAsC(file, array, 0);
                    fprintf(file, "]");
                }
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_type_usage:
            {
                fprintf(file, "%s ", root->string);
                for(int i = 0; i < root->type_usage.pointer_count; ++i)
                {
                    fprintf(file, "*");
                }
                
                break;
            }
            
            default: break;
        }
        
        if(root->next && follow_next)
        {
            DataDeskFWriteASTFromRootAsC(file, root->next, follow_next);
        }
    }
}

void
DataDeskFWriteStructAsC(FILE *file, DataDeskStruct struct_info)
{
    DataDeskFWriteASTFromRootAsC(file, struct_info.root, 0);
}
#endif // DATA_DESK_NO_CRT

#endif // DATA_DESK_H_INCLUDED_