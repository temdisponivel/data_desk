// NOTE(rjf): Utility program to generate docs pages for Data Desk

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define ENABLE_LOG_BY_DEFAULT 1

#include "data_desk.h"
#include "data_desk_debug.c"
#include "data_desk_utilities.c"
#include "data_desk_tokenizer.c"

typedef enum CTypeDecoratorType
{
    CTypeDecoratorType_None,
    CTypeDecoratorType_Pointer,
    CTypeDecoratorType_Array,
}
CTypeDecoratorType;

typedef struct CType CType;
struct CType
{
    Token string;
    CType *operand;
    CTypeDecoratorType decorator;
};

typedef enum DocsType
{
    DocsType_Null,
    DocsType_Callback,
    DocsType_Procedure,
    DocsType_Parameter,
    DocsType_NodeType,
    DocsType_NodeMember,
}
DocsType;

typedef struct Docs Docs;
struct Docs
{
    DocsType type;
    Token name;
    Token description;
    CType *type_information;
    Docs *next;
};

typedef struct DocsList DocsList;
struct DocsList
{
    Docs *head;
    Docs *tail;
};

static void
AppendDocs(DocsList *list, Docs *docs)
{
    if(list->tail == 0)
    {
        list->head = list->tail = docs;
    }
    else
    {
        list->tail->next = docs;
        list->tail = docs;
    }
}

static void
AppendDocsList(DocsList *list, DocsList *new_list)
{
    if(list->tail == 0)
    {
        list->head = new_list->head;
        list->tail = new_list->tail;
    }
    else
    {
        list->tail->next = new_list->head;
        list->tail = new_list->tail;
    }
}

static DocsList
ParseFile(MemoryArena *arena, char *file, char *filename)
{
    Tokenizer tokenizer_ = {0};
    Tokenizer *tokenizer = &tokenizer_;
    {
        tokenizer->at = file;
        tokenizer->filename = filename;
        tokenizer->line = 1;
    }
    
    DocsList docs_list = {0};
    
    for(;;)
    {
        // NOTE(rjf): Break if there are no tokens left.
        {
            Token token = PeekToken(tokenizer);
            if(!token.type || !token.string)
            {
                break;
            }
        }
        
        DocsType docs_type = DocsType_Null;
        if(RequireToken(tokenizer, "DataDeskDoc_Callback", 0))
        {
            docs_type = DocsType_Callback;
        }
        else if(RequireToken(tokenizer, "DataDeskDoc_Procedure", 0))
        {
            docs_type = DocsType_Procedure;
        }
        else if(RequireToken(tokenizer, "DataDeskDoc_Parameter", 0))
        {
            docs_type = DocsType_Parameter;
        }
        else if(RequireToken(tokenizer, "DataDeskDoc_NodeType", 0))
        {
            docs_type = DocsType_NodeType;
        }
        else if(RequireToken(tokenizer, "DataDeskDoc_NodeMember", 0))
        {
            docs_type = DocsType_NodeMember;
        }
        
        if(docs_type)
        {
            Token docs_string = {0};
            if(RequireToken(tokenizer, "(", 0) &&
               RequireTokenType(tokenizer, Token_StringConstant, &docs_string) &&
               RequireToken(tokenizer, ")", 0))
            {
                Docs *docs = MemoryArenaAllocate(arena, sizeof(*docs));
                docs->type = docs_type;
                docs->description = docs_string;
                AppendDocs(&docs_list, docs);
                
                printf("%.*s\n\n", docs->description.string_length, docs->description.string);
                
                // TODO(rjf): Parse out name + type info
            }
        }
        
        NextToken(tokenizer);
    }
    
    return docs_list;
}

int main(int argument_count, char **arguments)
{
    if(argument_count > 1)
    {
        MemoryArena arena = {0};
        DocsList docs_list = {0};
        
        for(int argument_index = 1; argument_index < argument_count; ++argument_index)
        {
            Log("Loading file \"%s\".", arguments[argument_index]);
            char *file = LoadEntireFileAndNullTerminate(arguments[argument_index]);
            DocsList new_docs_list = ParseFile(&arena, file, arguments[argument_index]);
            AppendDocsList(&docs_list, &new_docs_list);
        }
    }
    else
    {
        LogError("USAGE: %s <files to process>", arguments[0]);
    }
    
    return 0;
}