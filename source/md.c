#define MD_FUNCTION_IMPL MD_FUNCTION
#define MD_PRIVATE_FUNCTION_IMPL MD_FUNCTION_IMPL

//~ NOTE(rjf): OS Layers

MD_PRIVATE_FUNCTION_IMPL MD_b32 _MD_OS_IMPL_FileIter_Increment(MD_FileIter *it, MD_String8 path, MD_FileInfo *out_info);

#if MD_OS_WINDOWS
#include "md_win32.c"
#endif
#if MD_OS_LINUX
#include "md_posix.c"
#endif

//~

// NOTE(allen): Review @rjf; Building in C++
// While very latest version of C++ have designated initializers
// I would like to be able to build on more simple versions, so I
// ditched the designated initializers in favor of the extra work
// of maintaining order based initializers.

static MD_Node _md_nil_node =
{
    &_md_nil_node, // next
    &_md_nil_node, // prev
    &_md_nil_node, // parent
    &_md_nil_node, // first_child
    &_md_nil_node, // last_child
    &_md_nil_node, // first_tag
    &_md_nil_node, // last_tag
    MD_NodeKind_Nil,
    0, // flags
    {0}, // string
    {0}, // whole_string
    0xdeadffffffffffull,
    {(MD_u8*)"`NIL DD NODE`", 13},
    0,
    0,
};

#if 0
static MD_Node _md_nil_node =
{
    .next          = &_md_nil_node,
    .prev          = &_md_nil_node,
    .parent        = &_md_nil_node,
    .first_child   = &_md_nil_node,
    .last_child    = &_md_nil_node,
    .first_tag     = &_md_nil_node,
    .last_tag      = &_md_nil_node,
    .kind          = MD_NodeKind_Nil,
    .string        = {0},
    .whole_string  = {0},
    .string_hash   = 0xdeadffffffffffull,
    .filename      = {"`NIL DD NODE`", 13},
    .file_contents = 0,
    .at            = 0,
};
#endif

MD_PRIVATE_FUNCTION_IMPL void
_MD_MemoryZero(void *memory, MD_u64 size)
{
    memset(memory, 0, size);
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_MemoryCopy(void *dest, void *src, MD_u64 size)
{
    memcpy(dest, src, size);
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_WriteStringToBuffer(MD_String8 string, MD_u64 max, void *dest)
{
    MD_u64 write_size = string.size;
    if(write_size > max) write_size = max;
    _MD_MemoryCopy(dest, string.str, write_size);
}

MD_FUNCTION_IMPL MD_b32
MD_CharIsAlpha(MD_u8 c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

MD_FUNCTION_IMPL MD_b32
MD_CharIsDigit(MD_u8 c)
{
    return (c >= '0' && c <= '9');
}

MD_FUNCTION_IMPL MD_b32
MD_CharIsSymbol(MD_u8 c)
{
    return (c == '~' || c == '!' || c == '@' || c == '#' || c == '$' ||
            c == '%' || c == '^' || c == '&' || c == '*' || c == '(' ||
            c == ')' || c == '-' || c == '=' || c == '+' || c == '[' ||
            c == ']' || c == '{' || c == '}' || c == ':' || c == ';' ||
            c == ',' || c == '<' || c == '.' || c == '>' || c == '/' ||
            c == '?' || c == '|' || c == '\\');
}

MD_FUNCTION_IMPL MD_u8
MD_CharToUpper(MD_u8 c)
{
    return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

MD_FUNCTION_IMPL MD_u8
MD_CharToLower(MD_u8 c)
{
    return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}

MD_FUNCTION_IMPL MD_u8
MD_CorrectSlash(MD_u8 c)
{
    return (c == '\\' ? '/' : c);
}

MD_FUNCTION_IMPL MD_String8
MD_S8(MD_u8 *str, MD_u64 size)
{
    MD_String8 string;
    string.str = str;
    string.size = size;
    return string;
}

MD_FUNCTION_IMPL MD_String8
MD_StringSubstring(MD_String8 str, MD_u64 min, MD_u64 max)
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
        MD_u64 swap = min;
        min = max;
        max = swap;
    }
    str.size = max - min;
    str.str += min;
    return str;
}

MD_FUNCTION_IMPL MD_String8
MD_StringSkip(MD_String8 str, MD_u64 max)
{
    return MD_StringSubstring(str, 0, max);
}

MD_FUNCTION_IMPL MD_String8
MD_StringChop(MD_String8 str, MD_u64 min)
{
    return MD_StringSubstring(str, min, str.size);
}

MD_FUNCTION_IMPL MD_String8
MD_StringPrefix(MD_String8 str, MD_u64 size)
{
    return MD_StringSubstring(str, 0, size);
}

MD_FUNCTION_IMPL MD_String8
MD_StringSuffix(MD_String8 str, MD_u64 size)
{
    return MD_StringSubstring(str, str.size - size, size);
}

MD_FUNCTION_IMPL MD_b32
MD_StringMatch(MD_String8 a, MD_String8 b, MD_StringMatchFlags flags)
{
    int result = 0;
    if(a.size == b.size || flags & MD_StringMatchFlag_RightSideSloppy)
    {
        result = 1;
        for(MD_u64 i = 0; i < a.size; i += 1)
        {
            MD_b32 match = (a.str[i] == b.str[i]);
            if(flags & MD_StringMatchFlag_CaseInsensitive)
            {
                match |= (MD_CharToLower(a.str[i]) == MD_CharToLower(b.str[i]));
            }
            if(flags & MD_StringMatchFlag_SlashInsensitive)
            {
                match |= (MD_CorrectSlash(a.str[i]) == MD_CorrectSlash(b.str[i]));
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

MD_FUNCTION_IMPL MD_u64
MD_FindSubstring(MD_String8 str, MD_String8 substring, MD_u64 start_pos, MD_StringMatchFlags flags)
{
    MD_b32 found = 0;
    MD_u64 found_idx = str.size;
    for(MD_u64 i = start_pos; i < str.size; i += 1)
    {
        if(i + substring.size <= str.size)
        {
            MD_String8 substr_from_str = MD_StringSubstring(str, i, i+substring.size);
            if(MD_StringMatch(substr_from_str, substring, flags))
            {
                found_idx = i;
                found = 1;
                if(!(flags & MD_StringMatchFlag_FindLast))
                {
                    break;
                }
            }
        }
    }
    return found_idx;
}

MD_FUNCTION_IMPL MD_u64
MD_FindLastSubstring(MD_String8 str, MD_String8 substring, MD_StringMatchFlags flags)
{
    return MD_FindSubstring(str, substring, 0, flags | MD_StringMatchFlag_FindLast);
}

MD_FUNCTION_IMPL MD_String8
MD_TrimExtension(MD_String8 string)
{
    MD_u64 period_pos = MD_FindLastSubstring(string, MD_S8Lit("."), 0);
    if(period_pos < string.size)
    {
        string.size = period_pos;
    }
    return string;
}

MD_FUNCTION_IMPL MD_String8
MD_TrimFolder(MD_String8 string)
{
    MD_u64 slash_pos = MD_FindLastSubstring(string, MD_S8Lit("/"), MD_StringMatchFlag_SlashInsensitive);
    if(slash_pos < string.size)
    {
        string.str += slash_pos+1;
        string.size -= slash_pos+1;
    }
    return string;
}

MD_FUNCTION_IMPL MD_String8
MD_ExtensionFromPath(MD_String8 string)
{
    MD_u64 period_pos = MD_FindLastSubstring(string, MD_S8Lit("."), 0);
    if(period_pos < string.size)
    {
        string.str += period_pos+1;
        string.size -= period_pos+1;
    }
    return string;
}

MD_FUNCTION_IMPL MD_String8
MD_FolderFromPath(MD_String8 string)
{
    MD_u64 slash_pos = MD_FindLastSubstring(string, MD_S8Lit("/"), MD_StringMatchFlag_SlashInsensitive);
    if(slash_pos < string.size)
    {
        string.size = slash_pos;
    }
    return string;
}

MD_FUNCTION_IMPL MD_String8
MD_PushStringCopy(MD_String8 string)
{
    MD_String8 res;
    res.size = string.size;
    res.str = (MD_u8*)calloc(string.size+1, 1);
    _MD_MemoryCopy(res.str, string.str, string.size);
    return res;
}

MD_FUNCTION_IMPL MD_String8
MD_PushStringFV(char *fmt, va_list args)
{
    MD_String8 result = {0};
    va_list args2;
    va_copy(args2, args);
    MD_u64 needed_bytes = vsnprintf(0, 0, fmt, args)+1;
    result.str = (MD_u8*)calloc(needed_bytes, 1);
    result.size = needed_bytes-1;
    vsnprintf((char*)result.str, needed_bytes, fmt, args2);
    return result;
}

MD_FUNCTION_IMPL MD_String8
MD_PushStringF(char *fmt, ...)
{
    MD_String8 result = {0};
    va_list args;
    va_start(args, fmt);
    result = MD_PushStringFV(fmt, args);
    va_end(args);
    return result;
}

MD_FUNCTION_IMPL void
MD_PushStringToList(MD_String8List *list, MD_String8 string)
{
    MD_String8Node *node = (MD_String8Node*)calloc(1, sizeof(*node));
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

MD_FUNCTION_IMPL void
MD_PushStringListToList(MD_String8List *list, MD_String8List *to_push)
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
    _MD_MemoryZero(to_push, sizeof(*to_push));
}

MD_FUNCTION_IMPL MD_String8List
MD_SplitString(MD_String8 string, int split_count, MD_String8 *splits)
{
    MD_String8List list = {0};
    
    MD_u64 split_start = 0;
    for(MD_u64 i = 0; i < string.size; i += 1)
    {
        MD_b32 was_split = 0;
        for(int split_idx = 0; split_idx < split_count; split_idx += 1)
        {
            MD_b32 match = 0;
            if(i + splits[split_idx].size <= string.size)
            {
                match = 1;
                for(MD_u64 split_i = 0; split_i < splits[split_idx].size && i + split_i < string.size; split_i += 1)
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
                MD_String8 split_string = MD_S8(string.str + split_start, i - split_start);
                MD_PushStringToList(&list, split_string);
                split_start = i + splits[split_idx].size;
                i += splits[split_idx].size - 1;
                was_split = 1;
                break;
            }
        }
        
        if(was_split == 0 && i == string.size - 1)
        {
            MD_String8 split_string = MD_S8(string.str + split_start, i+1 - split_start);
            MD_PushStringToList(&list, split_string);
            break;
        }
    }
    
    return list;
}

MD_FUNCTION_IMPL MD_String8List
MD_SplitStringByString(MD_String8 string, MD_String8 split)
{
    return MD_SplitString(string, 1, &split);
}

MD_FUNCTION_IMPL MD_String8List
MD_SplitStringByCharacter(MD_String8 string, MD_u8 character)
{
    return MD_SplitStringByString(string, MD_S8(&character, 1));
}

MD_FUNCTION_IMPL int
MD_IntFromString(MD_String8 string)
{
    char str[64];
    _MD_WriteStringToBuffer(string, sizeof(str), str);
    return atoi(str);
}

MD_FUNCTION_IMPL float
MD_FloatFromString(MD_String8 string)
{
    char str[64];
    _MD_WriteStringToBuffer(string, sizeof(str), str);
    return atof(str);
}

MD_FUNCTION_IMPL MD_u64
MD_HashString(MD_String8 string)
{
    MD_u64 result = 5381;
    for(MD_u64 i = 0; i < string.size; i += 1)
    {
        result = ((result << 5) + result) + string.str[i];
    }
    return result;
}

MD_FUNCTION_IMPL MD_u64
MD_CalculateCStringLength(char *cstr)
{
    MD_u64 i = 0;
    for(; cstr[i]; i += 1);
    return i;
}

////////////////////////////////
// NOTE(allen): Unicode

MD_GLOBAL MD_u8 dd_utf8_class[32] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

MD_FUNCTION_IMPL MD_UnicodeConsume
MD_CodepointFromUtf8(MD_u8 *str, MD_u64 max)
{
#define MD_bitmask1 0x01
#define MD_bitmask2 0x03
#define MD_bitmask3 0x07
#define MD_bitmask4 0x0F
#define MD_bitmask5 0x1F
#define MD_bitmask6 0x3F
#define MD_bitmask7 0x7F
#define MD_bitmask8 0xFF
#define MD_bitmask9  0x01FF
#define MD_bitmask10 0x03FF
    
    MD_UnicodeConsume result = {~((MD_u32)0), 1};
    MD_u8 byte = str[0];
    MD_u8 byte_class = dd_utf8_class[byte >> 3];
    switch (byte_class)
    {
        case 1:
        {
            result.codepoint = byte;
        }break;
        
        case 2:
        {
            if (2 <= max)
            {
                MD_u8 cont_byte = str[1];
                if (dd_utf8_class[cont_byte >> 3] == 0)
                {
                    result.codepoint = (byte & MD_bitmask5) << 6;
                    result.codepoint |=  (cont_byte & MD_bitmask6);
                    result.advance = 2;
                }
            }
        }break;
        
        case 3:
        {
            if (3 <= max)
            {
                MD_u8 cont_byte[2] = {str[1], str[2]};
                if (dd_utf8_class[cont_byte[0] >> 3] == 0 &&
                    dd_utf8_class[cont_byte[1] >> 3] == 0)
                {
                    result.codepoint = (byte & MD_bitmask4) << 12;
                    result.codepoint |= ((cont_byte[0] & MD_bitmask6) << 6);
                    result.codepoint |=  (cont_byte[1] & MD_bitmask6);
                    result.advance = 3;
                }
            }
        }break;
        
        case 4:
        {
            if (4 <= max)
            {
                MD_u8 cont_byte[3] = {str[1], str[2], str[3]};
                if (dd_utf8_class[cont_byte[0] >> 3] == 0 &&
                    dd_utf8_class[cont_byte[1] >> 3] == 0 &&
                    dd_utf8_class[cont_byte[2] >> 3] == 0)
                {
                    result.codepoint = (byte & MD_bitmask3) << 18;
                    result.codepoint |= ((cont_byte[0] & MD_bitmask6) << 12);
                    result.codepoint |= ((cont_byte[1] & MD_bitmask6) <<  6);
                    result.codepoint |=  (cont_byte[2] & MD_bitmask6);
                    result.advance = 4;
                }
            }
        }break;
    }
    
    return(result);
}

MD_FUNCTION_IMPL MD_UnicodeConsume
MD_CodepointFromUtf16(MD_u16 *out, MD_u64 max)
{
    MD_UnicodeConsume result = {~((MD_u32)0), 1};
    result.codepoint = out[0];
    result.advance = 1;
    if (1 < max && 0xD800 <= out[0] && out[0] < 0xDC00 && 0xDC00 <= out[1] && out[1] < 0xE000)
    {
        result.codepoint = ((out[0] - 0xD800) << 10) | (out[1] - 0xDC00);
        result.advance = 2;
    }
    return(result);
}

MD_FUNCTION MD_u32
MD_Utf8FromCodepoint(MD_u8 *out, MD_u32 codepoint)
{
#define MD_bit8 0x80
    MD_u32 advance = 0;
    if (codepoint <= 0x7F)
    {
        out[0] = (MD_u8)codepoint;
        advance = 1;
    }
    else if (codepoint <= 0x7FF)
    {
        out[0] = (MD_bitmask2 << 6) | ((codepoint >> 6) & MD_bitmask5);
        out[1] = MD_bit8 | (codepoint & MD_bitmask6);
        advance = 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        out[0] = (MD_bitmask3 << 5) | ((codepoint >> 12) & MD_bitmask4);
        out[1] = MD_bit8 | ((codepoint >> 6) & MD_bitmask6);
        out[2] = MD_bit8 | ( codepoint       & MD_bitmask6);
        advance = 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        out[0] = (MD_bitmask4 << 3) | ((codepoint >> 18) & MD_bitmask3);
        out[1] = MD_bit8 | ((codepoint >> 12) & MD_bitmask6);
        out[2] = MD_bit8 | ((codepoint >>  6) & MD_bitmask6);
        out[3] = MD_bit8 | ( codepoint        & MD_bitmask6);
        advance = 4;
    }
    else
    {
        out[0] = '?';
        advance = 1;
    }
    return(advance);
}

MD_FUNCTION MD_u32
MD_Utf16FromCodepoint(MD_u16 *out, MD_u32 codepoint)
{
    MD_u32 advance = 1;
    if (codepoint == ~((MD_u32)0))
    {
        out[0] = (MD_u16)'?';
    }
    else if (codepoint < 0x10000)
    {
        out[0] = (MD_u16)codepoint;
    }
    else
    {
        MD_u64 v = codepoint - 0x10000;
        out[0] = 0xD800 + (v >> 10);
        out[1] = 0xDC00 + (v & MD_bitmask10);
        advance = 2;
    }
    return(advance);
}

MD_FUNCTION MD_String8
MD_S8FromS16(MD_String16 in)
{
    MD_u64 cap = in.size*3;
    MD_u8 *str = (MD_u8*)malloc(cap + 1);
    MD_u16 *ptr = in.str;
    MD_u16 *opl = ptr + in.size;
    MD_u64 size = 0;
    MD_UnicodeConsume consume;
    for (;ptr < opl;)
    {
        consume = MD_CodepointFromUtf16(ptr, opl - ptr);
        ptr += consume.advance;
        size += MD_Utf8FromCodepoint(str + size, consume.codepoint);
    }
    str[size] = 0;
    return(MD_S8(str, size));
}

MD_FUNCTION MD_String16
MD_S16FromS8(MD_String8 in)
{
    MD_u64 cap = in.size*2;
    MD_u16 *str = (MD_u16*)malloc(sizeof(MD_u16)*(cap + 1));
    MD_u8 *ptr = in.str;
    MD_u8 *opl = ptr + in.size;
    MD_u64 size = 0;
    MD_UnicodeConsume consume;
    for (;ptr < opl;)
    {
        consume = MD_CodepointFromUtf8(ptr, opl - ptr);
        ptr += consume.advance;
        size += MD_Utf16FromCodepoint(str + size, consume.codepoint);
    }
    str[size] = 0;
    MD_String16 result = {str, size};
    return(result);
}

MD_FUNCTION MD_String8
MD_S8FromS32(MD_String32 in)
{
    MD_u64 cap = in.size*4;
    MD_u8 *str = (MD_u8*)malloc(cap + 1);
    MD_u32 *ptr = in.str;
    MD_u32 *opl = ptr + in.size;
    MD_u64 size = 0;
    MD_UnicodeConsume consume;
    for (;ptr < opl; ptr += 1)
    {
        size += MD_Utf8FromCodepoint(str + size, *ptr);
    }
    str[size] = 0;
    return(MD_S8(str, size));
}

MD_FUNCTION MD_String32
MD_S32FromS8(MD_String8 in)
{
    MD_u64 cap = in.size;
    MD_u32 *str = (MD_u32*)malloc(sizeof(MD_u32)*(cap + 1));
    MD_u8 *ptr = in.str;
    MD_u8 *opl = ptr + in.size;
    MD_u64 size = 0;
    MD_UnicodeConsume consume;
    for (;ptr < opl;)
    {
        consume = MD_CodepointFromUtf8(ptr, opl - ptr);
        ptr += consume.advance;
        str[size] = consume.codepoint;
        size += 1;
    }
    str[size] = 0;
    MD_String32 result = {str, size};
    return(result);
}


MD_PRIVATE_FUNCTION_IMPL void
_MD_NodeTable_Initialize(MD_NodeTable *table)
{
    if(table->table_size == 0)
    {
        table->table_size = 4096;
        table->table = (MD_NodeTableSlot**)calloc(table->table_size, sizeof(MD_NodeTableSlot *));
    }
}

MD_FUNCTION_IMPL MD_NodeTableSlot *
MD_NodeTable_Lookup(MD_NodeTable *table, MD_String8 string)
{
    _MD_NodeTable_Initialize(table);
    
    MD_NodeTableSlot *slot = 0;
    MD_u64 hash = MD_HashString(string);
    MD_u64 index = hash % table->table_size;
    for(MD_NodeTableSlot *candidate = table->table[index]; candidate; candidate = candidate->next)
    {
        if(candidate->hash == hash)
        {
            slot = candidate;
            break;
        }
    }
    return slot;
}

MD_FUNCTION_IMPL MD_b32
MD_NodeTable_Insert(MD_NodeTable *table, MD_NodeTableCollisionRule collision_rule, MD_String8 string, MD_Node *node)
{
    _MD_NodeTable_Initialize(table);
    
    MD_NodeTableSlot *slot = 0;
    MD_u64 hash = MD_HashString(string);
    MD_u64 index = hash % table->table_size;
    
    for(MD_NodeTableSlot *candidate = table->table[index]; candidate; candidate = candidate->next)
    {
        if(candidate->hash == hash)
        {
            slot = candidate;
            break;
        }
    }
    
    if(slot == 0 || (slot != 0 && collision_rule == MD_NodeTableCollisionRule_Chain))
    {
        slot = (MD_NodeTableSlot*)calloc(1, sizeof(*slot));
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

MD_FUNCTION_IMPL MD_Token
MD_ZeroToken(void)
{
    MD_Token token = {0};
    return token;
}

MD_FUNCTION_IMPL MD_b32
MD_TokenKindIsWhitespace(MD_TokenKind kind)
{
    return kind > MD_TokenKind_WhitespaceMin && kind < MD_TokenKind_WhitespaceMax;
}

MD_FUNCTION_IMPL MD_ParseCtx
MD_Parse_InitializeCtx(MD_String8 filename, MD_String8 contents)
{
    MD_ParseCtx ctx = {0};
    ctx.first_root = ctx.last_root = MD_NilNode();
    ctx.at = contents.str;
    ctx.file_contents = contents;
    ctx.filename = filename;
    return ctx;
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_PushNodeToList(MD_Node **firstp, MD_Node **lastp, MD_Node *parent, MD_Node *node)
{
    if(!MD_NodeIsNil(node))
    {
        MD_Node *first = *firstp;
        MD_Node *last = *lastp;
        if(MD_NodeIsNil(last))
        {
            first = last = node;
            node->next = node->prev = MD_NilNode();
        }
        else
        {
            last->next = node;
            node->next = MD_NilNode();
            node->prev = last;
            last = last->next;
        }
        *firstp = first;
        *lastp = last;
        node->parent = parent;
    }
}

MD_FUNCTION_IMPL MD_Token
MD_Parse_PeekAll(MD_ParseCtx *ctx)
{
    MD_Token token = {0};
    MD_u8 *one_past_last = ctx->file_contents.str + ctx->file_contents.size;
    
    enum
    {
        MD_ReadMode_Normal,
        MD_ReadMode_SingleLineComment,
        MD_ReadMode_MultiLineComment,
    };
    int read_mode = MD_ReadMode_Normal;
    int multiline_comment_nest = 0;
    
    for(MD_u64 i = 0; ctx->at + i < one_past_last; i += 1)
    {
        
        if(read_mode == MD_ReadMode_SingleLineComment)
        {
            if(ctx->at[i] == '\n')
            {
                read_mode = MD_ReadMode_Normal;
            }
        }
        else if(read_mode == MD_ReadMode_MultiLineComment)
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
                read_mode = MD_ReadMode_Normal;
            }
        }
        else
        {
#define MD_TokenizerLoop(name, start, cond) MD_u64 name = start; for(; ((cond)) && ctx->at + name < one_past_last; name += 1)
            
            // NOTE(rjf): Single-Line Comments
            if(ctx->at[i] == '/' && ctx->at[i+1] == '/')
            {
                read_mode = MD_ReadMode_SingleLineComment;
            }
            
            // NOTE(rjf): Multi-Line Comments
            else if(ctx->at[i] == '/' && ctx->at[i+1] == '*')
            {
                read_mode = MD_ReadMode_MultiLineComment;
                multiline_comment_nest = 1;
            }
            
            // NOTE(rjf): Newlines
            else if(ctx->at[i] == '\n')
            {
                token.kind = MD_TokenKind_Newline;
                token.string.str = ctx->at + i;
                token.string.size = 1;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Identifier
            else if(MD_CharIsAlpha(ctx->at[i]))
            {
                MD_TokenizerLoop(j, i+1,
                                 MD_CharIsAlpha(ctx->at[j]) ||
                                 MD_CharIsDigit(ctx->at[j]) ||
                                 ctx->at[j] == '_');
                token.kind = MD_TokenKind_Identifier;
                token.string.str = ctx->at + i;
                token.string.size = j - i;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Numeric Literal
            else if(MD_CharIsDigit(ctx->at[i]) ||
                    (ctx->at + 1 < one_past_last &&
                     ctx->at[i] == '-' && MD_CharIsDigit(ctx->at[i+1])))
            {
                MD_TokenizerLoop(j, i+1,
                                 MD_CharIsAlpha(ctx->at[j]) ||
                                 MD_CharIsDigit(ctx->at[j]) ||
                                 ctx->at[j] == '.');
                token.kind = MD_TokenKind_NumericLiteral;
                token.string.str = ctx->at + i;
                token.string.size = j - i;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Symbol
            else if(MD_CharIsSymbol(ctx->at[i]))
            {
                token.kind = MD_TokenKind_Symbol;
                token.string.str = ctx->at + i;
                token.string.size = 1;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Multi-Line String Literal
            else if(ctx->at+i+2  < one_past_last &&
                    ctx->at[i] == '"' && ctx->at[i+1] == '"' && ctx->at[i+2] == '"')
            {
                MD_TokenizerLoop(j, i+3, !(ctx->at + j + 2 < one_past_last &&
                                           ctx->at[j] == '"' && ctx->at[j+1] == '"' && ctx->at[j+2] == '"'));
                token.kind = MD_TokenKind_StringLiteral;
                token.string.str = ctx->at + i + 3;
                token.string.size = j - (i + 3);
                token.outer_string.str = ctx->at + i;
                token.outer_string.size = (j + 3) - i;
                break;
            }
            
            // NOTE(rjf): Single-Line String Literal
            else if(ctx->at[i] == '"')
            {
                MD_TokenizerLoop(j, i+1, ctx->at[j] != '"');
                token.kind = MD_TokenKind_StringLiteral;
                token.string.str = ctx->at + i + 1;
                token.string.size = j - (i + 1);
                token.outer_string.str = ctx->at + i;
                token.outer_string.size = (j - i) + 1;
                break;
            }
            
            // NOTE(rjf): Single-Line Char Literal
            else if(ctx->at[i] == '\'')
            {
                MD_TokenizerLoop(j, i+1, ctx->at[j] != '\'');
                token.kind = MD_TokenKind_CharLiteral;
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
                MD_TokenizerLoop(j, i+3, !(ctx->at + j + 2 < one_past_last &&
                                           ctx->at[j] == '\'' && ctx->at[j+1] == '\'' && ctx->at[j+2] == '\''));
                token.kind = MD_TokenKind_StringLiteral;
                token.string.str = ctx->at + i + 3;
                token.string.size = j - (i + 3);
                token.outer_string.str = ctx->at + i;
                token.outer_string.size = (j + 3) - i;
                break;
            }
            
#undef MD_TokenizerLoop
        }
        
    }
    return token;
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_Parse_Bump(MD_ParseCtx *ctx, MD_Token token)
{
    ctx->at = token.outer_string.str + token.outer_string.size;
}

MD_FUNCTION_IMPL MD_Token
MD_Parse_PeekNonWhitespace(MD_ParseCtx *ctx)
{
    MD_Token result = {0};
    MD_ParseCtx ctx_restore = *ctx;
    
    for(MD_Token token = MD_Parse_PeekAll(ctx);; token = MD_Parse_PeekAll(ctx))
    {
        if(!MD_TokenKindIsWhitespace(token.kind))
        {
            result = token;
            break;
        }
        else if(token.kind == MD_TokenKind_Nil)
        {
            break;
        }
        _MD_Parse_Bump(ctx, token);
    }
    
    *ctx = ctx_restore;
    return result;
}

MD_FUNCTION_IMPL MD_b32
MD_Parse_TokenMatch(MD_Token token, MD_String8 string, MD_StringMatchFlags flags)
{
    return MD_StringMatch(token.string, string, flags);
}

MD_FUNCTION_IMPL MD_b32
MD_Parse_Require(MD_ParseCtx *ctx, MD_String8 string)
{
    int result = 0;
    
    MD_Token token_all = MD_Parse_PeekAll(ctx);
    if(MD_StringMatch(token_all.string, string, 0))
    {
        result = 1;
        _MD_Parse_Bump(ctx, token_all);
        goto end;
    }
    
    MD_Token token_nonws = MD_Parse_PeekNonWhitespace(ctx);
    if(MD_StringMatch(token_nonws.string, string, 0))
    {
        result = 1;
        _MD_Parse_Bump(ctx, token_nonws);
        goto end;
    }
    
    end:;
    return result;
}

MD_FUNCTION_IMPL MD_b32
MD_Parse_RequireKind(MD_ParseCtx *ctx, MD_TokenKind kind, MD_Token *out_token)
{
    int result = 0;
    MD_Token token = MD_TokenKindIsWhitespace(kind) ? MD_Parse_PeekAll(ctx) : MD_Parse_PeekNonWhitespace(ctx);
    if(token.kind == kind)
    {
        result = 1;
        _MD_Parse_Bump(ctx, token);
        if(out_token)
        {
            *out_token = token;
        }
    }
    return result;
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_Error(MD_ParseCtx *ctx, char *fmt, ...)
{
    MD_Error *error = (MD_Error*)calloc(1, sizeof(*error));
    error->filename = ctx->filename;
    va_list args;
    va_start(args, fmt);
    error->string = MD_PushStringFV(fmt, args);
    va_end(args);
}

MD_PRIVATE_FUNCTION_IMPL MD_Node *
_MD_MakeNode(MD_NodeKind kind, MD_String8 string, MD_String8 whole_string, MD_String8 filename,
             MD_u8 *file_contents, MD_u8 *at)
{
    MD_Node *node = (MD_Node*)calloc(1, sizeof(*node));
    if(node)
    {
        node->kind = kind;
        node->string = string;
        node->whole_string = whole_string;
        node->next = node->prev = node->parent = node->first_child = node->last_child = node->first_tag = node->last_tag = MD_NilNode();
        node->filename = filename;
        node->file_contents = file_contents;
        node->at = at;
    }
    else
    {
        node = MD_NilNode();
    }
    return node;
}

MD_PRIVATE_FUNCTION_IMPL MD_Node *
_MD_MakeNodeFromToken_Ctx(MD_ParseCtx *ctx, MD_NodeKind kind, MD_Token token)
{
    return _MD_MakeNode(kind, token.string, token.outer_string, ctx->filename, ctx->file_contents.str, ctx->at);
}

MD_PRIVATE_FUNCTION_IMPL MD_Node *
_MD_MakeNodeFromString_Ctx(MD_ParseCtx *ctx, MD_NodeKind kind, MD_String8 string)
{
    return _MD_MakeNode(kind, string, string, ctx->filename, ctx->file_contents.str, ctx->at);
}

typedef MD_u32 _MD_ParseSetFlags;
enum
{
    _MD_ParseSetFlag_Paren    = (1<<0),
    _MD_ParseSetFlag_Brace    = (1<<1),
    _MD_ParseSetFlag_Bracket  = (1<<2),
    _MD_ParseSetFlag_Implicit = (1<<3),
};

MD_PRIVATE_FUNCTION_IMPL MD_ParseResult _MD_ParseOneNode(MD_ParseCtx *ctx);
MD_PRIVATE_FUNCTION_IMPL void _MD_ParseSet(MD_ParseCtx *ctx, MD_Node *parent, _MD_ParseSetFlags flags, MD_Node **first_out, MD_Node **last_out);
MD_PRIVATE_FUNCTION_IMPL void _MD_ParseTagList(MD_ParseCtx *ctx, MD_Node **first_out, MD_Node **last_out);

MD_PRIVATE_FUNCTION_IMPL void
_MD_SetNodeFlagsByToken(MD_Node *node, MD_Token token)
{
#define Flag(_kind, _flag) if(token.kind == _kind) { node->flags |= _flag; }
    Flag(MD_TokenKind_Identifier,     MD_NodeFlag_Identifier);
    Flag(MD_TokenKind_NumericLiteral, MD_NodeFlag_Numeric);
    Flag(MD_TokenKind_StringLiteral,  MD_NodeFlag_StringLiteral);
    Flag(MD_TokenKind_CharLiteral,    MD_NodeFlag_CharLiteral);
#undef Flag
}

MD_PRIVATE_FUNCTION_IMPL MD_ParseResult
_MD_ParseOneNode(MD_ParseCtx *ctx)
{
    MD_u8 *at_first = ctx->at;
    
    MD_ParseResult result = {0};
    result.node = MD_NilNode();
    
    MD_Token token = {0};
    
    MD_Node *first_tag = 0;
    MD_Node *last_tag = 0;
    _MD_ParseTagList(ctx, &first_tag, &last_tag);
    
    // NOTE(rjf): Unnamed Sets
    if(MD_Parse_TokenMatch(MD_Parse_PeekNonWhitespace(ctx), MD_S8Lit("("), 0) ||
       MD_Parse_TokenMatch(MD_Parse_PeekNonWhitespace(ctx), MD_S8Lit("{"), 0) ||
       MD_Parse_TokenMatch(MD_Parse_PeekNonWhitespace(ctx), MD_S8Lit("["), 0))
    {
        result.node = _MD_MakeNodeFromString_Ctx(ctx, MD_NodeKind_UnnamedSet, MD_S8Lit(""));
        _MD_ParseSet(ctx, result.node,
                     _MD_ParseSetFlag_Paren   |
                     _MD_ParseSetFlag_Brace   |
                     _MD_ParseSetFlag_Bracket,
                     &result.node->first_child,
                     &result.node->last_child);
        goto end_parse;
    }
    
    // NOTE(rjf): Labels
    else if(MD_Parse_RequireKind(ctx, MD_TokenKind_Identifier,     &token) ||
            MD_Parse_RequireKind(ctx, MD_TokenKind_NumericLiteral, &token) ||
            MD_Parse_RequireKind(ctx, MD_TokenKind_StringLiteral,  &token) ||
            MD_Parse_RequireKind(ctx, MD_TokenKind_CharLiteral,    &token) ||
            MD_Parse_RequireKind(ctx, MD_TokenKind_Symbol,         &token))
    {
        result.node = MD_MakeNodeFromToken(MD_NodeKind_Label, ctx->filename, ctx->file_contents.str, ctx->at, token);
        _MD_SetNodeFlagsByToken(result.node, token);
        // NOTE(rjf): Children
        if(MD_Parse_Require(ctx, MD_S8Lit(":")))
        {
            _MD_ParseSet(ctx, result.node,
                         _MD_ParseSetFlag_Paren   |
                         _MD_ParseSetFlag_Brace   |
                         _MD_ParseSetFlag_Bracket |
                         _MD_ParseSetFlag_Implicit,
                         &result.node->first_child,
                         &result.node->last_child);
        }
        goto end_parse;
    }
    
    end_parse:;
    if(!MD_NodeIsNil(result.node))
    {
        result.bytes_parsed = (MD_u64)(ctx->at - at_first);
        result.node->first_tag = first_tag;
        result.node->last_tag = last_tag;
        for(MD_Node *tag = first_tag; !MD_NodeIsNil(tag); tag = tag->next)
        {
            tag->parent = result.node;
        }
    }
    return result;
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_ParseSet(MD_ParseCtx *ctx, MD_Node *parent, _MD_ParseSetFlags flags,
             MD_Node **first_out, MD_Node **last_out)
{
    MD_Node *first = MD_NilNode();
    MD_Node *last = MD_NilNode();
    
    MD_b32 brace = 0;
    MD_b32 paren = 0;
    MD_b32 bracket = 0;
    MD_b32 terminate_with_separator = !!(flags & _MD_ParseSetFlag_Implicit);
    
    if(flags & _MD_ParseSetFlag_Brace && MD_Parse_Require(ctx, MD_S8Lit("{")))
    {
        parent->flags |= MD_NodeFlag_BraceLeft;
        brace = 1;
        terminate_with_separator = 0;
    }
    else if(flags & _MD_ParseSetFlag_Paren && MD_Parse_Require(ctx, MD_S8Lit("(")))
    {
        parent->flags |= MD_NodeFlag_ParenLeft;
        paren = 1;
        terminate_with_separator = 0;
    }
    else if(flags & _MD_ParseSetFlag_Bracket && MD_Parse_Require(ctx, MD_S8Lit("[")))
    {
        parent->flags |= MD_NodeFlag_BracketLeft;
        bracket = 1;
        terminate_with_separator = 0;
    }
    
    // NOTE(rjf): Parse children.
    if(brace || paren || bracket || terminate_with_separator)
    {
        for(MD_u64 child_idx = 0;; child_idx += 1)
        {
            MD_ParseResult parse = _MD_ParseOneNode(ctx);
            MD_Node *child = parse.node;
            if(!MD_NodeIsNil(child))
            {
                _MD_PushNodeToList(&first, &last, parent, child);
            }
            
            // NOTE(rjf): Separators.
            {
                MD_b32 result = 0;
                if(MD_Parse_Require(ctx, MD_S8Lit(",")))
                {
                    result |= 1;
                    child->flags |= MD_NodeFlag_BeforeComma;
                }
                else if(MD_Parse_Require(ctx, MD_S8Lit(";")))
                {
                    result |= 1;
                    child->flags |= MD_NodeFlag_BeforeSemicolon;
                }
                else if(MD_Parse_Require(ctx, MD_S8Lit("->")))
                {
                    result |= 1;
                    child->flags |= MD_NodeFlag_BeforeArrow;
                }
                result |= !!MD_Parse_Require(ctx, MD_S8Lit("\n"));
                if(result && terminate_with_separator)
                {
                    goto end_parse;
                }
            }
            
            if(brace)
            {
                if(MD_Parse_Require(ctx, MD_S8Lit("}")))
                {
                    parent->flags |= MD_NodeFlag_BraceRight;
                    goto end_parse;
                }
            }
            else
            {
                if(flags & _MD_ParseSetFlag_Paren && MD_Parse_Require(ctx, MD_S8Lit(")")))
                {
                    parent->flags |= MD_NodeFlag_ParenRight;
                    goto end_parse;
                }
                else if(flags & _MD_ParseSetFlag_Bracket && MD_Parse_Require(ctx, MD_S8Lit("]")))
                {
                    parent->flags |= MD_NodeFlag_BracketRight;
                    goto end_parse;
                }
            }
        }
    }
    
    end_parse:;
    *first_out = first;
    *last_out = last;
}

MD_PRIVATE_FUNCTION_IMPL void
_MD_ParseTagList(MD_ParseCtx *ctx, MD_Node **first_out, MD_Node **last_out)
{
    MD_Node *first = MD_NilNode();
    MD_Node *last = MD_NilNode();
    
    for(;MD_Parse_Require(ctx, MD_S8Lit("@"));)
    {
        MD_Token name = {0};
        if(MD_Parse_RequireKind(ctx, MD_TokenKind_Identifier, &name))
        {
            MD_Node *tag = _MD_MakeNodeFromToken_Ctx(ctx, MD_NodeKind_Tag, name);
            _MD_ParseSet(ctx, tag, _MD_ParseSetFlag_Paren, &tag->first_child, &tag->last_child);
            _MD_PushNodeToList(&first, &last, MD_NilNode(), tag);
        }
        else
        {
            break;
        }
    }
    
    *first_out = first;
    *last_out = last;
}

MD_FUNCTION_IMPL MD_ParseResult
MD_ParseOneNode(MD_String8 filename, MD_String8 contents)
{
    MD_ParseCtx ctx = MD_Parse_InitializeCtx(filename, contents);
    return _MD_ParseOneNode(&ctx);
}

MD_FUNCTION MD_Node *
MD_ParseWholeString(MD_String8 filename, MD_String8 contents)
{
    MD_Node *root = MD_MakeNodeFromString(MD_NodeKind_File, filename, contents.str, contents.str, MD_PushStringF("`DD Parsed From \"%.*s\"`", MD_StringExpand(filename)));
    if(contents.size > 0)
    {
        MD_ParseCtx ctx = MD_Parse_InitializeCtx(filename, contents);
        for(MD_u64 i = 0; i < contents.size;)
        {
            MD_ParseResult parse = _MD_ParseOneNode(&ctx);
            _MD_PushNodeToList(&root->first_child, &root->last_child, root, parse.node);
            i += parse.bytes_parsed;
            if(parse.bytes_parsed == 0)
            {
                break;
            }
        }
    }
    return root;
}

MD_FUNCTION_IMPL MD_Node *
MD_ParseWholeFile(MD_String8 filename)
{
    return MD_ParseWholeString(filename, MD_LoadEntireFile(filename));
}

MD_FUNCTION_IMPL MD_b32
MD_NodeIsNil(MD_Node *node)
{
    return node == 0 || node == &_md_nil_node || node->kind == MD_NodeKind_Nil;
}

MD_FUNCTION_IMPL MD_Node *
MD_NilNode(void) { return &_md_nil_node; }

MD_FUNCTION_IMPL MD_Node *
MD_MakeNodeFromToken(MD_NodeKind kind, MD_String8 filename, MD_u8 *file, MD_u8 *at, MD_Token token)
{
    return _MD_MakeNode(kind, token.string, token.outer_string, filename, file, at);
}

MD_FUNCTION_IMPL MD_Node *
MD_MakeNodeFromString(MD_NodeKind kind, MD_String8 filename, MD_u8 *file, MD_u8 *at, MD_String8 string)
{
    return _MD_MakeNode(kind, string, string, filename, file, at);
}

MD_FUNCTION_IMPL void
MD_PushSibling(MD_Node **first, MD_Node **last, MD_Node *parent, MD_Node *new_sibling)
{
    _MD_PushNodeToList(first, last, parent, new_sibling);
}

MD_FUNCTION_IMPL void
MD_PushChild(MD_Node *parent, MD_Node *new_child)
{
    _MD_PushNodeToList(&parent->first_child, &parent->last_child, parent, new_child);
}

MD_FUNCTION_IMPL void
MD_PushTag(MD_Node *node, MD_Node *tag)
{
    _MD_PushNodeToList(&node->first_tag, &node->last_tag, node, tag);
}

MD_FUNCTION_IMPL MD_Node *
MD_NodeFromString(MD_Node *first, MD_Node *last, MD_String8 string)
{
    MD_Node *result = MD_NilNode();
    for(MD_Node *node = first; !MD_NodeIsNil(node); node = node->next)
    {
        if(MD_StringMatch(string, node->string, 0))
        {
            result = node;
            break;
        }
    }
    return result;
}

MD_FUNCTION_IMPL MD_Node *
MD_NodeFromIndex(MD_Node *first, MD_Node *last, int n)
{
    MD_Node *result = MD_NilNode();
    if(n >= 0)
    {
        int idx = 0;
        for(MD_Node *node = first; !MD_NodeIsNil(node); node = node->next, idx += 1)
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

MD_FUNCTION_IMPL int
MD_IndexFromNode(MD_Node *node)
{
    int idx = 0;
    if(node && !MD_NodeIsNil(node))
    {
        for(MD_Node *last = node->prev; last; last = last->prev, idx += 1);
    }
    return idx;
}

MD_FUNCTION_IMPL MD_Node *
MD_NextNodeSibling(MD_Node *last, MD_String8 string)
{
    MD_Node *result = MD_NilNode();
    if(last)
    {
        for(MD_Node *node = last->next; node; node = node->next)
        {
            if(MD_StringMatch(string, node->string, 0))
            {
                result = node;
                break;
            }
        }
    }
    return result;
}

MD_FUNCTION_IMPL MD_Node *
MD_ChildFromString(MD_Node *node, MD_String8 child_string)
{
    return MD_NodeFromString(node->first_child, node->last_child, child_string);
}

MD_FUNCTION_IMPL MD_Node *
MD_TagFromString(MD_Node *node, MD_String8 tag_string)
{
    return MD_NodeFromString(node->first_tag, node->last_tag, tag_string);
}

MD_FUNCTION_IMPL MD_Node *
MD_ChildFromIndex(MD_Node *node, int n)
{
    return MD_NodeFromIndex(node->first_child, node->last_child, n);
}

MD_FUNCTION_IMPL MD_Node *
MD_TagFromIndex(MD_Node *node, int n)
{
    return MD_NodeFromIndex(node->first_child, node->last_child, n);
}

MD_FUNCTION_IMPL MD_Node *
MD_TagArgFromIndex(MD_Node *node, MD_String8 tag_string, int n)
{
    MD_Node *tag = MD_TagFromString(node, tag_string);
    return MD_ChildFromIndex(tag, n);
}

MD_FUNCTION_IMPL MD_b32
MD_NodeHasTag(MD_Node *node, MD_String8 tag_string)
{
    return !MD_NodeIsNil(MD_TagFromString(node, tag_string));
}

static MD_Expr _md_nil_expr = {0};

MD_FUNCTION_IMPL MD_Expr *
MD_NilExpr(void)
{
    return &_md_nil_expr;
}

MD_FUNCTION_IMPL MD_Expr *
MD_MakeExpr(MD_Node *node, MD_ExprKind kind, MD_Expr *left, MD_Expr *right)
{
    MD_Expr *expr = (MD_Expr*)calloc(1, sizeof(*expr));
    if(expr)
    {
        expr->node = node;
        expr->kind = kind;
        expr->sub[0] = left;
        expr->sub[1] = right;
    }
    else
    {
        expr = MD_NilExpr();
    }
    return expr;
}

typedef struct _MD_NodeParseCtx _MD_NodeParseCtx;
struct _MD_NodeParseCtx
{
    MD_Node *at;
};

MD_PRIVATE_FUNCTION_IMPL MD_b32
_MD_RequireNodeKind(_MD_NodeParseCtx *ctx, MD_NodeKind kind, MD_Node **out)
{
    MD_b32 result = 0;
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

MD_PRIVATE_FUNCTION_IMPL MD_b32
_MD_RequireNode(_MD_NodeParseCtx *ctx, MD_String8 string)
{
    MD_b32 result = 0;
    if(ctx->at && MD_StringMatch(string, ctx->at->string, 0))
    {
        result = 1;
        ctx->at = ctx->at->next;
    }
    return result;
}

MD_FUNCTION_IMPL MD_Expr *
MD_ParseAsExpr(MD_Node *node)
{
    MD_Expr *expr = MD_NilExpr();
    
    return expr;
}

MD_FUNCTION_IMPL void
MD_OutputTree(FILE *file, MD_Node *node)
{
    for(MD_Node *tag = node->first_tag; !MD_NodeIsNil(tag); tag = tag->next)
    {
        fprintf(file, "@%.*s", MD_StringExpand(tag->string));
        if(!MD_NodeIsNil(tag->first_child))
        {
            fprintf(file, "(");
            for(MD_Node *child = tag->first_child; !MD_NodeIsNil(child); child = child->next)
            {
                MD_OutputTree(file, child);
                fprintf(file, ", ");
            }
            fprintf(file, ")\n");
        }
        else
        {
            fprintf(file, " ");
        }
    }
    
    fprintf(file, "%.*s", MD_StringExpand(node->whole_string));
    if(!MD_NodeIsNil(node->first_child))
    {
        fprintf(file, ":\n{\n");
        for(MD_Node *child = node->first_child; !MD_NodeIsNil(child); child = child->next)
        {
            MD_OutputTree(file, child);
            fprintf(file, ",\n");
        }
        fprintf(file, "}\n");
    }
    else
    {
        fprintf(file, " ");
    }
}

MD_FUNCTION_IMPL void
MD_OutputExpr(FILE *file, MD_Expr *expr)
{
    
}

MD_FUNCTION_IMPL void
MD_OutputTree_C_String(FILE *file, MD_Node *node)
{
    fprintf(file, "\"");
    for(MD_u64 i = 0; i < node->string.size; i += 1)
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

MD_FUNCTION_IMPL void
MD_OutputTree_C_Struct(FILE *file, MD_Node *node)
{
    if(node)
    {
        fprintf(file, "typedef struct %.*s %.*s;\n",
                MD_StringExpand(node->string),
                MD_StringExpand(node->string));
        fprintf(file, "struct %.*s\n{\n", MD_StringExpand(node->string));
        for(MD_Node *child = node->first_child; !MD_NodeIsNil(child); child = child->next)
        {
            MD_OutputTree_C_Decl(file, child);
            fprintf(file, ";\n");
        }
        fprintf(file, "};\n\n");
    }
}

MD_FUNCTION_IMPL void
MD_OutputTree_C_Decl(FILE *file, MD_Node *node)
{
    if(node)
    {
        // TODO(allen): MD_ParseAsType?
        MD_Expr *type = MD_ParseAsExpr(node);
        MD_Output_C_DeclByNameAndType(file, node->string, type);
    }
}

MD_FUNCTION_IMPL void
MD_Output_C_DeclByNameAndType(FILE *file, MD_String8 name, MD_Expr *type)
{
    MD_OutputType_C_LHS(file, type);
    fprintf(file, " %.*s", MD_StringExpand(name));
    MD_OutputType_C_RHS(file, type);
}

MD_PRIVATE_FUNCTION_IMPL MD_b32
_MD_OutputType_C_NeedsParens(MD_Expr *type)
{
    MD_b32 result = 0;
    if (type->kind == MD_ExprKind_Pointer &&
        type->sub[0]->kind == MD_ExprKind_Array)
    {
        result = 1;
    }
    return(result);
}

MD_FUNCTION_IMPL void
MD_OutputType_C_LHS(FILE *file, MD_Expr *type)
{
    switch (type->kind)
    {
        case MD_ExprKind_Atom:
        {
            MD_Node *node = type->node;
            fprintf(file, "%.*s", MD_StringExpand(node->whole_string));
        }break;
        
        case MD_ExprKind_Pointer:
        {
            MD_OutputType_C_LHS(file, type->sub[0]);
            if (_MD_OutputType_C_NeedsParens(type))
            {
                fprintf(file, "(");
            }
            fprintf(file, "*");
        }break;
        
        case MD_ExprKind_Array:
        {
            MD_OutputType_C_LHS(file, type->sub[0]);
            if (_MD_OutputType_C_NeedsParens(type))
            {
                fprintf(file, "(");
            }
        }break;
        
        default:
        {
            fprintf(file, "{ unexpected MD_ExprKind in type info }");
        }break;
    }
}

MD_FUNCTION_IMPL void
MD_OutputType_C_RHS(FILE *file, MD_Expr *type)
{
    switch (type->kind)
    {
        case MD_ExprKind_Atom:
        {}break;
        
        case MD_ExprKind_Pointer:
        {
            if (_MD_OutputType_C_NeedsParens(type))
            {
                fprintf(file, ")");
            }
            MD_OutputType_C_RHS(file, type->sub[0]);
        }break;
        
        case MD_ExprKind_Array:
        {
            if (_MD_OutputType_C_NeedsParens(type))
            {
                fprintf(file, ")");
            }
            fprintf(file, "[");
            // TODO(allen): MD_OutputExpr_C(file type->sub[1]);
            fprintf(file, "\"C expressions not implemented\"");
            fprintf(file, "]");
            MD_OutputType_C_RHS(file, type->sub[0]);
        }break;
        
        default:
        {}break;
    }
}

MD_FUNCTION_IMPL MD_CommandLine
MD_CommandLine_Start(int argument_count, char **arguments)
{
    MD_CommandLine cmdln = {0};
    cmdln.arguments = (MD_String8*)calloc(sizeof(MD_String8), argument_count-1);
    for(int i = 1; i < argument_count; i += 1)
    {
        cmdln.arguments[i-1] = MD_PushStringF("%s", arguments[i]);
    }
    cmdln.argument_count = argument_count-1;
    return cmdln;
}

MD_FUNCTION_IMPL MD_b32
MD_CommandLine_Flag(MD_CommandLine *cmdln, MD_String8 string)
{
    MD_b32 result = 0;
    for(int i = 0; i < cmdln->argument_count; i += 1)
    {
        if(MD_StringMatch(string, cmdln->arguments[i], 0))
        {
            result = 1;
            cmdln->arguments[i].str = 0;
            cmdln->arguments[i].size = 0;
            break;
        }
    }
    return result;
}

MD_FUNCTION_IMPL MD_b32
MD_CommandLine_FlagStrings(MD_CommandLine *cmdln, MD_String8 string, int out_count, MD_String8 *out)
{
    MD_b32 result = 0;
    for(int i = 0; i < cmdln->argument_count; i += 1)
    {
        if(MD_StringMatch(string, cmdln->arguments[i], 0))
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

MD_FUNCTION_IMPL MD_b32
MD_CommandLine_FlagIntegers(MD_CommandLine *cmdln, MD_String8 string, int out_count, MD_i64 *out)
{
    MD_b32 result = 0;
    for(int i = 0; i < cmdln->argument_count; i += 1)
    {
        if(MD_StringMatch(string, cmdln->arguments[i], 0))
        {
            cmdln->arguments[i].str = 0;
            cmdln->arguments[i].size = 0;
            if(cmdln->argument_count > i + out_count)
            {
                for(int out_idx = 0; out_idx < out_count; out_idx += 1)
                {
                    out[out_idx] = (MD_i64)MD_IntFromString(cmdln->arguments[i+out_idx+1]);
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

MD_FUNCTION_IMPL MD_b32
MD_CommandLine_FlagString(MD_CommandLine *cmdln, MD_String8 string, MD_String8 *out)
{
    return MD_CommandLine_FlagStrings(cmdln, string, 1, out);
}

MD_FUNCTION_IMPL MD_b32
MD_CommandLine_FlagInteger(MD_CommandLine *cmdln, MD_String8 string, MD_i64 *out)
{
    return MD_CommandLine_FlagIntegers(cmdln, string, 1, out);
}

MD_FUNCTION_IMPL MD_b32
MD_CommandLine_Increment(MD_CommandLine *cmdln, MD_String8 **string_ptr)
{
    MD_b32 result = 0;
    MD_String8 *string = *string_ptr;
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

MD_FUNCTION_IMPL MD_String8
MD_LoadEntireFile(MD_String8 filename)
{
    MD_String8 file_contents = {0};
    FILE *file = fopen((char*)MD_PushStringCopy(filename).str, "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        MD_u64 file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        file_contents.str = (MD_u8*)calloc(1, file_size+1);
        if(file_contents.str)
        {
            file_contents.size = file_size;
            fread(file_contents.str, 1, file_size, file);
        }
        fclose(file);
    }
    return file_contents;
}

MD_FUNCTION_IMPL MD_b32
MD_FileIter_Increment(MD_FileIter *it, MD_String8 path, MD_FileInfo *out_info)
{
    return _MD_OS_IMPL_FileIter_Increment(it, path, out_info);
}