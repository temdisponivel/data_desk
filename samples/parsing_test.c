#include "dd.h"
#include "dd.c"

int main(int argument_count, char **arguments)
{
    
    // NOTE(rjf): Parse all the files passed in via command line.
    DD_Node *first = 0;
    DD_Node *last = 0;
    for(int i = 1; i < argument_count; i += 1)
    {
        DD_Node *root = DD_ParseWholeFile(DD_S8CString(arguments[i]));
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
    for(DD_Node *root = first; !DD_NodeIsNil(root); root = root->next)
    {
        DD_OutputTree(stdout, root);
    }
    
    return 0;
}