#include "md.h"
#include "md.c"

static void
Initialize(void)
{
}

static void
TopLevel(MD_Node *node)
{
}

static void
CleanUp(void)
{
}

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
    
    // NOTE(rjf): Call "custom layer" back.
    // TODO(rjf): Clean this up
    Initialize();
    for(MD_Node *root = first; !MD_NodeIsNil(root); root = root->next)
    {
        for(MD_Node *node = root->first_child; !MD_NodeIsNil(node); node = node->next)
        {
            TopLevel(node);
        }
    }
    CleanUp();
    
    return 0;
}