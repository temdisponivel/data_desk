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

typedef struct DataDeskParsedNode DataDeskParsedNode;
struct DataDeskParsedNode
{
    char *name;
    char *name_lowercase_with_underscores;
    char *name_uppercase_with_underscores;
    char *name_lower_camel_case;
    char *name_upper_camel_case;
    char *name_with_spaces;
    DataDeskASTNode *root;
};

typedef DataDeskParsedNode DataDeskConstant;
typedef DataDeskParsedNode DataDeskStruct;
typedef DataDeskParsedNode DataDeskUnion;
typedef DataDeskParsedNode DataDeskEnum;
typedef DataDeskParsedNode DataDeskFlags;
typedef DataDeskParsedNode DataDeskDeclaration;
typedef DataDeskParsedNode DataDeskProcedureHeader;

/* DataDeskCustomInitCallback */
typedef void DataDeskInitCallback(void);

/* DataDeskCustomFileCallback */
typedef void DataDeskFileCallback(DataDeskASTNode *root, char *filename);

/* DataDeskCustomConstantCallback */
typedef void DataDeskConstantCallback(DataDeskConstant constant, char *filename);

/* DataDeskCustomStructCallback */
typedef void DataDeskStructCallback(DataDeskStruct parsed_struct, char *filename);

/* DataDeskCustomUnionCallback */
typedef void DataDeskUnionCallback(DataDeskUnion parsed_union, char *filename);

/* DataDeskCustomEnumCallback */
typedef void DataDeskEnumCallback(DataDeskEnum parsed_enum, char *filename);

/* DataDeskCustomFlagsCallback */
typedef void DataDeskFlagsCallback(DataDeskFlags parsed_flags, char *filename);

/* DataDeskCustomDeclarationCallback */
typedef void DataDeskDeclarationCallback(DataDeskDeclaration declaration, char *filename);

/* DataDeskCustomProcedureHeaderCallback */
typedef void DataDeskProcedureHeaderCallback(DataDeskProcedureHeader procedure, char *filename);

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
    DATA_DESK_AST_NODE_TYPE_union_declaration,
    DATA_DESK_AST_NODE_TYPE_enum_declaration,
    DATA_DESK_AST_NODE_TYPE_flags_declaration,
    DATA_DESK_AST_NODE_TYPE_declaration,
    DATA_DESK_AST_NODE_TYPE_type_usage,
    DATA_DESK_AST_NODE_TYPE_tag,
    DATA_DESK_AST_NODE_TYPE_constant_definition,
    DATA_DESK_AST_NODE_TYPE_procedure_header,
};

// NOTE(rjf): The binary operator precedence table in BinaryOperatorPrecedence
// must update to match this when this changes, and also the DataDeskGetBinaryOperatorString
// procedure in this file.
enum
{
    DATA_DESK_BINARY_OPERATOR_TYPE_invalid,
    DATA_DESK_BINARY_OPERATOR_TYPE_add,
    DATA_DESK_BINARY_OPERATOR_TYPE_subtract,
    DATA_DESK_BINARY_OPERATOR_TYPE_multiply,
    DATA_DESK_BINARY_OPERATOR_TYPE_divide,
    DATA_DESK_BINARY_OPERATOR_TYPE_modulus,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_left,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_right,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitwise_and,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitwise_or,
    DATA_DESK_BINARY_OPERATOR_TYPE_boolean_and,
    DATA_DESK_BINARY_OPERATOR_TYPE_boolean_or,
    DATA_DESK_BINARY_OPERATOR_TYPE_MAX
};

struct DataDeskASTNode
{
    int type;
    DataDeskASTNode *next;
    
    char *string;
    int string_length;
    
    DataDeskASTNode *first_tag;
    
    union
    {
        struct Identifier
        {
            DataDeskASTNode *declaration;
        }
        identifier;
        
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
        
        struct UnionDeclaration
        {
            DataDeskASTNode *first_member;
        }
        union_declaration;
        
        struct EnumDeclaration
        {
            DataDeskASTNode *first_constant;
        }
        enum_declaration;
        
        struct FlagsDeclaration
        {
            DataDeskASTNode *first_flag;
        }
        flags_declaration;
        
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
            DataDeskASTNode *union_declaration;
            DataDeskASTNode *type_definition;
        }
        type_usage;
        
        struct Tag
        {
            DataDeskASTNode *first_tag_parameter;
        }
        tag;
        
        struct ConstantDefinition
        {
            DataDeskASTNode *expression;
        }
        constant_definition;
        
        struct ProcedureHeader
        {
            DataDeskASTNode *return_type;
            DataDeskASTNode *first_parameter;
        }
        procedure_header;
    };
};





/*
| /////////////////////////////////////////////////////////////////
 |  Introspection Helper Functions
 | /////////////////////////////////////////////////////////////////
 |
 | The following function prototypes are convenience utility
 | functions that can be helpful when introspecting upon abstract
 | syntax trees.
 */

int DataDeskStringHasSubString(char *string, char *substring);
DataDeskASTNode *DataDeskGetNodeTag(DataDeskASTNode *root, char *tag);
DataDeskASTNode *DataDeskGetTagParameter(DataDeskASTNode *tag, int parameter_number);
int DataDeskNodeHasTag(DataDeskASTNode *root, char *tag);
int DataDeskDeclarationIsType(DataDeskASTNode *root, char *type);
int DataDeskStructMemberIsType(DataDeskASTNode *root, char *type);
int DataDeskInterpretNumericExpressionAsInteger(DataDeskASTNode *root);
inline char *DataDeskGetBinaryOperatorString(int type);

#ifndef DATA_DESK_NO_CRT
void DataDeskFWriteConstantAsC          (FILE *file, DataDeskConstant        constant_info);
void DataDeskFWriteStructAsC            (FILE *file, DataDeskStruct          struct_info);
void DataDeskFWriteUnionAsC             (FILE *file, DataDeskUnion           union_info);
void DataDeskFWriteEnumAsC              (FILE *file, DataDeskEnum            enum_info);
void DataDeskFWriteFlagsAsC             (FILE *file, DataDeskFlags           flags_info);
void DataDeskFWriteDeclarationAsC       (FILE *file, DataDeskDeclaration     declaration_info);
void DataDeskFWriteProcedureHeaderAsC   (FILE *file, DataDeskProcedureHeader procedure_info);
void DataDeskFWriteStringWithSpaces(FILE *file, char *string);
void DataDeskFWriteStringAsLowercaseWithUnderscores(FILE *file, char *string);
void DataDeskFWriteStringAsUppercaseWithUnderscores(FILE *file, char *string);
void DataDeskFWriteStringAsUpperCamelCase(FILE *file, char *string);
void DataDeskFWriteStringAsLowerCamelCase(FILE *file, char *string);
void DataDeskFWriteStringWithSpacesN(FILE *file, char *string, int string_length);
void DataDeskFWriteStringAsLowercaseWithUnderscoresN(FILE *file, char *string, int string_length);
void DataDeskFWriteStringAsUppercaseWithUnderscoresN(FILE *file, char *string, int string_length);
void DataDeskFWriteStringAsUpperCamelCaseN(FILE *file, char *string, int string_length);
void DataDeskFWriteStringAsLowerCamelCaseN(FILE *file, char *string, int string_length);
#endif





/*
| /////////////////////////////////////////////////////////////////
|  Helper Function Implementation Code
| /////////////////////////////////////////////////////////////////
*/

inline int
DataDeskCharIsAlpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

inline int
DataDeskCharIsDigit(int c)
{
    return (c >= '0' && c <= '9');
}

inline int
DataDeskCharIsLowercaseAlpha(int c)
{
    return (c >= 'a' && c <= 'z');
}

inline int
DataDeskCharIsUppercaseAlpha(int c)
{
    return (c >= 'A' && c <= 'Z');
}

inline int
DataDeskCharToLower(int c)
{
    return (DataDeskCharIsUppercaseAlpha(c) ? c + 32 : c);
}

inline int
DataDeskCharToUpper(int c)
{
    return (DataDeskCharIsLowercaseAlpha(c) ? c - 32 : c);
}

inline int
DataDeskStringHasAlphanumericBlock(char *string, char *substring)
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
                        if(substring_i > 0 &&
                           (DataDeskCharIsAlpha(substring[substring_i-1]) ||
                            DataDeskCharIsDigit(substring[substring_i-1]) ||
                            substring[substring_i-1] == '_'))
                        {
                            if(DataDeskCharIsAlpha(string[string_i]) ||
                               DataDeskCharIsDigit(string[string_i]) ||
                               string[string_i] == '_')
                            {
                                matches = 0;
                            }
                        }
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

inline int
DataDeskStringHasSubString(char *string, char *substring)
{
    return DataDeskStringHasAlphanumericBlock(string, substring);
}

inline DataDeskASTNode *
DataDeskGetNodeTag(DataDeskASTNode *root, char *tag)
{
    DataDeskASTNode *found_tag_node = 0;
    for(DataDeskASTNode *tag_node = root->first_tag;
        tag_node; tag_node = tag_node->next)
    {
        if(DataDeskStringHasAlphanumericBlock(tag_node->string, tag))
        {
            found_tag_node = tag_node;
            break;
        }
    }
    return found_tag_node;
}

inline DataDeskASTNode *
DataDeskGetTagParameter(DataDeskASTNode *tag, int parameter_number)
{
    DataDeskASTNode *result = 0;
    if(tag && tag->type == DATA_DESK_AST_NODE_TYPE_tag)
    {
        int i = 0;
        for(DataDeskASTNode *parameter = tag->tag.first_tag_parameter;
            parameter; parameter = parameter->next)
        {
            if(i == parameter_number)
            {
                result = parameter;
                break;
            }
            ++i;
        }
    }
    return result;
}

inline int
DataDeskNodeHasTag(DataDeskASTNode *node, char *tag)
{
    DataDeskASTNode *tag_node = DataDeskGetNodeTag(node, tag);
    return tag_node != 0;
}

inline int
DataDeskDeclarationIsType(DataDeskASTNode *root, char *type)
{
    int matches = 0;
    
    if(root->type == DATA_DESK_AST_NODE_TYPE_declaration)
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
        
        matches = pointer_count == root->declaration.type->type_usage.pointer_count;
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
    }
    
    return matches;
}

inline int
DataDeskStructMemberIsType(DataDeskASTNode *root, char *type)
{
    return DataDeskDeclarationIsType(root, type);
}

inline int
DataDeskCStringToInt(char *string)
{
    int value = 0;
    
    char value_str[64] = {0};
    int value_str_write_pos = 0;
    for(int i = 0; string[i]; ++i)
    {
        if(DataDeskCharIsDigit(string[i]))
        {
            for(int j = i; string[j]; ++j)
            {
                if(DataDeskCharIsDigit(string[j]))
                {
                    value_str[value_str_write_pos++] = string[j];
                }
                else
                {
                    break;
                }
            }
            
            int number_of_digits = value_str_write_pos;
            int multiplier = 1;
            for(int j = 0; j < number_of_digits; ++j)
            {
                value += (value_str[number_of_digits - j - 1] - '0') * multiplier;
                multiplier *= 10;
            }
            
            break;
        }
    }
    
    return value;
}

inline int
DataDeskInterpretNumericExpressionAsInteger(DataDeskASTNode *root)
{
    int result = 0;
    if(root)
    {
        switch(root->type)
        {
            case DATA_DESK_AST_NODE_TYPE_numeric_constant:
            {
                result = DataDeskCStringToInt(root->string);
                break;
            }
            case DATA_DESK_AST_NODE_TYPE_binary_operator:
            {
                int binary_operator_type = root->binary_operator.type;
                int left_tree = DataDeskInterpretNumericExpressionAsInteger(root->binary_operator.left);
                int right_tree = DataDeskInterpretNumericExpressionAsInteger(root->binary_operator.right);
                
                switch(binary_operator_type)
                {
                    case DATA_DESK_BINARY_OPERATOR_TYPE_add: { result = left_tree + right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_subtract: { result = left_tree - right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_multiply: { result = left_tree * right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_divide: { result = (right_tree != 0) ? (left_tree / right_tree) : 0; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_modulus: { result = (right_tree != 0) ? (left_tree % right_tree) : 0; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_left: { result = left_tree << right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_right: { result = left_tree >> right_tree; break; }
                    default: break;
                }
                
                break;
            }
            default: break;
        }
    }
    return result;
}

inline char *
DataDeskGetBinaryOperatorString(int type)
{
    static char *strings[] =
    {
        "",
        "+",
        "-",
        "*",
        "/",
        "%",
        "<<",
        ">>",
        "&",
        "|",
        "&&",
        "||",
    };
    return strings[type];
}

#ifndef DATA_DESK_NO_CRT
inline void
_DataDeskFWriteASTFromRootAsC(FILE *file, DataDeskASTNode *root, int follow_next, int nest)
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
                _DataDeskFWriteASTFromRootAsC(file, root->binary_operator.left, 0, nest);
                char *binary_operator_string = DataDeskGetBinaryOperatorString(root->binary_operator.type);
                fprintf(file, "%s", binary_operator_string);
                _DataDeskFWriteASTFromRootAsC(file, root->binary_operator.right, 0, nest+1);
                fprintf(file, ")");
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_struct_declaration:
            case DATA_DESK_AST_NODE_TYPE_union_declaration:
            {
                char *definition_type_string = "";
                if(root->type == DATA_DESK_AST_NODE_TYPE_struct_declaration)
                {
                    definition_type_string = "struct";
                }
                else if(root->type == DATA_DESK_AST_NODE_TYPE_union_declaration)
                {
                    definition_type_string = "union";
                }
                
                if(root->string)
                {
                    fprintf(file, "%s %s\n{\n", definition_type_string, root->string);
                }
                else
                {
                    fprintf(file, "%s\n{\n", definition_type_string);
                }
                
                for(DataDeskASTNode *member = root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    _DataDeskFWriteASTFromRootAsC(file, member, 0, nest+1);
                    fprintf(file, ";\n");
                }
                fprintf(file, "}");
                
                if(nest == 0)
                {
                    fprintf(file, ";\n\n");
                }
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_enum_declaration:
            {
                if(root->string)
                {
                    fprintf(file, "enum %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "enum\n{\n");
                }
                
                for(DataDeskASTNode *member = root->enum_declaration.first_constant;
                    member;
                    member = member->next)
                {
                    fprintf(file, "%s", member->string);
                    fprintf(file, ",\n");
                }
                fprintf(file, "}");
                
                fprintf(file, ";\n\n");
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_flags_declaration:
            {
                int needed_bits_for_flag_type = 32;
                int current_bit = 0;
                for(DataDeskASTNode *member = root->flags_declaration.first_flag;
                    member;
                    member = member->next)
                {
                    fprintf(file, "#define %s (1<<%i)\n", member->string, current_bit);
                    ++current_bit;
                }
                
                if(current_bit >= 31)
                {
                    needed_bits_for_flag_type = 64;
                }
                
                if(root->string)
                {
                    if(needed_bits_for_flag_type == 32)
                    {
                        fprintf(file, "typedef unsigned int %s;\n\n", root->string);
                    }
                    else if(needed_bits_for_flag_type > 32)
                    {
                        fprintf(file, "typedef unsigned long int %s;\n\n", root->string);
                    }
                }
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_declaration:
            {
                _DataDeskFWriteASTFromRootAsC(file, root->declaration.type, 0, nest+1);
                fprintf(file, "%s", root->string);
                
                for(DataDeskASTNode *array = root->declaration.type->type_usage.first_array_size_expression;
                    array;
                    array = array->next)
                {
                    fprintf(file, "[");
                    _DataDeskFWriteASTFromRootAsC(file, array, 0, nest);
                    fprintf(file, "]");
                }
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_type_usage:
            {
                if(root->type_usage.struct_declaration)
                {
                    _DataDeskFWriteASTFromRootAsC(file, root->type_usage.struct_declaration, 0, nest+1);
                    fprintf(file, "\n");
                }
                else
                {
                    fprintf(file, "%s ", root->string);
                }
                
                for(int i = 0; i < root->type_usage.pointer_count; ++i)
                {
                    fprintf(file, "*");
                }
                
                break;
            }
            
            case DATA_DESK_AST_NODE_TYPE_procedure_header:
            {
                if(root->procedure_header.return_type)
                {
                    _DataDeskFWriteASTFromRootAsC(file, root->procedure_header.return_type, 0, nest);
                }
                else
                {
                    fprintf(file, "void");
                }
                fprintf(file, " %s(", root->string);
                if(root->procedure_header.first_parameter)
                {
                    for(DataDeskASTNode *parameter = root->procedure_header.first_parameter;
                        parameter; parameter = parameter->next)
                    {
                        _DataDeskFWriteASTFromRootAsC(file, parameter, 0, nest);
                        if(parameter->next)
                        {
                            fprintf(file, ", ");
                        }
                    }
                }
                else
                {
                    fprintf(file, "void");
                }
                fprintf(file, ");\n");
                break;
            }
            
            default: break;
        }
        
        if(root->next && follow_next)
        {
            _DataDeskFWriteASTFromRootAsC(file, root->next, follow_next, nest);
        }
    }
}

inline void
DataDeskFWriteASTFromRootAsC(FILE *file, DataDeskASTNode *root, int follow_next)
{
    _DataDeskFWriteASTFromRootAsC(file, root, follow_next, 0);
}

inline void
DataDeskFWriteConstantAsC(FILE *file, DataDeskConstant constant_info)
{
    fprintf(file, "#define %s (", constant_info.name);
    DataDeskFWriteASTFromRootAsC(file, constant_info.root->constant_definition.expression, 0);
    fprintf(file, ")\n");
}

inline void
DataDeskFWriteStructAsC(FILE *file, DataDeskStruct struct_info)
{
    fprintf(file, "typedef struct %s %s;\n", struct_info.name, struct_info.name);
    DataDeskFWriteASTFromRootAsC(file, struct_info.root, 0);
}

inline void
DataDeskFWriteUnionAsC(FILE *file, DataDeskUnion union_info)
{
    fprintf(file, "typedef union %s %s;\n", union_info.name, union_info.name);
    DataDeskFWriteASTFromRootAsC(file, union_info.root, 0);
}

inline void
DataDeskFWriteEnumAsC(FILE *file, DataDeskEnum enum_info)
{
    DataDeskFWriteASTFromRootAsC(file, enum_info.root, 0);
}

inline void
DataDeskFWriteFlagsAsC(FILE *file, DataDeskFlags flags_info)
{
    DataDeskFWriteASTFromRootAsC(file, flags_info.root, 0);
}

inline void
DataDeskFWriteDeclarationAsC(FILE *file, DataDeskDeclaration declaration_info)
{
    DataDeskFWriteASTFromRootAsC(file, declaration_info.root, 0);
}

inline void
DataDeskFWriteProcedureHeaderAsC(FILE *file, DataDeskProcedureHeader procedure_info)
{
    DataDeskFWriteASTFromRootAsC(file, procedure_info.root, 0);
}

inline void
DataDeskFWriteStringWithSpaces(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringWithSpacesN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsLowercaseWithUnderscores(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsLowercaseWithUnderscoresN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsUppercaseWithUnderscores(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsUppercaseWithUnderscoresN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsUpperCamelCase(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsUpperCamelCaseN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsLowerCamelCase(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsLowerCamelCaseN(file, string, string_length);
}

inline void
DataDeskFWriteStringWithSpacesN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            fprintf(file, "%c", string[i]);
            if(DataDeskCharIsUppercaseAlpha(string[i+1]))
            {
                fprintf(file, " ", string[i]);
            }
            else if(string[i+1] == '_')
            {
                fprintf(file, " ", string[i]);
            }
        }
    }
}

inline void
DataDeskFWriteStringAsLowercaseWithUnderscoresN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        fprintf(file, "%c", DataDeskCharToLower(string[i]));
        if(DataDeskCharIsUppercaseAlpha(string[i+1]))
        {
            fprintf(file, "_", string[i]);
        }
    }
}

inline void
DataDeskFWriteStringAsUppercaseWithUnderscoresN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        fprintf(file, "%c", DataDeskCharToUpper(string[i]));
        if(DataDeskCharIsUppercaseAlpha(string[i+1]))
        {
            fprintf(file, "_", string[i]);
        }
    }
}

inline void
DataDeskFWriteStringAsUpperCamelCaseN(FILE *file, char *string, int string_length)
{
    int needs_uppercase = 1;
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            if(string[i+1] == '_')
            {
                needs_uppercase = 1;
            }
        }
        
        if(needs_uppercase)
        {
            fprintf(file, "%c", DataDeskCharToUpper(string[i]));
            needs_uppercase = 0;
        }
        else
        {
            fprintf(file, "%c", string[i]);
        }
    }
}

inline void
DataDeskFWriteStringAsLowerCamelCaseN(FILE *file, char *string, int string_length)
{
    int needs_uppercase = 0;
    int need_first_lowercase = 1;
    
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            if(string[i+1] == '_')
            {
                needs_uppercase = 1;
            }
        }
        
        if(needs_uppercase)
        {
            fprintf(file, "%c", DataDeskCharToUpper(string[i]));
            needs_uppercase = 0;
        }
        else
        {
            if(need_first_lowercase)
            {
                need_first_lowercase = 0;
                fprintf(file, "%c", DataDeskCharToLower(string[i]));
            }
            else
            {
                fprintf(file, "%c", string[i]);
            }
        }
    }
}

#endif // DATA_DESK_NO_CRT

#endif // DATA_DESK_H_INCLUDED_