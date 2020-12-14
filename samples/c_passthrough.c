#include "md.h"
#include "md.c"

int main(int argument_count, char **arguments)
{
#if 0
    // NOTE(rjf): Parse all the files passed in via command line.
    MD_ParseCtx ctx = MD_Parse_Begin();
    for(int i = 1; i < argument_count; i += 1)
    {
        MD_Parse_Filename(&ctx, MD_S8CString(arguments[i]));
    }
    MD_ParseResult result = MD_Parse_End(&ctx);
    
    // NOTE(rjf): Print out parsed results.
    MD_NodeListLoop(root, result.roots)
    {
        MD_NodeListLoop(child, root->children)
        {
            if(!MD_NodeIsNil(MD_TagOnNode(child, MD_S8Lit("struct"))))
            {
                MD_OutputTree_C_Struct(stderr, child);
            }
        }
    }
#endif
    
    return 0;
}