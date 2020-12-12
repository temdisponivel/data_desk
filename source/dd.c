#define DD_FUNCTION_IMPL DD_FUNCTION
#define DD_PRIVATE_FUNCTION_IMPL DD_FUNCTION_IMPL

//~ NOTE(rjf): OS Layers

DD_PRIVATE_FUNCTION_IMPL DD_b32 _DD_OS_IMPL_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info);
#if DD_OS_WINDOWS
#include "dd_win32.c"
#endif
#if DD_OS_LINUX
#include "dd_posix.c"
#endif

//~

static DD_Node _dd_nil_node =
{
 .next          = &_dd_nil_node,
 .prev          = &_dd_nil_node,
 .parent        = &_dd_nil_node,
 .first_child   = &_dd_nil_node,
 .last_child    = &_dd_nil_node,
 .first_tag     = &_dd_nil_node,
 .last_tag      = &_dd_nil_node,
 .kind          = DD_NodeKind_Nil,
 .string        = {0},
 .whole_string  = {0},
 .string_hash   = 0xdeadffffffffffull,
 .filename      = {"`NIL DD NODE`", 13},
 .file_contents = 0,
 .at            = 0,
};

DD_PRIVATE_FUNCTION_IMPL void
_DD_MemoryZero(void *memory, DD_u64 size)
{
 memset(memory, 0, size);
}

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
 return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}

DD_FUNCTION_IMPL DD_u8
DD_CorrectSlash(DD_u8 c)
{
 return (c == '\\' ? '/' : c);
}

DD_FUNCTION_IMPL DD_String8
DD_S8(DD_u8 *str, DD_u64 size)
{
 DD_String8 string;
 string.str = str;
 string.size = size;
 return string;
}

DD_FUNCTION_IMPL DD_String8
DD_StringSubstring(DD_String8 str, DD_u64 min, DD_u64 max)
{
 if(max > str.size)
 {
  max = str.size;
 }
 if(min > str.size)
 {
  min = str.size;
 }
 if(min > max)
 {
  DD_u64 swap = min;
  min = max;
  max = swap;
 }
 str.size = max - min;
 str.str += min;
 return str;
}

DD_FUNCTION_IMPL DD_String8
DD_StringSkip(DD_String8 str, DD_u64 max)
{
 return DD_StringSubstring(str, 0, max);
}

DD_FUNCTION_IMPL DD_String8
DD_StringChop(DD_String8 str, DD_u64 min)
{
 return DD_StringSubstring(str, min, str.size);
}

DD_FUNCTION_IMPL DD_String8
DD_StringPrefix(DD_String8 str, DD_u64 size)
{
 return DD_StringSubstring(str, 0, size);
}

DD_FUNCTION_IMPL DD_String8
DD_StringSuffix(DD_String8 str, DD_u64 size)
{
 return DD_StringSubstring(str, str.size - size, size);
}

DD_FUNCTION_IMPL DD_b32
DD_StringMatch(DD_String8 a, DD_String8 b, DD_StringMatchFlags flags)
{
 int result = 0;
 if(a.size == b.size || flags & DD_StringMatchFlag_RightSideSloppy)
 {
  result = 1;
  for(DD_u64 i = 0; i < a.size; i += 1)
  {
   DD_b32 match = (a.str[i] == b.str[i]);
   if(flags & DD_StringMatchFlag_CaseInsensitive)
   {
    match |= (DD_CharToLower(a.str[i]) == DD_CharToLower(b.str[i]));
   }
   if(flags & DD_StringMatchFlag_SlashInsensitive)
   {
    match |= (DD_CorrectSlash(a.str[i]) == DD_CorrectSlash(b.str[i]));
   }
   if(match == 0)
   {
    result = 0;
    break;
   }
  }
 }
 return result;
}

DD_FUNCTION_IMPL DD_u64
DD_FindSubstring(DD_String8 str, DD_String8 substring, DD_u64 start_pos, DD_StringMatchFlags flags)
{
 DD_b32 found = 0;
 DD_u64 found_idx = str.size;
 for(DD_u64 i = start_pos; i < str.size; i += 1)
 {
  if(i + substring.size <= str.size)
  {
   DD_String8 substr_from_str = DD_StringSubstring(str, i, i+substring.size);
   if(DD_StringMatch(substr_from_str, substring, flags))
   {
    found_idx = i;
    found = 1;
    if(!(flags & DD_StringMatchFlag_FindLast))
    {
     break;
    }
   }
  }
 }
 return found_idx;
}

DD_FUNCTION_IMPL DD_u64
DD_FindLastSubstring(DD_String8 str, DD_String8 substring, DD_StringMatchFlags flags)
{
 return DD_FindSubstring(str, substring, 0, flags | DD_StringMatchFlag_FindLast);
}

DD_FUNCTION_IMPL DD_String8
DD_WithoutExtension(DD_String8 string)
{
 DD_u64 period_pos = DD_FindLastSubstring(string, DD_S8Lit("."), 0);
 if(period_pos < string.size)
 {
  string.size = period_pos;
 }
 return string;
}

DD_FUNCTION_IMPL DD_String8
DD_WithoutFolder(DD_String8 string)
{
 DD_u64 slash_pos = DD_FindLastSubstring(string, DD_S8Lit("/"), DD_StringMatchFlag_SlashInsensitive);
 if(slash_pos < string.size)
 {
  string.str += slash_pos+1;
  string.size -= slash_pos+1;
 }
 return string;
}

DD_FUNCTION_IMPL DD_String8
DD_ExtensionString(DD_String8 string)
{
 DD_u64 period_pos = DD_FindLastSubstring(string, DD_S8Lit("."), 0);
 if(period_pos < string.size)
 {
  string.str += period_pos+1;
  string.size -= period_pos+1;
 }
 return string;
}

DD_FUNCTION_IMPL DD_String8
DD_FolderString(DD_String8 string)
{
 DD_u64 slash_pos = DD_FindLastSubstring(string, DD_S8Lit("/"), DD_StringMatchFlag_SlashInsensitive);
 if(slash_pos < string.size)
 {
  string.size = slash_pos;
 }
 return string;
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
 result.size = needed_bytes-1;
 vsnprintf(result.str, needed_bytes, fmt, args2);
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

DD_FUNCTION_IMPL char *
DD_PushCStringFV(char *fmt, va_list args)
{
 char *result = 0;
 va_list args2;
 va_copy(args2, args);
 DD_u64 needed_bytes = vsnprintf(0, 0, fmt, args)+1;
 result = calloc(needed_bytes, 1);
 vsnprintf(result, needed_bytes, fmt, args2);
 return result;
}

DD_FUNCTION_IMPL char *
DD_PushCStringF(char *fmt, ...)
{
 char *result = 0;
 va_list args;
 va_start(args, fmt);
 result = DD_PushCStringFV(fmt, args);
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
DD_PushStringListToList(DD_String8List *list, DD_String8List *to_push)
{
 if(to_push->first)
 {
  if(list->last == 0)
  {
   *list = *to_push;
  }
  else
  {
   list->last->next = to_push->first;
   list->last = to_push->last;
  }
 }
 _DD_MemoryZero(to_push, sizeof(*to_push));
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

DD_FUNCTION_IMPL DD_u64
DD_HashString(DD_String8 string)
{
 DD_u64 result = 5381;
 for(DD_u64 i = 0; i < string.size; i += 1)
 {
  result = ((result << 5) + result) + string.str[i];
 }
 return result;
}

DD_FUNCTION_IMPL DD_u64
DD_CalculateCStringLength(char *cstr)
{
 DD_u64 i = 0;
 for(; cstr[i]; i += 1);
 return i;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_NodeTable_Initialize(DD_NodeTable *table)
{
 if(table->table_size == 0)
 {
  table->table_size = 4096;
  table->table = calloc(table->table_size, sizeof(DD_NodeTableSlot *));
 }
}

DD_FUNCTION_IMPL DD_NodeTableSlot *
DD_NodeTable_Lookup(DD_NodeTable *table, DD_String8 string)
{
 _DD_NodeTable_Initialize(table);
 
 DD_NodeTableSlot *slot = 0;
 DD_u64 hash = DD_HashString(string);
 DD_u64 index = hash % table->table_size;
 for(DD_NodeTableSlot *candidate = table->table[index]; candidate; candidate = candidate->next)
 {
  if(candidate->hash == hash)
  {
   slot = candidate;
   break;
  }
 }
 return slot;
}

DD_FUNCTION_IMPL DD_b32
DD_NodeTable_Insert(DD_NodeTable *table, DD_NodeTableCollisionRule collision_rule, DD_String8 string, DD_Node *node)
{
 _DD_NodeTable_Initialize(table);
 
 DD_NodeTableSlot *slot = 0;
 DD_u64 hash = DD_HashString(string);
 DD_u64 index = hash % table->table_size;
 
 for(DD_NodeTableSlot *candidate = table->table[index]; candidate; candidate = candidate->next)
 {
  if(candidate->hash == hash)
  {
   slot = candidate;
   break;
  }
 }
 
 if(slot == 0 || (slot != 0 && collision_rule == DD_NodeTableCollisionRule_Chain))
 {
  slot = calloc(1, sizeof(*slot));
  if(slot)
  {
   slot->next = table->table[index];
   table->table[index] = slot;
  }
 }
 
 if(slot)
 {
  slot->node = node;
  slot->hash = hash;
 }
 
 return !!slot;
}

DD_FUNCTION_IMPL DD_Token
DD_ZeroToken(void)
{
 DD_Token token = {0};
 return token;
}

DD_FUNCTION_IMPL DD_b32
DD_TokenKindIsWhitespace(DD_TokenKind kind)
{
 return kind > DD_TokenKind_WhitespaceMin && kind < DD_TokenKind_WhitespaceMax;
}

DD_FUNCTION_IMPL DD_ParseCtx
DD_Parse_InitializeCtx(DD_String8 filename, DD_String8 contents)
{
 DD_ParseCtx ctx = {0};
 ctx.first_root = ctx.last_root = DD_NilNode();
 ctx.at = contents.str;
 ctx.file_contents = contents;
 ctx.filename = filename;
 return ctx;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_PushNodeToList(DD_Node **firstp, DD_Node **lastp, DD_Node *parent, DD_Node *node)
{
 if(!DD_NodeIsNil(node))
 {
  DD_Node *first = *firstp;
  DD_Node *last = *lastp;
  if(DD_NodeIsNil(last))
  {
   first = last = node;
   node->next = node->prev = DD_NilNode();
  }
  else
  {
   last->next = node;
   node->next = DD_NilNode();
   node->prev = last;
   last = last->next;
  }
  *firstp = first;
  *lastp = last;
  node->parent = parent;
 }
}

DD_FUNCTION_IMPL DD_Token
DD_Parse_PeekAll(DD_ParseCtx *ctx)
{
 DD_Token token = {0};
 DD_u8 *one_past_last = ctx->file_contents.str + ctx->file_contents.size;
 
 enum
 {
  DD_ReadMode_Normal,
  DD_ReadMode_SingleLineComment,
  DD_ReadMode_MultiLineComment,
 };
 int read_mode = DD_ReadMode_Normal;
 int multiline_comment_nest = 0;
 
 for(DD_u64 i = 0; ctx->at + i < one_past_last; i += 1)
 {
  
  if(read_mode == DD_ReadMode_SingleLineComment)
  {
   if(ctx->at[i] == '\n')
   {
    read_mode = DD_ReadMode_Normal;
   }
  }
  else if(read_mode == DD_ReadMode_MultiLineComment)
  {
   if(ctx->at[i] == '/' && ctx->at[i+1] == '*')
   {
    multiline_comment_nest += 1;
   }
   if(ctx->at[i] == '*' && ctx->at[i+1] == '/')
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
#define DD_TokenizerLoop(name, start, cond) DD_u64 name = start; for(; ((cond)) && ctx->at + name < one_past_last; name += 1)
   
   // NOTE(rjf): Single-Line Comments
   if(ctx->at[i] == '/' && ctx->at[i+1] == '/')
   {
    read_mode = DD_ReadMode_SingleLineComment;
   }
   
   // NOTE(rjf): Multi-Line Comments
   else if(ctx->at[i] == '/' && ctx->at[i+1] == '*')
   {
    read_mode = DD_ReadMode_MultiLineComment;
    multiline_comment_nest = 1;
   }
   
   // NOTE(rjf): Newlines
   else if(ctx->at[i] == '\n')
   {
    token.kind = DD_TokenKind_Newline;
    token.string.str = ctx->at + i;
    token.string.size = 1;
    token.outer_string = token.string;
    break;
   }
   
   // NOTE(rjf): Identifier
   else if(DD_CharIsAlpha(ctx->at[i]))
   {
    DD_TokenizerLoop(j, i+1,
                     DD_CharIsAlpha(ctx->at[j]) ||
                     DD_CharIsDigit(ctx->at[j]) ||
                     ctx->at[j] == '_');
    token.kind = DD_TokenKind_Identifier;
    token.string.str = ctx->at + i;
    token.string.size = j - i;
    token.outer_string = token.string;
    break;
   }
   
   // NOTE(rjf): Numeric Literal
   else if(DD_CharIsDigit(ctx->at[i]) ||
           (ctx->at + 1 < one_past_last &&
            ctx->at[i] == '-' && DD_CharIsDigit(ctx->at[i+1])))
   {
    DD_TokenizerLoop(j, i+1,
                     DD_CharIsAlpha(ctx->at[j]) ||
                     DD_CharIsDigit(ctx->at[j]) ||
                     ctx->at[j] == '.');
    token.kind = DD_TokenKind_NumericLiteral;
    token.string.str = ctx->at + i;
    token.string.size = j - i;
    token.outer_string = token.string;
    break;
   }
   
   // NOTE(rjf): Symbol
   else if(DD_CharIsSymbol(ctx->at[i]))
   {
    token.kind = DD_TokenKind_Symbol;
    token.string.str = ctx->at + i;
    token.string.size = 1;
    token.outer_string = token.string;
    break;
   }
   
   // NOTE(rjf): Multi-Line String Literal
   else if(ctx->at+i+2  < one_past_last &&
           ctx->at[i] == '"' && ctx->at[i+1] == '"' && ctx->at[i+2] == '"')
   {
    DD_TokenizerLoop(j, i+3, !(ctx->at + j + 2 < one_past_last &&
                               ctx->at[j] == '"' && ctx->at[j+1] == '"' && ctx->at[j+2] == '"'));
    token.kind = DD_TokenKind_StringLiteral;
    token.string.str = ctx->at + i + 3;
    token.string.size = j - (i + 3);
    token.outer_string.str = ctx->at + i;
    token.outer_string.size = (j + 3) - i;
    break;
   }
   
   // NOTE(rjf): Single-Line String Literal
   else if(ctx->at[i] == '"')
   {
    DD_TokenizerLoop(j, i+1, ctx->at[j] != '"');
    token.kind = DD_TokenKind_StringLiteral;
    token.string.str = ctx->at + i + 1;
    token.string.size = j - (i + 1);
    token.outer_string.str = ctx->at + i;
    token.outer_string.size = (j - i) + 1;
    break;
   }
   
   // NOTE(rjf): Single-Line Char Literal
   else if(ctx->at[i] == '\'')
   {
    DD_TokenizerLoop(j, i+1, ctx->at[j] != '\'');
    token.kind = DD_TokenKind_CharLiteral;
    token.string.str = ctx->at + i + 1;
    token.string.size = j - (i + 1);
    token.outer_string.str = ctx->at + i;
    token.outer_string.size = (j - i) + 1;
    break;
   }
   
   // NOTE(rjf): Multi-Line Char Literal
   else if(ctx->at+i+2  < one_past_last &&
           ctx->at[i] == '\'' && ctx->at[i+1] == '\'' && ctx->at[i+2] == '\'')
   {
    DD_TokenizerLoop(j, i+3, !(ctx->at + j + 2 < one_past_last &&
                               ctx->at[j] == '\'' && ctx->at[j+1] == '\'' && ctx->at[j+2] == '\''));
    token.kind = DD_TokenKind_StringLiteral;
    token.string.str = ctx->at + i + 3;
    token.string.size = j - (i + 3);
    token.outer_string.str = ctx->at + i;
    token.outer_string.size = (j + 3) - i;
    break;
   }
   
#undef DD_TokenizerLoop
  }
  
 }
 return token;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_Parse_Bump(DD_ParseCtx *ctx, DD_Token token)
{
 ctx->at = token.outer_string.str + token.outer_string.size;
}

DD_FUNCTION_IMPL DD_Token
DD_Parse_PeekNonWhitespace(DD_ParseCtx *ctx)
{
 DD_Token result = {0};
 DD_ParseCtx ctx_restore = *ctx;
 
 for(DD_Token token = DD_Parse_PeekAll(ctx);; token = DD_Parse_PeekAll(ctx))
 {
  if(!DD_TokenKindIsWhitespace(token.kind))
  {
   result = token;
   break;
  }
  else if(token.kind == DD_TokenKind_Nil)
  {
   break;
  }
  _DD_Parse_Bump(ctx, token);
 }
 
 *ctx = ctx_restore;
 return result;
}

DD_FUNCTION_IMPL DD_b32
DD_Parse_TokenMatch(DD_Token token, DD_String8 string, DD_StringMatchFlags flags)
{
 return DD_StringMatch(token.string, string, flags);
}

DD_FUNCTION_IMPL DD_b32
DD_Parse_Require(DD_ParseCtx *ctx, DD_String8 string)
{
 int result = 0;
 
 DD_Token token_all = DD_Parse_PeekAll(ctx);
 if(DD_StringMatch(token_all.string, string, 0))
 {
  result = 1;
  _DD_Parse_Bump(ctx, token_all);
  goto end;
 }
 
 DD_Token token_nonws = DD_Parse_PeekNonWhitespace(ctx);
 if(DD_StringMatch(token_nonws.string, string, 0))
 {
  result = 1;
  _DD_Parse_Bump(ctx, token_nonws);
  goto end;
 }
 
 end:;
 return result;
}

DD_FUNCTION_IMPL DD_b32
DD_Parse_RequireKind(DD_ParseCtx *ctx, DD_TokenKind kind, DD_Token *out_token)
{
 int result = 0;
 DD_Token token = DD_TokenKindIsWhitespace(kind) ? DD_Parse_PeekAll(ctx) : DD_Parse_PeekNonWhitespace(ctx);
 if(token.kind == kind)
 {
  result = 1;
  _DD_Parse_Bump(ctx, token);
  if(out_token)
  {
   *out_token = token;
  }
 }
 return result;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_Error(DD_ParseCtx *ctx, char *fmt, ...)
{
 DD_Error *error = calloc(1, sizeof(*error));
 error->filename = ctx->filename;
 va_list args;
 va_start(args, fmt);
 error->string = DD_PushStringFV(fmt, args);
 va_end(args);
}

DD_PRIVATE_FUNCTION_IMPL DD_Node *
_DD_MakeNode(DD_NodeKind kind, DD_String8 string, DD_String8 whole_string, DD_String8 filename,
             DD_u8 *file_contents, DD_u8 *at)
{
 DD_Node *node = calloc(1, sizeof(*node));
 if(node)
 {
  node->kind = kind;
  node->string = string;
  node->whole_string = whole_string;
  node->next = node->prev = node->parent = node->first_child = node->last_child = node->first_tag = node->last_tag = DD_NilNode();
  node->filename = filename;
  node->file_contents = file_contents;
  node->at = at;
 }
 else
 {
  node = DD_NilNode();
 }
 return node;
}

DD_PRIVATE_FUNCTION_IMPL DD_Node *
_DD_MakeNodeFromToken_Ctx(DD_ParseCtx *ctx, DD_NodeKind kind, DD_Token token)
{
 return _DD_MakeNode(kind, token.string, token.outer_string, ctx->filename, ctx->file_contents.str, ctx->at);
}

DD_PRIVATE_FUNCTION_IMPL DD_Node *
_DD_MakeNodeFromString_Ctx(DD_ParseCtx *ctx, DD_NodeKind kind, DD_String8 string)
{
 return _DD_MakeNode(kind, string, string, ctx->filename, ctx->file_contents.str, ctx->at);
}

typedef DD_u32 _DD_ParseSetFlags;
enum
{
 _DD_ParseSetFlag_Paren    = (1<<0),
 _DD_ParseSetFlag_Brace    = (1<<1),
 _DD_ParseSetFlag_Bracket  = (1<<2),
 _DD_ParseSetFlag_Implicit = (1<<3),
};

DD_PRIVATE_FUNCTION_IMPL DD_ParseResult _DD_ParseOneNode(DD_ParseCtx *ctx);
DD_PRIVATE_FUNCTION_IMPL void _DD_ParseSet(DD_ParseCtx *ctx, DD_Node *parent, _DD_ParseSetFlags flags, DD_Node **first_out, DD_Node **last_out);
DD_PRIVATE_FUNCTION_IMPL void _DD_ParseTagList(DD_ParseCtx *ctx, DD_Node **first_out, DD_Node **last_out);

DD_PRIVATE_FUNCTION_IMPL DD_ParseResult
_DD_ParseOneNode(DD_ParseCtx *ctx)
{
 DD_u8 *at_first = ctx->at;
 
 DD_ParseResult result = {0};
 result.node = DD_NilNode();
 
 DD_Token token = {0};
 
 DD_Node *first_tag = 0;
 DD_Node *last_tag = 0;
 _DD_ParseTagList(ctx, &first_tag, &last_tag);
 
 // NOTE(rjf): Identifier
 if(DD_Parse_RequireKind(ctx, DD_TokenKind_Identifier, &token))
 {
  // NOTE(rjf): Named Set
  if(DD_Parse_Require(ctx, DD_S8Lit(":")))
  {
   result.node = DD_MakeNodeFromToken(DD_NodeKind_Set, ctx->filename, ctx->file_contents.str, ctx->at, token);
   _DD_ParseSet(ctx, result.node,
                _DD_ParseSetFlag_Paren   |
                _DD_ParseSetFlag_Brace   |
                _DD_ParseSetFlag_Bracket |
                _DD_ParseSetFlag_Implicit,
                &result.node->first_child,
                &result.node->last_child);
   goto end_parse;
  }
  
  // NOTE(rjf): Plain identifier
  result.node = _DD_MakeNodeFromToken_Ctx(ctx, DD_NodeKind_Identifier, token);
  goto end_parse;
 }
 
 // NOTE(rjf): Unnamed Sets
 else if(DD_Parse_TokenMatch(DD_Parse_PeekNonWhitespace(ctx), DD_S8Lit("("), 0) ||
         DD_Parse_TokenMatch(DD_Parse_PeekNonWhitespace(ctx), DD_S8Lit("{"), 0) ||
         DD_Parse_TokenMatch(DD_Parse_PeekNonWhitespace(ctx), DD_S8Lit("["), 0))
 {
  result.node = _DD_MakeNodeFromString_Ctx(ctx, DD_NodeKind_Set, DD_S8Lit(""));
  _DD_ParseSet(ctx, result.node,
               _DD_ParseSetFlag_Paren   |
               _DD_ParseSetFlag_Brace   |
               _DD_ParseSetFlag_Bracket,
               &result.node->first_child,
               &result.node->last_child);
  goto end_parse;
 }
 
 // NOTE(rjf): Numeric Literal
 else if(DD_Parse_RequireKind(ctx, DD_TokenKind_NumericLiteral, &token))
 {
  result.node = _DD_MakeNodeFromToken_Ctx(ctx, DD_NodeKind_NumericLiteral, token);
  goto end_parse;
 }
 
 // NOTE(rjf): String Literal
 else if(DD_Parse_RequireKind(ctx, DD_TokenKind_StringLiteral, &token))
 {
  result.node = _DD_MakeNodeFromToken_Ctx(ctx, DD_NodeKind_StringLiteral, token);
  goto end_parse;
 }
 
 // NOTE(rjf): Char Literal
 else if(DD_Parse_RequireKind(ctx, DD_TokenKind_CharLiteral, &token))
 {
  result.node = _DD_MakeNodeFromToken_Ctx(ctx, DD_NodeKind_CharLiteral, token);
  goto end_parse;
 }
 
 // NOTE(rjf): Symbol
 else if(DD_Parse_RequireKind(ctx, DD_TokenKind_Symbol, &token))
 {
  result.node = _DD_MakeNodeFromToken_Ctx(ctx, DD_NodeKind_Symbol, token);
  goto end_parse;
 }
 
 end_parse:;
 if(!DD_NodeIsNil(result.node))
 {
  result.bytes_parsed = (DD_u64)(ctx->at - at_first);
  result.node->first_tag = first_tag;
  result.node->last_tag = last_tag;
  for(DD_Node *tag = first_tag; !DD_NodeIsNil(tag); tag = tag->next)
  {
   tag->parent = result.node;
  }
 }
 return result;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_ParseSet(DD_ParseCtx *ctx, DD_Node *parent, _DD_ParseSetFlags flags,
             DD_Node **first_out, DD_Node **last_out)
{
 DD_Node *first = DD_NilNode();
 DD_Node *last = DD_NilNode();
 
 DD_b32 brace = 0;
 DD_b32 paren = 0;
 DD_b32 bracket = 0;
 DD_b32 terminate_with_separator = !!(flags & _DD_ParseSetFlag_Implicit);
 
 if(flags & _DD_ParseSetFlag_Brace && DD_Parse_Require(ctx, DD_S8Lit("{")))
 {
  brace = 1;
  terminate_with_separator = 0;
 }
 else if(flags & _DD_ParseSetFlag_Paren && DD_Parse_Require(ctx, DD_S8Lit("(")))
 {
  paren = 1;
  terminate_with_separator = 0;
 }
 else if(flags & _DD_ParseSetFlag_Bracket && DD_Parse_Require(ctx, DD_S8Lit("[")))
 {
  bracket = 1;
  terminate_with_separator = 0;
 }
 
 // NOTE(rjf): Parse children.
 if(brace || paren || bracket || terminate_with_separator)
 {
  for(DD_u64 child_idx = 0;; child_idx += 1)
  {
   DD_ParseResult parse = _DD_ParseOneNode(ctx);
   DD_Node *child = parse.node;
   if(!DD_NodeIsNil(child))
   {
    _DD_PushNodeToList(&first, &last, parent, child);
   }
   
   // NOTE(rjf): Separators.
   {
    DD_b32 result = 0;
    result |= !!DD_Parse_Require(ctx, DD_S8Lit(","));
    result |= !!DD_Parse_Require(ctx, DD_S8Lit(";"));
    result |= !!DD_Parse_Require(ctx, DD_S8Lit("->"));
    result |= !!DD_Parse_Require(ctx, DD_S8Lit("\n"));
    if(result && terminate_with_separator)
    {
     goto end_parse;
    }
   }
   
   if(brace && DD_Parse_Require(ctx, DD_S8Lit("}")))
   {
    goto end_parse;
   }
   else if(paren && DD_Parse_Require(ctx, DD_S8Lit(")")))
   {
    goto end_parse;
   }
   else if(bracket && DD_Parse_Require(ctx, DD_S8Lit("]")))
   {
    goto end_parse;
   }
   else if(child == 0)
   {
    if(brace)   { _DD_Error(ctx, "Missing }."); }
    if(paren)   { _DD_Error(ctx, "Missing )."); }
    if(bracket) { _DD_Error(ctx, "Missing ]."); }
    goto end_parse;
   }
  }
 }
 
 end_parse:;
 *first_out = first;
 *last_out = last;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_ParseTagList(DD_ParseCtx *ctx, DD_Node **first_out, DD_Node **last_out)
{
 DD_Node *first = DD_NilNode();
 DD_Node *last = DD_NilNode();
 
 for(;DD_Parse_Require(ctx, DD_S8Lit("@"));)
 {
  DD_Token name = {0};
  if(DD_Parse_RequireKind(ctx, DD_TokenKind_Identifier, &name))
  {
   DD_Node *tag = _DD_MakeNodeFromToken_Ctx(ctx, DD_NodeKind_Tag, name);
   _DD_ParseSet(ctx, tag, _DD_ParseSetFlag_Paren, &tag->first_child, &tag->last_child);
   _DD_PushNodeToList(&first, &last, DD_NilNode(), tag);
  }
  else
  {
   break;
  }
 }
 
 *first_out = first;
 *last_out = last;
}

DD_FUNCTION_IMPL DD_ParseResult
DD_ParseOneNode(DD_String8 filename, DD_String8 contents)
{
 DD_ParseCtx ctx = DD_Parse_InitializeCtx(filename, contents);
 return _DD_ParseOneNode(&ctx);
}

DD_FUNCTION DD_Node *
DD_ParseWholeString(DD_String8 filename, DD_String8 contents)
{
 DD_Node *root = DD_MakeNodeFromString(DD_NodeKind_Set, filename, contents.str, contents.str, DD_PushStringF("`DD Parsed From \"%.*s\"`", DD_StringExpand(filename)));
 if(contents.size > 0)
 {
  DD_ParseCtx ctx = DD_Parse_InitializeCtx(filename, contents);
  for(DD_u64 i = 0; i < contents.size;)
  {
   DD_ParseResult parse = _DD_ParseOneNode(&ctx);
   _DD_PushNodeToList(&root->first_child, &root->last_child, root, parse.node);
   i += parse.bytes_parsed;
   if(parse.bytes_parsed == 0)
   {
    break;
   }
  }
 }
 return root;
}

DD_FUNCTION_IMPL DD_Node *
DD_ParseWholeFile(DD_String8 filename)
{
 return DD_ParseWholeString(filename, DD_LoadEntireFile(filename));
}

DD_FUNCTION_IMPL DD_b32
DD_NodeIsNil(DD_Node *node)
{
 return node == 0 || node == &_dd_nil_node || node->kind == DD_NodeKind_Nil;
}

DD_FUNCTION_IMPL DD_Node *
DD_NilNode(void) { return &_dd_nil_node; }

DD_FUNCTION_IMPL DD_Node *
DD_MakeNodeFromToken(DD_NodeKind kind, DD_String8 filename, DD_u8 *file, DD_u8 *at, DD_Token token)
{
 return _DD_MakeNode(kind, token.string, token.outer_string, filename, file, at);
}

DD_FUNCTION_IMPL DD_Node *
DD_MakeNodeFromString(DD_NodeKind kind, DD_String8 filename, DD_u8 *file, DD_u8 *at, DD_String8 string)
{
 return _DD_MakeNode(kind, string, string, filename, file, at);
}

DD_FUNCTION_IMPL void
DD_PushSibling(DD_Node **first, DD_Node **last, DD_Node *parent, DD_Node *new_sibling)
{
 _DD_PushNodeToList(first, last, parent, new_sibling);
}

DD_FUNCTION_IMPL void
DD_PushChild(DD_Node *parent, DD_Node *new_child)
{
 _DD_PushNodeToList(&parent->first_child, &parent->last_child, parent, new_child);
}

DD_FUNCTION_IMPL void
DD_PushTag(DD_Node *node, DD_Node *tag)
{
 _DD_PushNodeToList(&node->first_tag, &node->last_tag, node, tag);
}

DD_FUNCTION_IMPL DD_Node *
DD_NodeInList(DD_Node *first, DD_Node *last, DD_String8 string)
{
 DD_Node *result = DD_NilNode();
 for(DD_Node *node = first; !DD_NodeIsNil(node); node = node->next)
 {
  if(DD_StringMatch(string, node->string, 0))
  {
   result = node;
   break;
  }
 }
 return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_NthNodeInList(DD_Node *first, DD_Node *last, int n)
{
 DD_Node *result = DD_NilNode();
 if(n >= 0)
 {
  int idx = 0;
  for(DD_Node *node = first; !DD_NodeIsNil(node); node = node->next, idx += 1)
  {
   if(idx == n)
   {
    result = node;
    break;
   }
  }
 }
 return result;
}

DD_FUNCTION_IMPL int
DD_IndexFromNode(DD_Node *node)
{
 int idx = 0;
 if(node && !DD_NodeIsNil(node))
 {
  for(DD_Node *last = node->prev; last; last = last->prev, idx += 1);
 }
 return idx;
}

DD_FUNCTION_IMPL DD_Node *
DD_NextNodeSibling(DD_Node *last, DD_String8 string)
{
 DD_Node *result = DD_NilNode();
 if(last)
 {
  for(DD_Node *node = last->next; node; node = node->next)
  {
   if(DD_StringMatch(string, node->string, 0))
   {
    result = node;
    break;
   }
  }
 }
 return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_TagOnNode(DD_Node *node, DD_String8 tag_string)
{
 return DD_NodeInList(node->first_tag, node->last_tag, tag_string);
}

DD_FUNCTION_IMPL DD_Node *
DD_ChildOnNode(DD_Node *node, DD_String8 child_string)
{
 return DD_NodeInList(node->first_child, node->last_child, child_string);
}

DD_FUNCTION_IMPL DD_Node *
DD_NthTagArg(DD_Node *node, DD_String8 tag_string, int n)
{
 DD_Node *tag = DD_TagOnNode(node, tag_string);
 return DD_NthChild(tag, n);
}

DD_FUNCTION_IMPL DD_Node *
DD_NthChild(DD_Node *node, int n)
{
 return DD_NthNodeInList(node->first_child, node->last_child, n);
}

static DD_Expr _dd_nil_expr = {0};

DD_FUNCTION_IMPL DD_Expr *
DD_NilExpr(void)
{
 return &_dd_nil_expr;
}

DD_FUNCTION_IMPL DD_Expr *
DD_MakeExpr(DD_Node *node, DD_ExprKind kind, DD_Expr *left, DD_Expr *right)
{
 DD_Expr *expr = calloc(1, sizeof(*expr));
 if(expr)
 {
  expr->node = node;
  expr->kind = kind;
  expr->sub[0] = left;
  expr->sub[1] = right;
 }
 else
 {
  expr = DD_NilExpr();
 }
 return expr;
}

typedef struct _DD_NodeParseCtx _DD_NodeParseCtx;
struct _DD_NodeParseCtx
{
 DD_Node *at;
};

DD_PRIVATE_FUNCTION_IMPL DD_b32
_DD_RequireNodeKind(_DD_NodeParseCtx *ctx, DD_NodeKind kind, DD_Node **out)
{
 DD_b32 result = 0;
 if(ctx->at && ctx->at->kind == kind)
 {
  result = 1;
  if(out)
  {
   *out = ctx->at;
  }
  ctx->at = ctx->at->next;
 }
 return result;
}

DD_PRIVATE_FUNCTION_IMPL DD_b32
_DD_RequireNode(_DD_NodeParseCtx *ctx, DD_String8 string)
{
 DD_b32 result = 0;
 if(ctx->at && DD_StringMatch(string, ctx->at->string, 0))
 {
  result = 1;
  ctx->at = ctx->at->next;
 }
 return result;
}

DD_FUNCTION_IMPL DD_Expr *
DD_ParseAsExpr(DD_Node *node)
{
 DD_Expr *expr = DD_NilExpr();
 
 return expr;
}

DD_FUNCTION_IMPL void
DD_OutputTree(FILE *file, DD_Node *node)
{
 for(DD_Node *tag = node->first_tag; !DD_NodeIsNil(tag); tag = tag->next)
 {
  fprintf(file, "@%.*s", DD_StringExpand(tag->string));
  if(!DD_NodeIsNil(tag->first_child))
  {
   fprintf(file, "(");
   for(DD_Node *child = tag->first_child; !DD_NodeIsNil(child); child = child->next)
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
 if(!DD_NodeIsNil(node->first_child))
 {
  fprintf(file, ":\n{\n");
  for(DD_Node *child = node->first_child; !DD_NodeIsNil(child); child = child->next)
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

DD_FUNCTION_IMPL void
DD_OutputExpr(FILE *file, DD_Expr *expr)
{
 
}

DD_FUNCTION_IMPL void
DD_OutputTree_C_String(FILE *file, DD_Node *node)
{
 fprintf(file, "\"");
 for(DD_u64 i = 0; i < node->string.size; i += 1)
 {
  if(node->string.str[i] == '\n')
  {
   fprintf(file, "\\n\"\n\"");
  }
  else
  {
   fprintf(file, "%c", node->string.str[i]);
  }
 }
 fprintf(file, "\"");
}

DD_FUNCTION_IMPL void
DD_OutputTree_C_Struct(FILE *file, DD_Node *node)
{
 if(node)
 {
  fprintf(file, "typedef struct %.*s %.*s;\n",
          DD_StringExpand(node->string),
          DD_StringExpand(node->string));
  fprintf(file, "struct %.*s\n{\n", DD_StringExpand(node->string));
  for(DD_Node *child = node->first_child; !DD_NodeIsNil(child); child = child->next)
  {
   DD_OutputTree_C_Decl(file, child);
   fprintf(file, ";\n");
  }
  fprintf(file, "};\n\n");
 }
}

DD_FUNCTION_IMPL void
DD_OutputTree_C_Decl(FILE *file, DD_Node *node)
{
 if(node)
 {
  DD_Expr *type = DD_ParseAsExpr(node);
  DD_OutputType_C_LHS(file, type);
  fprintf(file, " %.*s", DD_StringExpand(node->string));
  DD_OutputType_C_RHS(file, type);
 }
}

DD_FUNCTION_IMPL void
DD_OutputType_C_LHS(FILE *file, DD_Expr *type)
{
 
}

DD_FUNCTION_IMPL void
DD_OutputType_C_RHS(FILE *file, DD_Expr *type)
{
 
}

DD_FUNCTION_IMPL DD_CommandLine
DD_CommandLine_Start(int argument_count, char **arguments)
{
 DD_CommandLine cmdln = {0};
 cmdln.arguments = calloc(sizeof(DD_String8), argument_count-1);
 for(int i = 1; i < argument_count; i += 1)
 {
  cmdln.arguments[i-1] = DD_PushStringF("%s", arguments[i]);
 }
 cmdln.argument_count = argument_count-1;
 return cmdln;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_Flag(DD_CommandLine *cmdln, DD_String8 string)
{
 DD_b32 result = 0;
 for(int i = 0; i < cmdln->argument_count; i += 1)
 {
  if(DD_StringMatch(string, cmdln->arguments[i], 0))
  {
   result = 1;
   cmdln->arguments[i].str = 0;
   cmdln->arguments[i].size = 0;
   break;
  }
 }
 return result;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagStrings(DD_CommandLine *cmdln, DD_String8 string, int out_count, DD_String8 *out)
{
 DD_b32 result = 0;
 for(int i = 0; i < cmdln->argument_count; i += 1)
 {
  if(DD_StringMatch(string, cmdln->arguments[i], 0))
  {
   cmdln->arguments[i].str = 0;
   cmdln->arguments[i].size = 0;
   if(cmdln->argument_count > i + out_count)
   {
    for(int out_idx = 0; out_idx < out_count; out_idx += 1)
    {
     out[out_idx] = cmdln->arguments[i+out_idx+1];
     cmdln->arguments[i+out_idx+1].str = 0;
     cmdln->arguments[i+out_idx+1].size = 0;
    }
    result = 1;
    break;
   }
  }
 }
 return result;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagIntegers(DD_CommandLine *cmdln, DD_String8 string, int out_count, DD_i64 *out)
{
 DD_b32 result = 0;
 for(int i = 0; i < cmdln->argument_count; i += 1)
 {
  if(DD_StringMatch(string, cmdln->arguments[i], 0))
  {
   cmdln->arguments[i].str = 0;
   cmdln->arguments[i].size = 0;
   if(cmdln->argument_count > i + out_count)
   {
    for(int out_idx = 0; out_idx < out_count; out_idx += 1)
    {
     out[out_idx] = (DD_i64)DD_IntFromString(cmdln->arguments[i+out_idx+1]);
     cmdln->arguments[i+out_idx+1].str = 0;
     cmdln->arguments[i+out_idx+1].size = 0;
    }
    result = 1;
    break;
   }
  }
 }
 return result;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagString(DD_CommandLine *cmdln, DD_String8 string, DD_String8 *out)
{
 return DD_CommandLine_FlagStrings(cmdln, string, 1, out);
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagInteger(DD_CommandLine *cmdln, DD_String8 string, DD_i64 *out)
{
 return DD_CommandLine_FlagIntegers(cmdln, string, 1, out);
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_Increment(DD_CommandLine *cmdln, DD_String8 **string_ptr)
{
 DD_b32 result = 0;
 DD_String8 *string = *string_ptr;
 if(string == 0)
 {
  for(int i = 0; i < cmdln->argument_count; i += 1)
  {
   if(cmdln->arguments[i].str)
   {
    string = &cmdln->arguments[i];
    break;
   }
  }
 }
 else
 {
  int idx = (int)(string - cmdln->arguments);
  string = 0;
  for(int i = idx+1; i < cmdln->argument_count; i += 1)
  {
   if(cmdln->arguments[i].str)
   {
    string = &cmdln->arguments[i];
    break;
   }
  }
 }
 *string_ptr = string;
 result = !!string;
 return result;
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