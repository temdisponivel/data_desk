/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 11 May 2020
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef DATA_DESK_H_INCLUDED_
#define DATA_DESK_H_INCLUDED_

#define DATA_DESK_VERSION_MAJOR 1
#define DATA_DESK_VERSION_MINOR 1
#define DATA_DESK_VERSION_PATCH 0
#define DATA_DESK_STRINGIFY_(a) #a
#define DATA_DESK_STRINGIFY(a) DATA_DESK_STRINGIFY_(a)
#define DATA_DESK_VERSION_STRING DATA_DESK_STRINGIFY(DATA_DESK_VERSION_MAJOR) "." DATA_DESK_STRINGIFY(DATA_DESK_VERSION_MINOR) "." DATA_DESK_STRINGIFY(DATA_DESK_VERSION_PATCH)

#ifndef DATA_DESK_USE_OLD_NAMES
#define DATA_DESK_USE_OLD_NAMES 1
#endif

#ifndef DATA_DESK_NO_CRT
#include <stdio.h>
#include <stdarg.h>
#endif

#if defined(_MSC_VER)
#define DATA_DESK_EXPORT __declspec(dllexport)
#if _MSC_VER < 1900
#define MSVC2013_C 1
#endif
#else
#define DATA_DESK_EXPORT
#endif

#if !defined(MSVC2013_C)
#define MSVC2013_C 0
#endif

#if defined(__cplusplus)
#define DATA_DESK_EXTERN_C extern "C"
#else
#define DATA_DESK_EXTERN_C
#endif

#define DATA_DESK_FUNC DATA_DESK_EXPORT DATA_DESK_EXTERN_C
#define DATA_DESK_PROC DATA_DESK_FUNC
#if MSVC2013_C
#define DATA_DESK_HEADER_PROC static
#else
#define DATA_DESK_HEADER_PROC static inline
#endif




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

typedef struct DataDeskNode DataDeskNode;

/* DataDeskCustomInitCallback */
typedef void DataDeskInitCallback(void);

/* DataDeskCustomParseCallback */
typedef void DataDeskParseCallback(DataDeskNode *root, char *filename);

/* DataDeskCustomCleanUpCallback */
typedef void DataDeskCleanUpCallback(void);





/*
| /////////////////////////////////////////////////////////////////
|  Abstract Syntax Tree Representation
| /////////////////////////////////////////////////////////////////
|
| The following code outlines the general structure for the
| abstract syntax trees that Data Desk generates. Each sub-struct
| contained inside of the DataDeskNode struct is only safe to
| access if the "type" variable in the struct is set to the
| corresponding type value. All of the type constants are defined
| in the following list.
*/

typedef enum DataDeskNodeType
{
#if DATA_DESK_USE_OLD_NAMES
    DATA_DESK_NODE_TYPE_invalid,
    DATA_DESK_NODE_TYPE_identifier,
    DATA_DESK_NODE_TYPE_numeric_constant,
    DATA_DESK_NODE_TYPE_string_constant,
    DATA_DESK_NODE_TYPE_char_constant,
    DATA_DESK_NODE_TYPE_unary_operator,
    DATA_DESK_NODE_TYPE_binary_operator,
    DATA_DESK_NODE_TYPE_struct_declaration,
    DATA_DESK_NODE_TYPE_union_declaration,
    DATA_DESK_NODE_TYPE_enum_declaration,
    DATA_DESK_NODE_TYPE_flags_declaration,
    DATA_DESK_NODE_TYPE_constant_definition,
    DATA_DESK_NODE_TYPE_procedure_header,
    DATA_DESK_NODE_TYPE_declaration,
    DATA_DESK_NODE_TYPE_type_usage,
    DATA_DESK_NODE_TYPE_tag,
#else
    DataDeskNodeType_Invalid,
    DataDeskNodeType_Null = DataDeskNodeType_Invalid,
    DataDeskNodeType_Identifier,
    DataDeskNodeType_NumericConstant,
    DataDeskNodeType_StringConstant,
    DataDeskNodeType_CharConstant,
    DataDeskNodeType_UnaryOperator,
    DataDeskNodeType_BinaryOperator,
    DataDeskNodeType_StructDeclaration,
    DataDeskNodeType_UnionDeclaration,
    DataDeskNodeType_EnumDeclaration,
    DataDeskNodeType_FlagsDeclaration,
    DataDeskNodeType_ConstantDefinition,
    DataDeskNodeType_ProcedureHeader,
    DataDeskNodeType_Declaration,
    DataDeskNodeType_TypeUsage,
    DataDeskNodeType_Tag,
#endif
}
DataDeskNodeType;

// NOTE(rjf): Used to store sub-type of main node type (like the specific kind of binary
// operator for a DataDeskNodeType_BinaryOperator node).
typedef unsigned int DataDeskNodeSubType;

// NOTE(rjf): The unary operator precedence table in UnaryOperatorPrecedence
// must update to match this when this changes, and also the DataDeskGetUnaryOperatorString
// procedure in this file.
typedef enum DataDeskUnaryOperatorType
{
    DataDeskUnaryOperatorType_Invalid,
    DataDeskUnaryOperatorType_Null = DataDeskUnaryOperatorType_Invalid,
    DataDeskUnaryOperatorType_Negative,
    DataDeskUnaryOperatorType_Not,
    DataDeskUnaryOperatorType_BitwiseNegate,
    DataDeskUnaryOperatorType_Max
        
#if DATA_DESK_USE_OLD_NAMES
    DATA_DESK_UNARY_OPERATOR_TYPE_invalid = DataDeskUnaryOperatorType_Invalid,
    DATA_DESK_UNARY_OPERATOR_TYPE_negative,
    DATA_DESK_UNARY_OPERATOR_TYPE_not,
    DATA_DESK_UNARY_OPERATOR_TYPE_bitwise_negate,
    DATA_DESK_UNARY_OPERATOR_TYPE_MAX,
#endif
}
DataDeskUnaryOperatorType;

// NOTE(rjf): The binary operator precedence table in BinaryOperatorPrecedence
// must update to match this when this changes, and also the DataDeskGetBinaryOperatorString
// procedure in this file.
typedef enum DataDeskBinaryOperatorType
{
    DataDeskBinaryOperatorType_Invalid,
    DataDeskBinaryOperatorType_Null = DataDeskBinaryOperatorType_Invalid,
    DataDeskBinaryOperatorType_Add,
    DataDeskBinaryOperatorType_Subtract,
    DataDeskBinaryOperatorType_Multiply,
    DataDeskBinaryOperatorType_Divide,
    DataDeskBinaryOperatorType_Modulus,
    DataDeskBinaryOperatorType_BitshiftLeft,
    DataDeskBinaryOperatorType_BitshiftRight,
    DataDeskBinaryOperatorType_BitwiseAnd,
    DataDeskBinaryOperatorType_BitwiseOr,
    DataDeskBinaryOperatorType_BooleanAnd,
    DataDeskBinaryOperatorType_BooleanOr,
    DataDeskBinaryOperatorType_Max,
    
#if DATA_DESK_USE_OLD_NAMES
    DATA_DESK_BINARY_OPERATOR_TYPE_invalid = DataDeskBinaryOperatorType_Invalid,
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
    DATA_DESK_BINARY_OPERATOR_TYPE_MAX,
#endif
}
DataDeskBinaryOperatorType;

typedef enum DataDeskWordStyle
{
    DataDeskWordStyle_Word,
    DataDeskWordStyle_CamelCase = DataDeskWordStyle_Word,
    
    DataDeskWordStyle_WORD,
    DataDeskWordStyle_AllCaps = DataDeskWordStyle_WORD,
    DataDeskWordStyle_UpperCase = DataDeskWordStyle_WORD,
    
    DataDeskWordStyle_word,
    DataDeskWordStyle_SnakeCase = DataDeskWordStyle_word,
    DataDeskWordStyle_LowerCase = DataDeskWordStyle_word,
    
    DataDeskWordStyle_Max,
}
DataDeskWordStyle;

typedef enum DataDeskWordSeparator
{
    DataDeskWordSeparator_None,
    DataDeskWordSeparator_Underscore,
    DataDeskWordSeparator_Space,
    DataDeskWordSeparator_Max,
}
DataDeskWordSeparator;

struct DataDeskNode
{
    //~ NOTE(rjf): AST Relationship Data
    DataDeskNode *next;
    DataDeskNode *prev;
    DataDeskNode *parent;
    DataDeskNode *children_list_head;
    DataDeskNodeType type;
    DataDeskNodeSubType sub_type;
    
    //~ NOTE(rjf): String
    union
    {
        int string_length;
        int name_length;
    };
    union
    {
        char *string;
        char *name;
    };
    
    //~ NOTE(rjf): String Transforms
    
    // NOTE(rjf): DataDeskWordStyle_Max*DataDeskWordStyle_Max => First-word rule multiplied
    // by second+ word rule
    //
    // DataDeskWordSeparator_Max => All possible separators
    
    char *name_transforms[DataDeskWordStyle_Max*DataDeskWordStyle_Max*DataDeskWordSeparator_Max];
    
    //~ NOTE(rjf): File/Line Source Information
    char *file;
    int line;
    
    //~ NOTE(rjf): Tag List
    union
    {
        DataDeskNode *tag_list_head;
        DataDeskNode *tag_list;
        DataDeskNode *first_tag;
    };
    
    //~ NOTE(rjf): Node-specific data shortcuts
    union
    {
        struct
        {
            DataDeskNode *reference;
        }
        identifier;
        
        struct
        {
            DataDeskNode *operand;
        }
        unary_operator;
        
        struct
        {
            DataDeskNode *left;
            DataDeskNode *right;
        }
        binary_operator;
        
        struct
        {
            DataDeskNode *first_member;
        }
        struct_declaration;
        
        struct
        {
            DataDeskNode *first_member;
        }
        union_declaration;
        
        struct
        {
            DataDeskNode *first_constant;
        }
        enum_declaration;
        
        struct
        {
            DataDeskNode *first_flag;
        }
        flags_declaration;
        
        struct
        {
            DataDeskNode *expression;
        }
        constant_definition;
        
        struct
        {
            DataDeskNode *return_type;
            DataDeskNode *first_parameter;
        }
        procedure_header;
        
        struct
        {
            DataDeskNode *type;
            DataDeskNode *initialization;
        }
        declaration;
        
        struct
        {
            int pointer_count;
            DataDeskNode *first_array_size_expression;
            DataDeskNode *struct_declaration;
            DataDeskNode *union_declaration;
            DataDeskNode *type_definition;
        }
        type_usage;
        
        struct
        {
            DataDeskNode *first_tag_parameter;
        }
        tag;
        
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

DATA_DESK_HEADER_PROC int DataDeskStringHasSubString(char *string, char *substring);
DATA_DESK_HEADER_PROC DataDeskNode *DataDeskGetNodeTag(DataDeskNode *root, char *tag);
DATA_DESK_HEADER_PROC DataDeskNode *DataDeskGetTagParameter(DataDeskNode *tag, int parameter_number);
DATA_DESK_HEADER_PROC int DataDeskNodeHasTag(DataDeskNode *root, char *tag);
DATA_DESK_HEADER_PROC int DataDeskDeclarationIsType(DataDeskNode *root, char *type);
DATA_DESK_HEADER_PROC int DataDeskStructMemberIsType(DataDeskNode *root, char *type);
DATA_DESK_HEADER_PROC int DataDeskInterpretNumericExpressionAsInteger(DataDeskNode *root);
DATA_DESK_HEADER_PROC char *DataDeskGetBinaryOperatorString(int type);
DATA_DESK_HEADER_PROC char *DataDeskGetUnaryOperatorString(int type);
DATA_DESK_HEADER_PROC void DataDeskError(DataDeskNode *node, char *format, ...);
DATA_DESK_HEADER_PROC void DataDeskWarning(DataDeskNode *node, char *format, ...);

#ifndef DATA_DESK_NO_CRT
DATA_DESK_HEADER_PROC void DataDeskFWriteGraphAsC(FILE *file, DataDeskNode *root, int follow_next);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringWithSpaces(FILE *file, char *string);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsLowercaseWithUnderscores(FILE *file, char *string);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsUppercaseWithUnderscores(FILE *file, char *string);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsUpperCamelCase(FILE *file, char *string);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsLowerCamelCase(FILE *file, char *string);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringWithSpacesN(FILE *file, char *string, int string_length);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsLowercaseWithUnderscoresN(FILE *file, char *string, int string_length);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsUppercaseWithUnderscoresN(FILE *file, char *string, int string_length);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsUpperCamelCaseN(FILE *file, char *string, int string_length);
DATA_DESK_HEADER_PROC void DataDeskFWriteStringAsLowerCamelCaseN(FILE *file, char *string, int string_length);
#endif





/*
| /////////////////////////////////////////////////////////////////
|  Helper Function Implementation Code
| /////////////////////////////////////////////////////////////////
*/

DATA_DESK_HEADER_PROC int
DataDeskCharIsAlpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

DATA_DESK_HEADER_PROC int
DataDeskCharIsDigit(int c)
{
    return (c >= '0' && c <= '9');
}

DATA_DESK_HEADER_PROC int
DataDeskCharIsLowercaseAlpha(int c)
{
    return (c >= 'a' && c <= 'z');
}

DATA_DESK_HEADER_PROC int
DataDeskCharIsUppercaseAlpha(int c)
{
    return (c >= 'A' && c <= 'Z');
}

DATA_DESK_HEADER_PROC int
DataDeskCharToLower(int c)
{
    return (DataDeskCharIsUppercaseAlpha(c) ? c + 32 : c);
}

DATA_DESK_HEADER_PROC int
DataDeskCharToUpper(int c)
{
    return (DataDeskCharIsLowercaseAlpha(c) ? c - 32 : c);
}

DATA_DESK_HEADER_PROC int
DataDeskStringHasAlphanumericBlock(char *string, char *substring)
{
    int matches = 0;
    
    if(string && substring)
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

DATA_DESK_HEADER_PROC int
DataDeskStringHasSubString(char *string, char *substring)
{
    return DataDeskStringHasAlphanumericBlock(string, substring);
}

DATA_DESK_HEADER_PROC DataDeskNode *
DataDeskGetNodeTag(DataDeskNode *root, char *tag)
{
    DataDeskNode *found_tag_node = 0;
    for(DataDeskNode *tag_node = root->first_tag;
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

DATA_DESK_HEADER_PROC DataDeskNode *
DataDeskGetTagParameter(DataDeskNode *tag, int parameter_number)
{
    DataDeskNode *result = 0;
    if(tag && tag->type == DataDeskNodeType_Tag)
    {
        int i = 0;
        for(DataDeskNode *parameter = tag->tag.first_tag_parameter;
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

DATA_DESK_HEADER_PROC int
DataDeskNodeHasTag(DataDeskNode *node, char *tag)
{
    DataDeskNode *tag_node = DataDeskGetNodeTag(node, tag);
    return tag_node != 0;
}

DATA_DESK_HEADER_PROC int
DataDeskDeclarationIsType(DataDeskNode *root, char *type)
{
    int matches = 0;
    
    if(root->type == DataDeskNodeType_Declaration)
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
            matches = 0;
            if(root->declaration.type->string)
            {
                matches = 1;
                for(int i = 0; type_name[i] || root->declaration.type->string[i]; ++i)
                {
                    if(type_name[i] != root->declaration.type->string[i])
                    {
                        matches = 0;
                        break;
                    }
                }
            }
        }
    }
    
    return matches;
}

DATA_DESK_HEADER_PROC int
DataDeskStructMemberIsType(DataDeskNode *root, char *type)
{
    return DataDeskDeclarationIsType(root, type);
}

DATA_DESK_HEADER_PROC int
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

DATA_DESK_HEADER_PROC int
DataDeskInterpretNumericExpressionAsInteger(DataDeskNode *root)
{
    int result = 0;
    if(root)
    {
        switch(root->type)
        {
            case DataDeskNodeType_NumericConstant:
            {
                result = DataDeskCStringToInt(root->string);
                break;
            }
            case DATA_DESK_NODE_TYPE_unary_operator:
            {
                DataDeskUnaryOperatorType unary_operator_type = root->sub_type;
                int operand = DataDeskInterpretNumericExpressionAsInteger(root->unary_operator.operand);
                
                switch(unary_operator_type)
                {
                    case DATA_DESK_UNARY_OPERATOR_TYPE_not:
                    {
                        result = !operand;
                        break;
                    }
                    case DATA_DESK_UNARY_OPERATOR_TYPE_negative:
                    {
                        result = -operand;
                        break;
                    }
                    case DATA_DESK_UNARY_OPERATOR_TYPE_bitwise_negate:
                    {
                        result = ~operand;
                        break;
                    }
                    default: break;
                }
                
                break;
            }
            case DATA_DESK_NODE_TYPE_binary_operator:
            {
                DataDeskBinaryOperatorType binary_operator_type = root->sub_type;
                int left_tree = DataDeskInterpretNumericExpressionAsInteger(root->binary_operator.left);
                int right_tree = DataDeskInterpretNumericExpressionAsInteger(root->binary_operator.right);
                
                switch(binary_operator_type)
                {
                    case DATA_DESK_BINARY_OPERATOR_TYPE_add:            { result = left_tree + right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_subtract:       { result = left_tree - right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_multiply:       { result = left_tree * right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_divide:         { result = (right_tree != 0) ? (left_tree / right_tree) : 0; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_modulus:        { result = (right_tree != 0) ? (left_tree % right_tree) : 0; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_left:  { result = left_tree << right_tree; break; }
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

DATA_DESK_HEADER_PROC char *
DataDeskGetBinaryOperatorString(int type)
{
    char *strings[] =
    {
        "(invalid)",
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

DATA_DESK_HEADER_PROC char *
DataDeskGetUnaryOperatorString(int type)
{
    char *strings[] =
    {
        "(invalid)",
        "-",
        "!",
        "~",
    };
    return strings[type];
}

DATA_DESK_HEADER_PROC void
DataDeskError(DataDeskNode *node, char *format, ...)
{
#ifndef DATA_DESK_NO_CRT
    char *filename = "";
    int line = 0;
    
    if(node)
    {
        filename = node->file;
        line = node->line;
    }
    
    va_list args;
    va_start(args, format);
    fprintf(stderr, "ERROR (%s:%i): ", filename, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
#endif
}

DATA_DESK_HEADER_PROC void
DataDeskWarning(DataDeskNode *node, char *format, ...)
{
#ifndef DATA_DESK_NO_CRT
    char *filename = "";
    int line = 0;
    
    if(node)
    {
        filename = node->file;
        line = node->line;
    }
    
    va_list args;
    va_start(args, format);
    fprintf(stderr, "WARNING (%s:%i): ", filename, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
#endif
}

#ifndef DATA_DESK_NO_CRT
DATA_DESK_HEADER_PROC void
_DataDeskFWriteGraphAsC(FILE *file, DataDeskNode *root, int follow_next, int nest)
{
    if(root)
    {
        if(root->first_tag)
        {
            fprintf(file, "// ");
            for(DataDeskNode *tag = root->first_tag; tag; tag = tag->next)
            {
                fprintf(file, "%s", tag->string);
                
                if(tag->tag.first_tag_parameter)
                {
                    fprintf(file, "(");
                    for(DataDeskNode *tag_arg = tag->tag.first_tag_parameter; tag_arg; tag_arg = tag_arg->next)
                    {
                        _DataDeskFWriteGraphAsC(file, tag_arg, 0, nest+1);
                        if(tag_arg->next)
                        {
                            fprintf(file, ", ");
                        }
                    }
                    fprintf(file, ")");
                }
                fprintf(file, " ");
            }
            fprintf(file, "\n");
        }
        
        switch(root->type)
        {
            case DATA_DESK_NODE_TYPE_identifier:
            case DATA_DESK_NODE_TYPE_numeric_constant:
            case DATA_DESK_NODE_TYPE_string_constant:
            case DATA_DESK_NODE_TYPE_char_constant:
            {
                fprintf(file, "%s", root->string);
                break;
            }
            
            case DATA_DESK_NODE_TYPE_unary_operator:
            {
                fprintf(file, "(");
                char *unary_operator_string = DataDeskGetUnaryOperatorString(root->sub_type);
                fprintf(file, "%s", unary_operator_string);
                fprintf(file, "(");
                _DataDeskFWriteGraphAsC(file, root->unary_operator.operand, 0, nest+1);
                fprintf(file, ")");
                fprintf(file, ")");
                break;
            }
            
            case DATA_DESK_NODE_TYPE_binary_operator:
            {
                fprintf(file, "(");
                _DataDeskFWriteGraphAsC(file, root->binary_operator.left, 0, nest);
                char *binary_operator_string = DataDeskGetBinaryOperatorString(root->sub_type);
                fprintf(file, "%s", binary_operator_string);
                _DataDeskFWriteGraphAsC(file, root->binary_operator.right, 0, nest+1);
                fprintf(file, ")");
                
                break;
            }
            
            case DATA_DESK_NODE_TYPE_struct_declaration:
            {
                if(nest == 0)
                {
                    fprintf(file, "typedef struct %s %s;\n", root->string, root->string);
                }
                
                if(root->string)
                {
                    fprintf(file, "struct %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "struct\n{\n");
                }
                
                for(DataDeskNode *member = root->struct_declaration.first_member;
                    member; member = member->next)
                {
                    _DataDeskFWriteGraphAsC(file, member, 0, nest+1);
                    fprintf(file, ";\n");
                }
                fprintf(file, "}");
                
                if(nest == 0)
                {
                    fprintf(file, ";\n\n");
                }
                
                break;
            }
            
            case DATA_DESK_NODE_TYPE_union_declaration:
            {
                if(nest == 0)
                {
                    fprintf(file, "typedef union %s %s;\n", root->string, root->string);
                }
                
                if(root->string)
                {
                    fprintf(file, "union %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "union\n{\n");
                }
                
                for(DataDeskNode *member = root->union_declaration.first_member;
                    member; member = member->next)
                {
                    _DataDeskFWriteGraphAsC(file, member, 0, nest+1);
                    fprintf(file, ";\n");
                }
                fprintf(file, "}");
                
                if(nest == 0)
                {
                    fprintf(file, ";\n\n");
                }
                
                break;
            }
            
            case DATA_DESK_NODE_TYPE_enum_declaration:
            {
                if(nest == 0)
                {
                    fprintf(file, "typedef %s %s %s;\n", "enum", root->string, root->string);
                }
                
                if(root->string)
                {
                    fprintf(file, "enum %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "enum\n{\n");
                }
                
                for(DataDeskNode *member = root->enum_declaration.first_constant;
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
            
            case DATA_DESK_NODE_TYPE_flags_declaration:
            {
                int needed_bits_for_flag_type = 32;
                int current_bit = 0;
                for(DataDeskNode *member = root->flags_declaration.first_flag;
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
            
            case DATA_DESK_NODE_TYPE_declaration:
            {
                _DataDeskFWriteGraphAsC(file, root->declaration.type, 0, nest+1);
                fprintf(file, "%s", root->string);
                
                for(DataDeskNode *array = root->declaration.type->type_usage.first_array_size_expression;
                    array;
                    array = array->next)
                {
                    fprintf(file, "[");
                    _DataDeskFWriteGraphAsC(file, array, 0, nest);
                    fprintf(file, "]");
                }
                
                break;
            }
            
            case DATA_DESK_NODE_TYPE_type_usage:
            {
                if(root->type_usage.struct_declaration)
                {
                    _DataDeskFWriteGraphAsC(file, root->type_usage.struct_declaration, 0, nest+1);
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
            
            case DATA_DESK_NODE_TYPE_constant_definition:
            {
                fprintf(file, "#define %s (", root->string);
                DataDeskFWriteGraphAsC(file, root->constant_definition.expression, 0);
                fprintf(file, ")\n");
                break;
            }
            
            case DATA_DESK_NODE_TYPE_procedure_header:
            {
                if(root->procedure_header.return_type)
                {
                    _DataDeskFWriteGraphAsC(file, root->procedure_header.return_type, 0, nest);
                }
                else
                {
                    fprintf(file, "void");
                }
                fprintf(file, " %s(", root->string);
                if(root->procedure_header.first_parameter)
                {
                    for(DataDeskNode *parameter = root->procedure_header.first_parameter;
                        parameter; parameter = parameter->next)
                    {
                        _DataDeskFWriteGraphAsC(file, parameter, 0, nest);
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
            _DataDeskFWriteGraphAsC(file, root->next, follow_next, nest);
        }
    }
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteGraphAsC(FILE *file, DataDeskNode *root, int follow_next)
{
    _DataDeskFWriteGraphAsC(file, root, follow_next, 0);
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringWithSpaces(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringWithSpacesN(file, string, string_length);
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringAsLowercaseWithUnderscores(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsLowercaseWithUnderscoresN(file, string, string_length);
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringAsUppercaseWithUnderscores(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsUppercaseWithUnderscoresN(file, string, string_length);
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringAsUpperCamelCase(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsUpperCamelCaseN(file, string, string_length);
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringAsLowerCamelCase(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsLowerCamelCaseN(file, string, string_length);
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringWithSpacesN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            fprintf(file, "%c", string[i]);
            if(DataDeskCharIsUppercaseAlpha(string[i+1]))
            {
                fprintf(file, " ");
            }
            else if(string[i+1] == '_')
            {
                fprintf(file, " ");
            }
        }
    }
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringAsLowercaseWithUnderscoresN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        fprintf(file, "%c", DataDeskCharToLower(string[i]));
        if(DataDeskCharIsUppercaseAlpha(string[i+1]))
        {
            fprintf(file, "_");
        }
    }
}

DATA_DESK_HEADER_PROC void
DataDeskFWriteStringAsUppercaseWithUnderscoresN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        fprintf(file, "%c", DataDeskCharToUpper(string[i]));
        if(DataDeskCharIsUppercaseAlpha(string[i+1]))
        {
            fprintf(file, "_");
        }
    }
}

DATA_DESK_HEADER_PROC void
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

DATA_DESK_HEADER_PROC void
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

/*
Copyright 2020 Ryan Fleury

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
