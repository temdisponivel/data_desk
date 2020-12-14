#include "md.h"
#include "md.c"

int main(int argument_count, char **arguments)
{
    
    // NOTE(rjf): Parse all the files passed in via command line.
    MD_Node *first = 0;
    MD_Node *last = 0;
    for(int i = 1; i < argument_count; i += 1)
    {
        MD_Node *root = MD_ParseWholeFile(MD_S8CString(arguments[i]));
        // TODO(rjf): Clean this use-case up
        if(last == 0)
        {
            first = last = root;
        }
        else
        {
            last->next = root;
            last = last->next;
        }
    }
    
    // NOTE(rjf): Print out parsed results.
    for(MD_Node *root = first; !MD_NodeIsNil(root); root = root->next)
    {
        MD_OutputTree(stdout, root);
    }
    
    return 0;
}