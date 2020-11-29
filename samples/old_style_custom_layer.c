#include "dd.h"
#include "dd.c"

static void
Initialize(void)
{
}

static void
TopLevel(DD_Node *node)
{
}

static void
CleanUp(void)
{
}

int main(int argument_count, char **arguments)
{
    
    // NOTE(rjf): Parse all the files passed in via command line.
    DD_ParseCtx ctx = DD_Parse_Begin();
    for(int i = 1; i < argument_count; i += 1)
    {
        DD_Parse_Filename(&ctx, DD_S8CString(arguments[i]));
    }
    DD_ParseResult result = DD_Parse_End(&ctx);
    
    // NOTE(rjf): Call "custom layer" back.
    Initialize();
    for(DD_Node *node = result.root->children.first; node; node = node->next)
    {
        TopLevel(node);
    }
    CleanUp();
    
    return 0;
}