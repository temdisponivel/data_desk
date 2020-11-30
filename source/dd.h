#ifndef DD_H
#define DD_H

#if defined(DD_WIN32) || defined(DD_POSIX)
#define DD_OS 1
#else
#define DD_OS 0
#endif

#ifndef DD_WIN32
#define DD_WIN32 0
#endif

#ifndef DD_POSIX
#define DD_POSIX 0
#endif

#define DD_FUNCTION

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef int8_t   DD_i8;
typedef int16_t  DD_i16;
typedef int32_t  DD_i32;
typedef int64_t  DD_i64;
typedef uint8_t  DD_u8;
typedef uint16_t DD_u16;
typedef uint32_t DD_u32;
typedef uint64_t DD_u64;
typedef int8_t   DD_b8;
typedef int16_t  DD_b16;
typedef int32_t  DD_b32;
typedef int64_t  DD_b64;

//~ Basic UTF-8 string types.

typedef struct DD_String8 DD_String8;
struct DD_String8
{
    DD_u8 *str;
    DD_u64 size;
};

typedef struct DD_String8Node DD_String8Node;
struct DD_String8Node
{
    DD_String8Node *next;
    DD_String8 string;
};

typedef struct DD_String8List DD_String8List;
struct DD_String8List
{
    DD_String8Node *first;
    DD_String8Node *last;
};

typedef DD_u32 DD_StringMatchFlags;
enum
{
    DD_StringMatchFlag_CaseInsensitive = (1<<0),
    DD_StringMatchFlag_RightSideSloppy = (1<<1),
    DD_StringMatchFlag_FindLast        = (1<<2),
};

//~ Node kinds that comprise the language.
typedef enum DD_NodeKind
{
    DD_NodeKind_Null,
    DD_NodeKind_Identifier,
    DD_NodeKind_Symbol,
    DD_NodeKind_NumericLiteral,
    DD_NodeKind_StringLiteral,
    DD_NodeKind_CharLiteral,
    DD_NodeKind_Set,
    DD_NodeKind_Tag,
    DD_NodeKind_MAX,
}
DD_NodeKind;

//~ Node type and helpers.

typedef struct DD_Node DD_Node;
typedef struct DD_NodeList DD_NodeList;

struct DD_NodeList
{
    DD_Node *first;
    DD_Node *last;
};

struct DD_Node
{
    // Tree relationship pointers.
    DD_Node *next;
    DD_Node *prev;
    DD_Node *parent;
    DD_NodeList children;
    
    // Tag list.
    DD_NodeList tags;
    
    // Node info.
    DD_NodeKind kind;
    DD_String8 string;
    DD_String8 whole_string;
    
    // Source code location information.
    DD_String8 file;
    DD_u64 line;
};

//~ String-To-Node table

typedef enum DD_NodeTableCollisionRule
{
    DD_NodeTableCollisionRule_Chain,
    DD_NodeTableCollisionRule_Overwrite,
}
DD_NodeTableCollisionRule;

typedef struct DD_NodeTableSlot DD_NodeTableSlot;
struct DD_NodeTableSlot
{
    DD_NodeTableSlot *next;
    DD_u64 hash;
    DD_Node *node;
};

typedef struct DD_NodeTable DD_NodeTable;
struct DD_NodeTable
{
    DD_u64 table_size;
    DD_NodeTableSlot **table;
};

//~ Token kinds.

typedef enum DD_TokenKind
{
    DD_TokenKind_Null,
    
    // A group of characters that begins with an underscore or alphabetic character,
    // and consists of numbers, alphabetic characters, or underscores after that.
    DD_TokenKind_Identifier,
    
    // A group of characters beginning with a numeric character or a '-', and then
    // consisting of only numbers, alphabetic characters, or '.'s after that.
    DD_TokenKind_NumericLiteral,
    
    // A group of arbitrary characters, grouped together by a " character, OR by a
    // """ symbol at the beginning and end of the group. String literals beginning with
    // " are to only be specified on a single line, but """ strings can exist across
    // many lines.
    DD_TokenKind_StringLiteral,
    
    // A group of arbitrary characters, grouped together by a ' character at the beginning,
    // and a ' character at the end.
    DD_TokenKind_CharLiteral,
    
    // A group of symbolic characters, where symbolic characters means any of the following:
    // ~!@#$%^&*()-+=[{]}:;<>,./?|\
//
    // Groups of multiple characters are only allowed in specific circumstances. Most of these
    // are only 1 character long, but some groups are allowed:
    //
    // "<<", ">>", "<=", ">=", "+=", "-=", "*=", "/=", "::", ":=", "==", "&=", "|=", "->"
    DD_TokenKind_Symbol,
    
    DD_TokenKind_MAX,
}
DD_TokenKind;

//~ Token type.
typedef struct DD_Token DD_Token;
struct DD_Token
{
    DD_TokenKind kind;
    DD_String8 string;
    DD_String8 outer_string;
};

//~ Struct used for tokenization state.
typedef struct DD_Tokenizer DD_Tokenizer;
struct DD_Tokenizer
{
    DD_u8 *at;
    DD_u64 at_line;
    DD_String8 filename;
    DD_String8 file_contents;
};

//~ Parsing State

typedef struct DD_Error DD_Error;
struct DD_Error
{
    DD_Error *next;
    DD_String8 string;
    DD_String8 filename;
    DD_Node *node;
    DD_u64 line;
};

typedef struct DD_ParseCtx DD_ParseCtx;
struct DD_ParseCtx
{
    DD_NodeList roots;
    DD_Error *first_error;
    DD_Error *last_error;
};

//~ Parse results.
typedef struct DD_ParseResult DD_ParseResult;
struct DD_ParseResult
{
    DD_NodeList roots;
    DD_Error *first_error;
};

//~ Command line parsing helper types.
typedef struct DD_CommandLine DD_CommandLine;
struct DD_CommandLine
{
    DD_String8 *arguments;
    int argument_count;
};

//~ File system access types.

typedef DD_u32 DD_FileFlags;
enum
{
    DD_FileFlag_Directory = (1<<0),
};

typedef struct DD_FileInfo DD_FileInfo;
struct DD_FileInfo
{
    DD_FileFlags flags;
    DD_String8 path;
    DD_String8 extension;
    DD_u64 file_size;
};

typedef struct DD_FileIter DD_FileIter;
struct DD_FileIter
{
    // This is opaque state to store OS-specific file-system iteration data.
    DD_u64 state;
};

//~ Basic Utility Functions
DD_FUNCTION DD_b32 DD_CharIsAlpha(DD_u8 c);
DD_FUNCTION DD_b32 DD_CharIsDigit(DD_u8 c);
DD_FUNCTION DD_b32 DD_CharIsSymbol(DD_u8 c);
DD_FUNCTION DD_u8  DD_CharToUpper(DD_u8 c);
DD_FUNCTION DD_u8  DD_CharToLower(DD_u8 c);

//~ String Functions
DD_FUNCTION DD_String8     DD_S8(DD_u8 *str, DD_u64 size);
#define DD_S8CString(s)    DD_S8((DD_u8 *)(s), DD_CalculateCStringLength(s))
#define DD_S8Lit(s)        DD_S8((DD_u8 *)(s), DD_CalculateCStringLength(s))
#define DD_ZeroString()    DD_S8(0, 0)
DD_FUNCTION DD_b32         DD_StringIsZero(DD_String8 str);
DD_FUNCTION DD_String8     DD_StringSubstring(DD_String8 str, DD_u64 min, DD_u64 max);
DD_FUNCTION DD_String8     DD_StringPrefix(DD_String8 str, DD_u64 max);
DD_FUNCTION DD_String8     DD_StringSuffix(DD_String8 str, DD_u64 min);

DD_FUNCTION DD_b32         DD_StringMatch(DD_String8 a, DD_String8 b, DD_StringMatchFlags flags);
DD_FUNCTION DD_b32         DD_StringFindSubstring(DD_String8 str, DD_String8 substring, DD_u64 occurrence, DD_StringMatchFlags flags, DD_u64 *start);
DD_FUNCTION DD_b32         DD_StringFindLastSubstring(DD_String8 str, DD_String8 substring, DD_StringMatchFlags flags, DD_u64 *start);

DD_FUNCTION DD_String8     DD_WithoutExtension(DD_String8 string);
DD_FUNCTION DD_String8     DD_WithoutFolder(DD_String8 string);
DD_FUNCTION DD_String8     DD_ExtensionString(DD_String8 string);
DD_FUNCTION DD_String8     DD_FolderString(DD_String8 string);

DD_FUNCTION char *         DD_CStringFromString8(DD_String8 string);
DD_FUNCTION DD_String8     DD_PushStringFV(char *fmt, va_list args);
DD_FUNCTION DD_String8     DD_PushStringF(char *fmt, ...);
DD_FUNCTION char *         DD_PushCStringFV(char *fmt, va_list args);
DD_FUNCTION char *         DD_PushCStringF(char *fmt, ...);
#define DD_StringExpand(s) (int)(s).size, (s).str
DD_FUNCTION void           DD_PushStringToList(DD_String8List *list, DD_String8 string);
DD_FUNCTION void           DD_PushStringListToList(DD_String8List *list, DD_String8List to_push);
DD_FUNCTION DD_String8List DD_SplitString(DD_String8 string, int split_count, DD_String8 *splits);
DD_FUNCTION DD_String8List DD_SplitStringByString(DD_String8 string, DD_String8 split);
DD_FUNCTION DD_String8List DD_SplitStringByCharacter(DD_String8 string, DD_u8 character);
DD_FUNCTION int            DD_IntFromString(DD_String8 string);
DD_FUNCTION float          DD_FloatFromString(DD_String8 string);
DD_FUNCTION DD_u64         DD_HashString(DD_String8 string);
DD_FUNCTION DD_u64         DD_CalculateCStringLength(char *cstr);

//~ String-To-Node-List Table
DD_FUNCTION DD_NodeTableSlot *DD_NodeTable_Lookup(DD_NodeTable *table, DD_String8 string);
DD_FUNCTION DD_b32            DD_NodeTable_Insert(DD_NodeTable *table, DD_NodeTableCollisionRule collision_rule, DD_String8 string, DD_Node *node);

//~ Tokenization Functions
DD_FUNCTION DD_Token     DD_TokenZero(void);
DD_FUNCTION DD_Tokenizer DD_Tokenizer_Start(DD_String8 filename, DD_String8 file_contents);
DD_FUNCTION DD_Token     DD_Tokenizer_Peek(DD_Tokenizer *tokenizer);
DD_FUNCTION DD_b32       DD_Tokenizer_PeekMatch(DD_Tokenizer *tokenizer, DD_String8 string);
DD_FUNCTION void         DD_Tokenizer_Next(DD_Tokenizer *tokenizer);
DD_FUNCTION DD_b32       DD_Tokenizer_Require(DD_Tokenizer *tokenizer, DD_String8 string);
DD_FUNCTION DD_b32       DD_Tokenizer_RequireKind(DD_Tokenizer *tokenizer, DD_TokenKind kind, DD_Token *out_token);

//~ Tree/List Building Functions
DD_FUNCTION DD_Node *DD_MakeNode(DD_NodeKind kind, DD_String8 file, DD_u64 line, DD_Token token);
DD_FUNCTION DD_Node *DD_MakeNode_Tokenizer(DD_NodeKind kind, DD_Tokenizer *tokenizer, DD_Token token);
DD_FUNCTION void     DD_PushNodeToList(DD_NodeList *list, DD_Node *parent, DD_Node *node);
DD_FUNCTION void     DD_PushListToList(DD_NodeList *list, DD_Node *parent, DD_NodeList to_push);
DD_FUNCTION void     DD_PushChild(DD_Node *parent, DD_Node *new_child);
DD_FUNCTION void     DD_PushTag(DD_Node *node, DD_Node *tag);
DD_FUNCTION void     DD_PushChildren(DD_Node *parent, DD_NodeList children);
DD_FUNCTION void     DD_PushTags(DD_Node *node, DD_NodeList tags);

//~ Parsing Functions
DD_FUNCTION DD_ParseCtx    DD_Parse_Begin(void);
DD_FUNCTION void           DD_Parse_Filename(DD_ParseCtx *ctx, DD_String8 filename);
DD_FUNCTION void           DD_Parse_String(DD_ParseCtx *ctx, DD_String8 filename, DD_String8 string);
DD_FUNCTION void           DD_Parse_Tokenizer(DD_ParseCtx *ctx, DD_Tokenizer tokenizer);
DD_FUNCTION DD_ParseResult DD_Parse_End(DD_ParseCtx *ctx);

//~ Introspection Helpers
DD_FUNCTION DD_Node *DD_TagOnNode(DD_Node *node, DD_String8 tag_string);
DD_FUNCTION DD_b32   DD_NodeHasTag(DD_Node *node, DD_String8 tag_string);
DD_FUNCTION DD_b32   DD_RequireTagArg(DD_Node *node, DD_String8 tag_string, int arg_index, DD_Node **arg_value_out);
DD_FUNCTION DD_b32   DD_RequireNodeChild(DD_Node *node, int child_index, DD_Node **child_value_out);

//~ Generation Functions
DD_FUNCTION void DD_OutputTree(FILE *file, DD_Node *node);

//~ Command Line Argument Helper Functions
DD_FUNCTION DD_CommandLine DD_CommandLine_Start(int argument_count, char **arguments);
DD_FUNCTION DD_b32         DD_CommandLine_Flag(DD_CommandLine *cmdln, DD_String8 string);
DD_FUNCTION DD_b32         DD_CommandLine_FlagStrings(DD_CommandLine *cmdln, DD_String8 string, int out_count, DD_String8 *out);
DD_FUNCTION DD_b32         DD_CommandLine_FlagIntegers(DD_CommandLine *cmdln, DD_String8 string, int out_count, DD_i64 *out);
DD_FUNCTION DD_b32         DD_CommandLine_FlagString(DD_CommandLine *cmdln, DD_String8 string, DD_String8 *out);
DD_FUNCTION DD_b32         DD_CommandLine_FlagInteger(DD_CommandLine *cmdln, DD_String8 string, DD_i64 *out);
DD_FUNCTION DD_b32         DD_CommandLine_Increment(DD_CommandLine *cmdln, DD_String8 **string_ptr);

//~ File System Functions
DD_FUNCTION DD_String8  DD_LoadEntireFile(DD_String8 filename);

//~ Functions that require OS information... will be excluded from the build
// if an OS is not specified.
#if DD_OS
DD_FUNCTION DD_b32      DD_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info);
#endif

#endif // DD_H