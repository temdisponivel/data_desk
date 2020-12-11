#include "dd.h"
#include "dd.c"

int main(int argument_count, char **arguments)
{
#if 0
    // NOTE(rjf): Parse all the files passed in via command line.
    DD_ParseCtx ctx = DD_Parse_Begin();
    for(int i = 1; i < argument_count; i += 1)
    {
        DD_Parse_Filename(&ctx, DD_S8CString(arguments[i]));
    }
    DD_ParseResult result = DD_Parse_End(&ctx);
    
    // NOTE(rjf): Print out parsed results.
    DD_NodeListLoop(root, result.roots)
    {
        DD_NodeListLoop(child, root->children)
        {
            if(!DD_NodeIsNil(DD_TagOnNode(child, DD_S8Lit("struct"))))
            {
                DD_OutputTree_C_Struct(stderr, child);
            }
        }
    }
#endif
    
    return 0;
}