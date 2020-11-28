#ifndef DD_H
#define DD_H

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

//~ Basic UTF-8 string type.
typedef struct DD_String8 DD_String8;
struct DD_String8
{
    DD_u8 *str;
    DD_u64 size;
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
    DD_Node *root;
    DD_Error *first_error;
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
DD_FUNCTION DD_b32         DD_StringMatch(DD_String8 a, DD_String8 b);
DD_FUNCTION DD_b32         DD_StringMatchCaseInsensitive(DD_String8 a, DD_String8 b);
DD_FUNCTION DD_String8     DD_ExtensionString(DD_String8 string);
DD_FUNCTION char *         DD_CStringFromString8(DD_String8 string);
DD_FUNCTION DD_String8     DD_PushStringFV(char *fmt, va_list args);
DD_FUNCTION DD_String8     DD_PushStringF(char *fmt, ...);
#define DD_S8CString(s)    DD_S8((DD_u8 *)(s), strlen(s))
#define DD_S8Lit(s)        DD_S8((DD_u8 *)(s), strlen(s))
#define DD_StringExpand(s) (int)(s).size, (s).str

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

//~ Generation Functions
DD_FUNCTION void DD_OutputTree(FILE *file, DD_Node *node);

//~ File System Functions
DD_FUNCTION DD_String8  DD_LoadEntireFile(DD_String8 filename);
DD_FUNCTION DD_b32      DD_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info);

#endif // DD_H