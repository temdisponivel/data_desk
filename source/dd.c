#define DD_FUNCTION_IMPL DD_FUNCTION
#define DD_PRIVATE_FUNCTION_IMPL DD_FUNCTION_IMPL

//~ NOTE(rjf): OS Implementation Functions
DD_PRIVATE_FUNCTION_IMPL DD_b32 _DD_OS_IMPL_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info);

//~ NOTE(rjf): OS Layers

#if DD_WIN32
#include "dd_win32.c"
#endif

#if DD_POSIX
#include "dd_posix.c"
#endif

//~

DD_FUNCTION_IMPL DD_b32
DD_CharIsAlpha(DD_u8 c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

DD_FUNCTION_IMPL DD_b32
DD_CharIsDigit(DD_u8 c)
{
    return (c >= '0' && c <= '9');
}

DD_FUNCTION_IMPL DD_b32
DD_CharIsSymbol(DD_u8 c)
{
    return (c == '~' || c == '!' || c == '@' || c == '#' || c == '$' ||
            c == '%' || c == '^' || c == '&' || c == '*' || c == '(' ||
            c == ')' || c == '-' || c == '=' || c == '+' || c == '[' ||
            c == ']' || c == '{' || c == '}' || c == ':' || c == ';' ||
            c == ',' || c == '<' || c == '.' || c == '>' || c == '/' ||
            c == '?' || c == '|' || c == '\\');
}

DD_FUNCTION_IMPL DD_u8
DD_CharToUpper(DD_u8 c)
{
    return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

DD_FUNCTION_IMPL DD_u8
DD_CharToLower(DD_u8 c)
{
    return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'Z')) : c;
}

DD_FUNCTION_IMPL DD_String8
DD_S8(DD_u8 *str, DD_u64 size)
{
    DD_String8 string;
    string.str = str;
    string.size = size;
    return string;
}

DD_FUNCTION_IMPL DD_b32
DD_StringMatch(DD_String8 a, DD_String8 b)
{
    int result = 0;
    if(a.str && b.str && a.size && b.size && a.size == b.size)
    {
        result = 1;
        for(DD_u64 i = 0; i < a.size; i += 1)
        {
            if(a.str[i] != b.str[i])
            {
                result = 0;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_StringMatchCaseInsensitive(DD_String8 a, DD_String8 b)
{
    int result = 0;
    if(a.str && b.str && a.size && b.size && a.size == b.size)
    {
        result = 1;
        for(DD_u64 i = 0; i < a.size; i += 1)
        {
            if(DD_CharToUpper(a.str[i]) != DD_CharToUpper(b.str[i]))
            {
                result = 0;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION DD_String8
DD_ExtensionString(DD_String8 string)
{
    DD_String8 ext = string;
    for(DD_u64 i = 0; i < string.size; i += 1)
    {
        if(string.str[i] == '.')
        {
            ext.str = string.str + i + 1;
            ext.size = string.size - (i + 1);
        }
    }
    return ext;
}

DD_FUNCTION_IMPL char *
DD_CStringFromString8(DD_String8 string)
{
    char *result = calloc(string.size+1, 1);
    for(DD_u64 i = 0; i < string.size; i += 1)
    {
        result[i] = string.str[i];
    }
    return result;
}

DD_FUNCTION_IMPL DD_String8
DD_PushStringFV(char *fmt, va_list args)
{
    DD_String8 result = {0};
    va_list args2;
    va_copy(args2, args);
    DD_u64 needed_bytes = vsnprintf(0, 0, fmt, args)+1;
    result.str = calloc(needed_bytes, 1);
    result.size = needed_bytes - 1;
    vsnprintf(result.str, result.size, fmt, args2);
    return result;
}

DD_FUNCTION_IMPL DD_String8
DD_PushStringF(char *fmt, ...)
{
    DD_String8 result = {0};
    va_list args;
    va_start(args, fmt);
    result = DD_PushStringFV(fmt, args);
    va_end(args);
    return result;
}

DD_FUNCTION_IMPL void
DD_PushStringToList(DD_String8List *list, DD_String8 string)
{
    DD_String8Node *node = calloc(1, sizeof(*node));
    node->next = 0;
    node->string = string;
    if(list->last == 0)
    {
        list->first = list->last = node;
    }
    else
    {
        list->last->next = node;
        list->last = list->last->next;
    }
}

DD_FUNCTION_IMPL void
DD_PushStringListToList(DD_String8List *list, DD_String8List to_push)
{
    if(to_push.first)
    {
        if(list->last == 0)
        {
            *list = to_push;
        }
        else
        {
            list->last->next = to_push.first;
            list->last = to_push.last;
        }
    }
}

DD_FUNCTION_IMPL DD_String8List
DD_SplitString(DD_String8 string, int split_count, DD_String8 *splits)
{
    DD_String8List list = {0};
    
    DD_u64 split_start = 0;
    for(DD_u64 i = 0; i < string.size; i += 1)
    {
        DD_b32 was_split = 0;
        for(int split_idx = 0; split_idx < split_count; split_idx += 1)
        {
            DD_b32 match = 0;
            if(i + splits[split_idx].size <= string.size)
            {
                match = 1;
                for(DD_u64 split_i = 0; split_i < splits[split_idx].size && i + split_i < string.size; split_i += 1)
                {
                    if(splits[split_idx].str[split_i] != string.str[i + split_i])
                    {
                        match = 0;
                        break;
                    }
                }
            }
            if(match)
            {
                DD_String8 split_string = DD_S8(string.str + split_start, i - split_start);
                DD_PushStringToList(&list, split_string);
                split_start = i + splits[split_idx].size;
                i += splits[split_idx].size - 1;
                was_split = 1;
                break;
            }
        }
        
        if(was_split == 0 && i == string.size - 1)
        {
            DD_String8 split_string = DD_S8(string.str + split_start, i+1 - split_start);
            DD_PushStringToList(&list, split_string);
            break;
        }
    }
    
    return list;
}

DD_FUNCTION_IMPL DD_String8List
DD_SplitStringByString(DD_String8 string, DD_String8 split)
{
    return DD_SplitString(string, 1, &split);
}

DD_FUNCTION_IMPL DD_String8List
DD_SplitStringByCharacter(DD_String8 string, DD_u8 character)
{
    return DD_SplitStringByString(string, DD_S8(&character, 1));
}

DD_FUNCTION_IMPL int
DD_IntFromString(DD_String8 string)
{
    char *str = DD_CStringFromString8(string);
    return atoi(str);
}

DD_FUNCTION_IMPL float
DD_FloatFromString(DD_String8 string)
{
    char *str = DD_CStringFromString8(string);
    return atof(str);
}

DD_FUNCTION_IMPL DD_Token
DD_TokenZero(void)
{
    DD_Token token = {0};
    return token;
}

DD_FUNCTION_IMPL DD_Tokenizer
DD_Tokenizer_Start(DD_String8 filename, DD_String8 file_contents)
{
    DD_Tokenizer tokenizer = {0};
    tokenizer.at = file_contents.str;
    tokenizer.at_line = 1;
    tokenizer.filename = filename;
    tokenizer.file_contents = file_contents;
    return tokenizer;
}

DD_FUNCTION_IMPL DD_Token
DD_Tokenizer_Peek(DD_Tokenizer *tokenizer)
{
    DD_Token token = {0};
    DD_u8 *one_past_last = tokenizer->file_contents.str + tokenizer->file_contents.size;
    
    enum
    {
        DD_ReadMode_Normal,
        DD_ReadMode_SingleLineComment,
        DD_ReadMode_MultiLineComment,
    };
    int read_mode = DD_ReadMode_Normal;
    int multiline_comment_nest = 0;
    
    for(DD_u64 i = 0; tokenizer->at + i < one_past_last; i += 1)
    {
        
        if(read_mode == DD_ReadMode_SingleLineComment)
        {
            if(tokenizer->at[i] == '\n')
            {
                read_mode = DD_ReadMode_Normal;
            }
        }
        else if(read_mode == DD_ReadMode_MultiLineComment)
        {
            if(tokenizer->at[i] == '/' && tokenizer->at[i+1] == '*')
            {
                multiline_comment_nest += 1;
            }
            if(tokenizer->at[i] == '*' && tokenizer->at[i+1] == '/')
            {
                multiline_comment_nest -= 1;
            }
            if(multiline_comment_nest <= 0)
            {
                read_mode = DD_ReadMode_Normal;
            }
        }
        else
        {
#define DD_TokenizerLoop(name, start, cond) DD_u64 name = start; for(; ((cond)) && tokenizer->at + name < one_past_last; name += 1)
            
            // NOTE(rjf): Single-Line Comments
            if(tokenizer->at[i] == '/' && tokenizer->at[i+1] == '/')
            {
                read_mode = DD_ReadMode_SingleLineComment;
            }
            
            // NOTE(rjf): Multi-Line Comments
            else if(tokenizer->at[i] == '/' && tokenizer->at[i+1] == '*')
            {
                read_mode = DD_ReadMode_MultiLineComment;
                multiline_comment_nest = 1;
            }
            
            // NOTE(rjf): Identifier
            else if(DD_CharIsAlpha(tokenizer->at[i]))
            {
                DD_TokenizerLoop(j, i+1,
                                 DD_CharIsAlpha(tokenizer->at[j]) ||
                                 DD_CharIsDigit(tokenizer->at[j]) ||
                                 tokenizer->at[j] == '_');
                token.kind = DD_TokenKind_Identifier;
                token.string.str = tokenizer->at + i;
                token.string.size = j - i;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Numeric Literal
            else if(DD_CharIsDigit(tokenizer->at[i]) ||
                    (tokenizer->at + 1 < one_past_last &&
                     tokenizer->at[i] == '-' && DD_CharIsDigit(tokenizer->at[i+1])))
            {
                DD_TokenizerLoop(j, i+1,
                                 DD_CharIsAlpha(tokenizer->at[j]) ||
                                 DD_CharIsDigit(tokenizer->at[j]) ||
                                 tokenizer->at[j] == '.');
                token.kind = DD_TokenKind_NumericLiteral;
                token.string.str = tokenizer->at + i;
                token.string.size = j - i;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Symbol
            else if(DD_CharIsSymbol(tokenizer->at[i]))
            {
                token.kind = DD_TokenKind_Symbol;
                token.string.str = tokenizer->at + i;
                token.string.size = 1;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Multi-Line String Literal
            else if(tokenizer->at+i+2  < one_past_last &&
                    tokenizer->at[i] == '"' && tokenizer->at[i+1] == '"' && tokenizer->at[i+2] == '"')
            {
                DD_TokenizerLoop(j, i+3, !(tokenizer->at + j + 2 < one_past_last &&
                                           tokenizer->at[j] == '"' && tokenizer->at[j+1] == '"' && tokenizer->at[j+2] == '"'));
                token.kind = DD_TokenKind_StringLiteral;
                token.string.str = tokenizer->at + i + 3;
                token.string.size = j - (i + 3);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j + 3) - i;
                break;
            }
            
            // NOTE(rjf): Single-Line String Literal
            else if(tokenizer->at[i] == '"')
            {
                DD_TokenizerLoop(j, i+1, tokenizer->at[j] != '"');
                token.kind = DD_TokenKind_StringLiteral;
                token.string.str = tokenizer->at + i + 1;
                token.string.size = j - (i + 1);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j - i) + 1;
                break;
            }
            
            // NOTE(rjf): Single-Line Char Literal
            else if(tokenizer->at[i] == '\'')
            {
                DD_TokenizerLoop(j, i+1, tokenizer->at[j] != '\'');
                token.kind = DD_TokenKind_CharLiteral;
                token.string.str = tokenizer->at + i + 1;
                token.string.size = j - (i + 1);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j - i) + 1;
                break;
            }
            
            // NOTE(rjf): Multi-Line Char Literal
            else if(tokenizer->at+i+2  < one_past_last &&
                    tokenizer->at[i] == '\'' && tokenizer->at[i+1] == '\'' && tokenizer->at[i+2] == '\'')
            {
                DD_TokenizerLoop(j, i+3, !(tokenizer->at + j + 2 < one_past_last &&
                                           tokenizer->at[j] == '\'' && tokenizer->at[j+1] == '\'' && tokenizer->at[j+2] == '\''));
                token.kind = DD_TokenKind_StringLiteral;
                token.string.str = tokenizer->at + i + 3;
                token.string.size = j - (i + 3);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j + 3) - i;
                break;
            }
            
#undef DD_TokenizerLoop
        }
        
    }
    return token;
}

DD_FUNCTION_IMPL DD_b32
DD_Tokenizer_PeekMatch(DD_Tokenizer *tokenizer, DD_String8 string)
{
    int result = 0;
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(DD_StringMatch(token.string, string))
    {
        result = 1;
    }
    return result;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_Tokenizer_Bump(DD_Tokenizer *tokenizer, DD_Token token)
{
    tokenizer->at = token.outer_string.str + token.outer_string.size;
}

DD_FUNCTION_IMPL void
DD_Tokenizer_Next(DD_Tokenizer *tokenizer)
{
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(token.kind)
    {
        _DD_Tokenizer_Bump(tokenizer, token);
    }
}

DD_FUNCTION_IMPL DD_b32
DD_Tokenizer_Require(DD_Tokenizer *tokenizer, DD_String8 string)
{
    int result = 0;
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(DD_StringMatch(token.string, string))
    {
        result = 1;
        _DD_Tokenizer_Bump(tokenizer, token);
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_Tokenizer_RequireKind(DD_Tokenizer *tokenizer, DD_TokenKind kind, DD_Token *out_token)
{
    int result = 0;
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(token.kind == kind)
    {
        result = 1;
        _DD_Tokenizer_Bump(tokenizer, token);
        if(out_token)
        {
            *out_token = token;
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_MakeNode(DD_NodeKind kind, DD_String8 file, DD_u64 line, DD_Token token)
{
    DD_Node *node = calloc(1, sizeof(*node));
    node->kind = kind;
    node->string = token.string;
    node->whole_string = token.outer_string;
    node->file = file;
    node->line = line;
    return node;
}

DD_FUNCTION_IMPL DD_Node *
DD_MakeNode_Tokenizer(DD_NodeKind kind, DD_Tokenizer *tokenizer, DD_Token token)
{
    return DD_MakeNode(kind, tokenizer->filename, tokenizer->at_line, token);
}

DD_FUNCTION_IMPL void
DD_PushNodeToList(DD_NodeList *list, DD_Node *parent, DD_Node *node)
{
    if(list->last == 0)
    {
        list->first = list->last = node;
        node->next = node->prev = 0;
    }
    else
    {
        list->last->next = node;
        node->prev = list->last;
        list->last = list->last->next;
    }
    node->parent = parent;
}

DD_FUNCTION void
DD_PushListToList(DD_NodeList *list, DD_Node *parent, DD_NodeList to_push)
{
    if(to_push.first && to_push.last)
    {
        for(DD_Node *node = to_push.first; node; node = node->next)
        {
            node->parent = parent;
        }
        if(list->last == 0)
        {
            *list = to_push;
        }
        else
        {
            list->last->next = to_push.first;
            list->last = to_push.last;
        }
    }
}

DD_FUNCTION_IMPL void
DD_PushChild(DD_Node *parent, DD_Node *new_child)
{
    DD_PushNodeToList(&parent->children, parent, new_child);
}

DD_FUNCTION_IMPL void
DD_PushTag(DD_Node *node, DD_Node *tag)
{
    DD_PushNodeToList(&node->tags, node, tag);
}

DD_FUNCTION void
DD_PushChildren(DD_Node *parent, DD_NodeList children)
{
    DD_PushListToList(&parent->children, parent, children);
}

DD_FUNCTION void
DD_PushTags(DD_Node *node, DD_NodeList tags)
{
    DD_PushListToList(&node->tags, node, tags);
}

DD_FUNCTION_IMPL DD_ParseCtx
DD_Parse_Begin(void)
{
    DD_ParseCtx ctx = {0};
    return ctx;
}

DD_FUNCTION_IMPL void
DD_Parse_Filename(DD_ParseCtx *ctx, DD_String8 filename)
{
    DD_String8 file_contents = DD_LoadEntireFile(filename);
    DD_Parse_Tokenizer(ctx, DD_Tokenizer_Start(filename, file_contents));
}

DD_FUNCTION_IMPL void
DD_Parse_String(DD_ParseCtx *ctx, DD_String8 filename, DD_String8 string)
{
    DD_Parse_Tokenizer(ctx, DD_Tokenizer_Start(filename, string));
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_Error(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer, char *fmt, ...)
{
    DD_Error *error = calloc(1, sizeof(*error));
    error->filename = tokenizer->filename;
    error->line = tokenizer->at_line;
    va_list args;
    va_start(args, fmt);
    error->string = DD_PushStringFV(fmt, args);
    va_end(args);
}

typedef DD_u32 _DD_ParseSetFlags;
enum
{
    _DD_ParseSetFlag_Paren    = (1<<0),
    _DD_ParseSetFlag_Brace    = (1<<1),
    _DD_ParseSetFlag_Bracket  = (1<<2),
    _DD_ParseSetFlag_Implicit = (1<<3),
};
DD_PRIVATE_FUNCTION_IMPL DD_Node *   _DD_Parse(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer);
DD_PRIVATE_FUNCTION_IMPL DD_NodeList _DD_ParseSet(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer, DD_Node *parent, _DD_ParseSetFlags flags);
DD_PRIVATE_FUNCTION_IMPL DD_NodeList _DD_ParseTagList(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer);

DD_PRIVATE_FUNCTION_IMPL DD_Node *
_DD_Parse(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer)
{
    DD_Node *result = 0;
    DD_NodeList tags = _DD_ParseTagList(ctx, tokenizer);
    DD_Token token = {0};
    
    // NOTE(rjf): Identifier
    if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_Identifier, &token))
    {
        // NOTE(rjf): Named Set
        if(DD_Tokenizer_Require(tokenizer, DD_S8Lit(":")))
        {
            result = DD_MakeNode_Tokenizer(DD_NodeKind_Set, tokenizer, token);
            result->children = _DD_ParseSet(ctx, tokenizer, result,
                                            _DD_ParseSetFlag_Paren   |
                                            _DD_ParseSetFlag_Brace   |
                                            _DD_ParseSetFlag_Bracket |
                                            _DD_ParseSetFlag_Implicit);
            goto end_parse;
        }
        
        // NOTE(rjf): Plain identifier
        result = DD_MakeNode_Tokenizer(DD_NodeKind_Identifier, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): Unnamed Sets
    else if(DD_Tokenizer_PeekMatch(tokenizer, DD_S8Lit("(")) ||
            DD_Tokenizer_PeekMatch(tokenizer, DD_S8Lit("{")) ||
            DD_Tokenizer_PeekMatch(tokenizer, DD_S8Lit("[")))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_Set, tokenizer, DD_TokenZero());
        result->children = _DD_ParseSet(ctx, tokenizer, result,
                                        _DD_ParseSetFlag_Paren   |
                                        _DD_ParseSetFlag_Brace   |
                                        _DD_ParseSetFlag_Bracket);
        goto end_parse;
    }
    
    // NOTE(rjf): Numeric Literal
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_NumericLiteral, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_NumericLiteral, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): String Literal
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_StringLiteral, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_StringLiteral, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): Char Literal
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_CharLiteral, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_CharLiteral, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): Symbol
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_Symbol, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_Symbol, tokenizer, token);
        goto end_parse;
    }
    
    end_parse:;
    if(result)
    {
        DD_PushListToList(&result->tags, result, tags);
    }
    return result;
}

DD_PRIVATE_FUNCTION_IMPL DD_NodeList
_DD_ParseSet(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer, DD_Node *parent, _DD_ParseSetFlags flags)
{
    DD_NodeList list = {0};
    
    DD_b32 brace = 0;
    DD_b32 paren = 0;
    DD_b32 bracket = 0;
    DD_b32 terminate_with_separator = !!(flags & _DD_ParseSetFlag_Implicit);
    
    if(flags & _DD_ParseSetFlag_Brace && DD_Tokenizer_Require(tokenizer, DD_S8Lit("{")))
    {
        brace = 1;
        terminate_with_separator = 0;
    }
    else if(flags & _DD_ParseSetFlag_Paren && DD_Tokenizer_Require(tokenizer, DD_S8Lit("(")))
    {
        paren = 1;
        terminate_with_separator = 0;
    }
    else if(flags & _DD_ParseSetFlag_Bracket && DD_Tokenizer_Require(tokenizer, DD_S8Lit("[")))
    {
        bracket = 1;
        terminate_with_separator = 0;
    }
    
    // NOTE(rjf): Parse children.
    if(brace || paren || bracket || terminate_with_separator)
    {
        for(DD_u64 child_idx = 0;; child_idx += 1)
        {
            DD_Node *child = _DD_Parse(ctx, tokenizer);
            if(child)
            {
                DD_PushNodeToList(&list, parent, child);
            }
            
            // NOTE(rjf): Separators.
            {
                DD_b32 result = 0;
                result |= !!DD_Tokenizer_Require(tokenizer, DD_S8Lit(","));
                result |= !!DD_Tokenizer_Require(tokenizer, DD_S8Lit(";"));
                result |= !!DD_Tokenizer_Require(tokenizer, DD_S8Lit("->"));
                
                if(result && terminate_with_separator)
                {
                    goto end_parse;
                }
                
                // NOTE(rjf): Also use newlines as a termination separator.
                if(terminate_with_separator)
                {
                    for(DD_u64 i = 0; tokenizer->at + i < tokenizer->file_contents.str + tokenizer->file_contents.size; i += 1)
                    {
                        if(tokenizer->at[i] == '\n')
                        {
                            goto end_parse;
                        }
                        else if(tokenizer->at[i] > 32)
                        {
                            break;
                        }
                    }
                }
            }
            
            if(brace && DD_Tokenizer_Require(tokenizer, DD_S8Lit("}")))
            {
                goto end_parse;
            }
            else if(paren && DD_Tokenizer_Require(tokenizer, DD_S8Lit(")")))
            {
                goto end_parse;
            }
            else if(bracket && DD_Tokenizer_Require(tokenizer, DD_S8Lit("]")))
            {
                goto end_parse;
            }
            else if(child == 0)
            {
                if(brace)   { _DD_Error(ctx, tokenizer, "Missing }."); }
                if(paren)   { _DD_Error(ctx, tokenizer, "Missing )."); }
                if(bracket) { _DD_Error(ctx, tokenizer, "Missing ]."); }
                goto end_parse;
            }
        }
    }
    
    end_parse:;
    return list;
}

DD_PRIVATE_FUNCTION_IMPL DD_NodeList
_DD_ParseTagList(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer)
{
    DD_NodeList list = {0};
    
    for(;DD_Tokenizer_Require(tokenizer, DD_S8Lit("@"));)
    {
        DD_Token name = {0};
        if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_Identifier, &name))
        {
            DD_Node *tag = DD_MakeNode_Tokenizer(DD_NodeKind_Tag, tokenizer, name);
            tag->children = _DD_ParseSet(ctx, tokenizer, tag, _DD_ParseSetFlag_Paren);
            DD_PushNodeToList(&list, 0, tag);
        }
        else
        {
            break;
        }
    }
    
    return list;
}

DD_FUNCTION_IMPL void
DD_Parse_Tokenizer(DD_ParseCtx *ctx, DD_Tokenizer tokenizer)
{
    for(;;)
    {
        DD_Node *node = _DD_Parse(ctx, &tokenizer);
        if(node == 0)
        {
            break;
        }
        DD_PushNodeToList(&ctx->roots, 0, node);
    }
}

DD_FUNCTION_IMPL DD_ParseResult
DD_Parse_End(DD_ParseCtx *ctx)
{
    DD_ParseResult result = {0};
    result.root = ctx->roots.first;
    result.first_error = ctx->first_error;
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_TagOnNode(DD_Node *node, DD_String8 tag_string)
{
    DD_Node *result = 0;
    if(node)
    {
        for(DD_Node *tag = node->tags.first; tag; tag = tag->next)
        {
            if(DD_StringMatch(tag->string, tag_string))
            {
                result = tag;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_NodeHasTag(DD_Node *node, DD_String8 tag_string)
{
    return !!DD_TagOnNode(node, tag_string);
}

DD_FUNCTION_IMPL DD_b32
DD_RequireTagArg(DD_Node *node, DD_String8 tag_string, int arg_index, DD_Node **arg_value_out)
{
    DD_b32 result = 0;
    DD_Node *tag = DD_TagOnNode(node, tag_string);
    if(tag)
    {
        DD_Node *found_arg = 0;
        int idx = 0;
        for(DD_Node *arg = tag->children.first; arg; arg = arg->next, idx += 1)
        {
            if(idx == arg_index)
            {
                found_arg = arg;
                break;
            }
        }
        if(found_arg && arg_value_out)
        {
            *arg_value_out = found_arg;
        }
    }
    return result;
}

DD_FUNCTION_IMPL void
DD_OutputTree(FILE *file, DD_Node *node)
{
    for(DD_Node *tag = node->tags.first; tag; tag = tag->next)
    {
        fprintf(file, "@%.*s", DD_StringExpand(tag->string));
        if(tag->children.first)
        {
            fprintf(file, "(");
            for(DD_Node *child = tag->children.first; child; child = child->next)
            {
                DD_OutputTree(file, child);
                fprintf(file, ", ");
            }
            fprintf(file, ")\n");
        }
        else
        {
            fprintf(file, " ");
        }
    }
    
    fprintf(file, "%.*s", DD_StringExpand(node->whole_string));
    if(node->children.first)
    {
        fprintf(file, ":\n{\n");
        for(DD_Node *child = node->children.first; child; child = child->next)
        {
            DD_OutputTree(file, child);
            fprintf(file, ",\n");
        }
        fprintf(file, "}\n");
    }
    else
    {
        fprintf(file, " ");
    }
}

DD_FUNCTION_IMPL DD_String8
DD_LoadEntireFile(DD_String8 filename)
{
    DD_String8 file_contents = {0};
    FILE *file = fopen(DD_CStringFromString8(filename), "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        DD_u64 file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        file_contents.str = calloc(1, file_size+1);
        if(file_contents.str)
        {
            file_contents.size = file_size;
            fread(file_contents.str, 1, file_size, file);
        }
        fclose(file);
    }
    return file_contents;
}

DD_FUNCTION_IMPL DD_b32
DD_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info)
{
    return _DD_OS_IMPL_FileIter_Increment(it, path, out_info);
}