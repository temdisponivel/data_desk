#ifndef DD_H
#define DD_H

// NOTE(rjf): Compiler cracking from the 4th dimension

#if defined(_MSC_VER)

# define DD_COMPILER_CL 1

# if defined(_WIN32)
#  define DD_OS_WINDOWS 1
# else
#  error This compiler/platform combo is not supported yet
# endif

# if defined(_M_AMD64)
#  define DD_ARCH_X64 1
# elif defined(_M_IX86)
#  define DD_ARCH_X86 1
# elif defined(_M_ARM64)
#  define DD_ARCH_ARM64 1
# elif defined(_M_ARM)
#  define DD_ARCH_ARM32 1
# else
#  error architecture not supported yet
# endif

#if _MSC_VER >= 1920
#define DD_COMPILER_CL_YEAR 2019
#elif _MSC_VER >= 1910
#define DD_COMPILER_CL_YEAR 2017
#elif _MSC_VER >= 1900
#define DD_COMPILER_CL_YEAR 2015
#elif _MSC_VER >= 1800
#define DD_COMPILER_CL_YEAR 2013
#elif _MSC_VER >= 1700
#define DD_COMPILER_CL_YEAR 2012
#elif _MSC_VER >= 1600
#define DD_COMPILER_CL_YEAR 2010
#elif _MSC_VER >= 1500
#define DD_COMPILER_CL_YEAR 2008
#elif _MSC_VER >= 1400
#define DD_COMPILER_CL_YEAR 2005
#else
#define DD_COMPILER_CL_YEAR 0
#endif

#elif defined(__clang__)

# define DD_COMPILER_CLANG 1

# if defined(__APPLE__) && defined(__MACH__)
#  define DD_OS_MAC 1
# elif defined(__gnu_linux__)
#  define DD_OS_LINUX 1
# else
#  error This compiler/platform combo is not supported yet
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define DD_ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define DD_ARCH_X86 1
# elif defined(__aarch64__)
#  define DD_ARCH_ARM64 1
# elif defined(__arm__)
#  define DD_ARCH_ARM32 1
# else
#  error architecture not supported yet
# endif

#elif defined(__GNUC__) || defined(__GNUG__)

# define DD_COMPILER_GCC 1

# if defined(__gnu_linux__)
#  define DD_OS_LINUX 1
# else
#  error This compiler/platform combo is not supported yet
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define DD_ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define DD_ARCH_X86 1
# elif defined(__aarch64__)
#  define DD_ARCH_ARM64 1
# elif defined(__arm__)
#  define DD_ARCH_ARM32 1
# else
#  error architecture not supported yet
# endif

#else
# error This compiler is not supported yet
#endif

#if defined(DD_ARCH_X64)
# define DD_ARCH_64BIT 1
#elif defined(DD_ARCH_X86)
# define DD_ARCH_32BIT 1

#endif

// zeroify

#if !defined(DD_ARCH_32BIT)
#define DD_ARCH_32BIT 0
#endif
#if !defined(DD_ARCH_64BIT)
#define DD_ARCH_64BIT 0
#endif
#if !defined(DD_ARCH_X64)
#define DD_ARCH_X64 0
#endif
#if !defined(DD_ARCH_X86)
#define DD_ARCH_X86 0
#endif
#if !defined(DD_ARCH_ARM64)
#define DD_ARCH_ARM64 0
#endif
#if !defined(DD_ARCH_ARM32)
#define DD_ARCH_ARM32 0
#endif
#if !defined(DD_COMPILER_CL)
#define DD_COMPILER_CL 0
#endif
#if !defined(DD_COMPILER_GCC)
#define DD_COMPILER_GCC 0
#endif
#if !defined(DD_COMPILER_CLANG)
#define DD_COMPILER_CLANG 0
#endif
#if !defined(DD_OS_WINDOWS)
#define DD_OS_WINDOWS 0
#endif
#if !defined(DD_OS_LINUX)
#define DD_OS_LINUX 0
#endif
#if !defined(DD_OS_MAC)
#define DD_OS_MAC 0
#endif

#define DD_FUNCTION
#define DD_GLOBAL static

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

typedef struct DD_String16 DD_String16;
struct DD_String16
{
 DD_u16 *str;
 DD_u64 size;
};

typedef struct DD_String32 DD_String32;
struct DD_String32
{
 DD_u32 *str;
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
 DD_StringMatchFlag_CaseInsensitive  = (1<<0),
 DD_StringMatchFlag_RightSideSloppy  = (1<<1),
 DD_StringMatchFlag_FindLast         = (1<<2),
 DD_StringMatchFlag_SlashInsensitive = (1<<3),
};

typedef struct DD_UnicodeConsume DD_UnicodeConsume;
struct DD_UnicodeConsume{
 DD_u32 codepoint;
 DD_u32 advance;
};

//~ Node type that forms all ASTs.

typedef enum DD_NodeKind
{
 DD_NodeKind_Nil,
 DD_NodeKind_File,
 DD_NodeKind_Label,
 DD_NodeKind_UnnamedSet,
 DD_NodeKind_Tag,
 DD_NodeKind_MAX,
}
DD_NodeKind;

typedef DD_u32 DD_NodeFlags;
enum
{
 DD_NodeFlag_ParenLeft        = (1<<0),
 DD_NodeFlag_ParenRight       = (1<<1),
 DD_NodeFlag_BracketLeft      = (1<<2),
 DD_NodeFlag_BracketRight     = (1<<3),
 DD_NodeFlag_BraceLeft        = (1<<4),
 DD_NodeFlag_BraceRight       = (1<<5),
 DD_NodeFlag_BeforeSemicolon  = (1<<6),
 DD_NodeFlag_BeforeComma      = (1<<7),
 DD_NodeFlag_BeforeArrow      = (1<<8),
 DD_NodeFlag_Numeric          = (1<<9),
 DD_NodeFlag_Identifier       = (1<<10),
 DD_NodeFlag_StringLiteral    = (1<<11),
 DD_NodeFlag_CharLiteral      = (1<<12),
};

typedef struct DD_Node DD_Node;
struct DD_Node
{
 // Tree relationship pointers.
 DD_Node *next;
 DD_Node *prev;
 DD_Node *parent;
 DD_Node *first_child;
 DD_Node *last_child;
 
 // Tag list.
 DD_Node *first_tag;
 DD_Node *last_tag;
 
 // Node info.
 DD_NodeKind kind;
 DD_NodeFlags flags;
 DD_String8 string;
 DD_String8 whole_string;
 DD_u64 string_hash;
 
 // Source code location information.
 DD_String8 filename;
 DD_u8 *file_contents;
 DD_u8 *at;
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
 DD_TokenKind_Nil,
 
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
 
 DD_TokenKind_WhitespaceMin,
 DD_TokenKind_Newline,
 DD_TokenKind_WhitespaceMax,
 
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

//~ Parsing State

typedef struct DD_Error DD_Error;
struct DD_Error
{
 DD_Error *next;
 DD_String8 string;
 DD_String8 filename;
 DD_Node *node;
};

typedef struct DD_ParseCtx DD_ParseCtx;
struct DD_ParseCtx
{
 DD_Node *first_root;
 DD_Node *last_root;
 DD_Error *first_error;
 DD_Error *last_error;
 DD_u8 *at;
 DD_String8 filename;
 DD_String8 file_contents;
};

typedef struct DD_ParseResult DD_ParseResult;
struct DD_ParseResult
{
 DD_Node *node;
 DD_Error *first_error;
 DD_u64 bytes_parsed;
};

//~ Expression and Type-Expression parser helper types.

typedef enum DD_ExprKind
{
 DD_ExprKind_Nil,
 
 // NOTE(rjf): Atom
 DD_ExprKind_Atom,
 
 // NOTE(rjf): Arithmetic
 DD_ExprKind_Add,
 DD_ExprKind_Subtract,
 DD_ExprKind_Multiply,
 DD_ExprKind_Divide,
 
 // NOTE(rjf): Comparison
 DD_ExprKind_IsEqual,
 DD_ExprKind_IsNotEqual,
 DD_ExprKind_LessThan,
 DD_ExprKind_GreaterThan,
 DD_ExprKind_LessThanEqualTo,
 DD_ExprKind_GreaterThanEqualTo,
 
 // NOTE(rjf): Bools
 DD_ExprKind_BoolAnd,
 DD_ExprKind_BoolOr,
 DD_ExprKind_BoolNot,
 
 // NOTE(rjf): Bitwise
 DD_ExprKind_BitAnd,
 DD_ExprKind_BitOr,
 DD_ExprKind_BitNot,
 DD_ExprKind_BitXor,
 
 // NOTE(rjf): Unary numeric
 DD_ExprKind_Negative,
 
 // NOTE(rjf): Type
 DD_ExprKind_Pointer,
 DD_ExprKind_Array,
 
 DD_ExprKind_MAX,
}
DD_ExprKind;

typedef struct DD_Expr DD_Expr;
struct DD_Expr
{
 DD_Node *node;
 DD_ExprKind kind;
 DD_Expr *parent;
 DD_Expr *sub[2];
};

//~ Command line parsing helper types.
typedef struct DD_CommandLine DD_CommandLine;
struct DD_CommandLine
{
 // TODO(rjf): Linked-list vs. array?
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
 DD_String8 filename;
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
DD_FUNCTION DD_u8  DD_CorrectSlash(DD_u8 c);

//~ String Functions
DD_FUNCTION DD_String8     DD_S8(DD_u8 *str, DD_u64 size);
#define DD_S8CString(s)    DD_S8((DD_u8 *)(s), DD_CalculateCStringLength(s))
#define DD_S8Lit(s)        (DD_String8){(DD_u8 *)(s), sizeof(s)-1}
DD_FUNCTION DD_String8     DD_StringSubstring(DD_String8 str, DD_u64 min, DD_u64 max);
DD_FUNCTION DD_String8     DD_StringSkip(DD_String8 str, DD_u64 max);
DD_FUNCTION DD_String8     DD_StringChop(DD_String8 str, DD_u64 min);
DD_FUNCTION DD_String8     DD_StringPrefix(DD_String8 str, DD_u64 size);
DD_FUNCTION DD_String8     DD_StringSuffix(DD_String8 str, DD_u64 size);

DD_FUNCTION DD_b32         DD_StringMatch(DD_String8 a, DD_String8 b, DD_StringMatchFlags flags);
DD_FUNCTION DD_u64         DD_FindSubstring(DD_String8 str, DD_String8 substring,
                                            DD_u64 start_pos, DD_StringMatchFlags flags);
DD_FUNCTION DD_u64         DD_FindLastSubstring(DD_String8 str, DD_String8 substring, DD_StringMatchFlags flags);

DD_FUNCTION DD_String8     DD_TrimExtension(DD_String8 string);
DD_FUNCTION DD_String8     DD_TrimFolder(DD_String8 string);
DD_FUNCTION DD_String8     DD_ExtensionFromPath(DD_String8 string);
DD_FUNCTION DD_String8     DD_FolderFromPath(DD_String8 string);

DD_FUNCTION DD_String8     DD_PushStringCopy(DD_String8 string);
DD_FUNCTION DD_String8     DD_PushStringFV(char *fmt, va_list args);
DD_FUNCTION DD_String8     DD_PushStringF(char *fmt, ...);

#define DD_StringExpand(s) (int)(s).size, (s).str

DD_FUNCTION void           DD_PushStringToList(DD_String8List *list, DD_String8 string);
DD_FUNCTION void           DD_PushStringListToList(DD_String8List *list, DD_String8List *to_push);
DD_FUNCTION DD_String8List DD_SplitString(DD_String8 string, int split_count, DD_String8 *splits);
DD_FUNCTION DD_String8List DD_SplitStringByString(DD_String8 string, DD_String8 split);
DD_FUNCTION DD_String8List DD_SplitStringByCharacter(DD_String8 string, DD_u8 character);
// TODO(rjf): Radix
DD_FUNCTION int            DD_IntFromString(DD_String8 string);
DD_FUNCTION float          DD_FloatFromString(DD_String8 string);
DD_FUNCTION DD_u64         DD_HashString(DD_String8 string);
DD_FUNCTION DD_u64         DD_CalculateCStringLength(char *cstr);

//~ Unicode Conversions
DD_FUNCTION DD_UnicodeConsume DD_CodepointFromUtf8(DD_u8 *str, DD_u64 max);
DD_FUNCTION DD_UnicodeConsume DD_CodepointFromUtf16(DD_u16 *str, DD_u64 max);

DD_FUNCTION DD_u32 DD_Utf8FromCodepoint(DD_u8 *out, DD_u32 codepoint);
DD_FUNCTION DD_u32 DD_Utf16FromCodepoint(DD_u16 *out, DD_u32 codepoint);

DD_FUNCTION DD_String8     DD_S8FromS16(DD_String16 str);
DD_FUNCTION DD_String16    DD_S16FromS8(DD_String8 str);

DD_FUNCTION DD_String8     DD_S8FromS32(DD_String32 str);
DD_FUNCTION DD_String32    DD_S32FromS8(DD_String8 str);

//~ String-To-Node-List Table
DD_FUNCTION DD_NodeTableSlot *DD_NodeTable_Lookup(DD_NodeTable *table, DD_String8 string);
DD_FUNCTION DD_b32            DD_NodeTable_Insert(DD_NodeTable *table, DD_NodeTableCollisionRule collision_rule, DD_String8 string, DD_Node *node);

//~ Parsing Functions
DD_FUNCTION DD_Token       DD_ZeroToken(void);
DD_FUNCTION DD_b32         DD_TokenKindIsWhitespace(DD_TokenKind kind);
DD_FUNCTION DD_ParseCtx    DD_Parse_InitializeCtx(DD_String8 filename, DD_String8 contents);
DD_FUNCTION DD_Token       DD_Parse_PeekAll(DD_ParseCtx *ctx);
DD_FUNCTION DD_Token       DD_Parse_PeekNonWhitespace(DD_ParseCtx *ctx);
DD_FUNCTION DD_b32         DD_Parse_PeekMatch(DD_ParseCtx *ctx, DD_String8 string);
DD_FUNCTION DD_b32         DD_Parse_TokenMatch(DD_Token token, DD_String8 string, DD_StringMatchFlags flags);
DD_FUNCTION DD_b32         DD_Parse_Require(DD_ParseCtx *ctx, DD_String8 string);
DD_FUNCTION DD_b32         DD_Parse_RequireKind(DD_ParseCtx *ctx, DD_TokenKind kind, DD_Token *out_token);
DD_FUNCTION DD_ParseResult DD_ParseOneNode     (DD_String8 filename, DD_String8 contents);
DD_FUNCTION DD_Node *      DD_ParseWholeString (DD_String8 filename, DD_String8 contents);
DD_FUNCTION DD_Node *      DD_ParseWholeFile   (DD_String8 filename);

//~ Tree/List Building Functions
DD_FUNCTION DD_b32   DD_NodeIsNil(DD_Node *node);
DD_FUNCTION DD_Node *DD_NilNode(void);
DD_FUNCTION DD_Node *DD_MakeNodeFromToken(DD_NodeKind kind, DD_String8 filename, DD_u8 *file, DD_u8 *at, DD_Token token);
DD_FUNCTION DD_Node *DD_MakeNodeFromString(DD_NodeKind kind, DD_String8 filename, DD_u8 *file, DD_u8 *at, DD_String8 string);
DD_FUNCTION void     DD_PushSibling(DD_Node **first, DD_Node **last, DD_Node *parent, DD_Node *new_sibling);
DD_FUNCTION void     DD_PushChild(DD_Node *parent, DD_Node *new_child);
DD_FUNCTION void     DD_PushTag(DD_Node *node, DD_Node *tag);

//~ Introspection Helpers
#define DD_EachNode(it, first) DD_Node *it = (first); !DD_NodeIsNil(it); it = it->next
DD_FUNCTION DD_Node *DD_NodeFromString(DD_Node *first, DD_Node *last, DD_String8 string);
DD_FUNCTION DD_Node *DD_NodeFromIndex(DD_Node *first, DD_Node *last, int n);
DD_FUNCTION int      DD_IndexFromNode(DD_Node *node);
DD_FUNCTION DD_Node *DD_NextNodeSibling(DD_Node *last, DD_String8 string);
DD_FUNCTION DD_Node *DD_ChildFromString(DD_Node *node, DD_String8 child_string);
DD_FUNCTION DD_Node *DD_TagFromString(DD_Node *node, DD_String8 tag_string);
DD_FUNCTION DD_Node *DD_ChildFromIndex(DD_Node *node, int n);
DD_FUNCTION DD_Node *DD_TagFromIndex(DD_Node *node, int n);
DD_FUNCTION DD_Node *DD_TagArgFromIndex(DD_Node *node, DD_String8 tag_string, int n);
DD_FUNCTION DD_b32   DD_NodeHasTag(DD_Node *node, DD_String8 tag_string);

//~ Expression and Type-Expression Helper Functions
DD_FUNCTION DD_Expr *DD_NilExpr(void);
DD_FUNCTION DD_Expr *DD_MakeExpr(DD_Node *node, DD_ExprKind kind, DD_Expr *left, DD_Expr *right);
DD_FUNCTION DD_Expr *DD_ParseAsExpr(DD_Node *node);

//~ Generation Functions
DD_FUNCTION void DD_OutputTree(FILE *file, DD_Node *node);
DD_FUNCTION void DD_OutputExpr(FILE *file, DD_Expr *expr);
DD_FUNCTION void DD_OutputTree_C_String(FILE *file, DD_Node *node);
DD_FUNCTION void DD_OutputTree_C_Struct(FILE *file, DD_Node *node);
DD_FUNCTION void DD_OutputTree_C_Decl(FILE *file, DD_Node *node);
DD_FUNCTION void DD_OutputType_C_LHS(FILE *file, DD_Expr *type);
DD_FUNCTION void DD_OutputType_C_RHS(FILE *file, DD_Expr *type);

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
// TODO(rjf): Decouple
DD_FUNCTION DD_b32      DD_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info);

#endif // DD_H