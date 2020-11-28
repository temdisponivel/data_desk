#include "dd.h"
#include "dd.c"

typedef struct PageInfo PageInfo;
struct PageInfo
{
    DD_Node *title;
    DD_Node *desc;
    DD_Node *date;
};

static void
Generate(PageInfo *info, FILE *file, DD_Node *node)
{
    switch(node->kind)
    {
        case DD_NodeKind_StringLiteral:
        case DD_NodeKind_CharLiteral:
        {
            char *html_tag = "p";
            if(DD_NodeHasTag(node, DD_S8Lit("title")))
            {
                html_tag = "h1";
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("subtitle")))
            {
                html_tag = "h2";
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("code")))
            {
                html_tag = "pre";
            }
            DD_String8List strlist = DD_SplitStringByString(node->string, DD_S8Lit("\n\n"));
            for(DD_String8Node *node = strlist.first; node; node = node->next)
            {
                fprintf(file, "<%s>", html_tag);
                fprintf(file, "%.*s", DD_StringExpand(node->string));
                fprintf(file, "</%s>\n", html_tag);
            }
        }break;
        
        case DD_NodeKind_Set:
        {
            if(DD_NodeHasTag(node, DD_S8Lit("list")))
            {
                fprintf(file, "<ul>\n");
                for(DD_Node *child = node->children.first; child; child = child->next)
                {
                    fprintf(file, "<li>\n");
                    Generate(info, file, child);
                    fprintf(file, "</li>\n");
                }
                fprintf(file, "</ul>\n");
            }
        }break;
        
        default: break;
    }
}

int main(int argument_count, char **arguments)
{
    for(int i = 1; i < argument_count; i += 1)
    {
        DD_ParseCtx ctx = DD_Parse_Begin();
        DD_Parse_Filename(&ctx, DD_S8CString(arguments[i]));
        DD_ParseResult parse = DD_Parse_End(&ctx);
        
        PageInfo info = {0};
        {
            for(DD_Node *node = parse.root; node; node = node->next)
            {
                if(node->kind == DD_NodeKind_Set && node->children.first)
                {
                    if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("title")))
                    {
                        info.title = node->children.first;
                    }
                    else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("desc")))
                    {
                        info.desc = node->children.first;
                    }
                    else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("date")))
                    {
                        info.date = node;
                    }
                }
            }
        }
        
        FILE *file = fopen("generated.html", "w");
        if(file)
        {
            fprintf(file, "<!DOCTYPE html>\n");
            fprintf(file, "<html>\n");
            fprintf(file, "<head>\n");
            if(info.title)
            {
                fprintf(file, "<title>%.*s</title>\n", DD_StringExpand(info.title->string));
            }
            fprintf(file, "</head>\n");
            fprintf(file, "<body>\n");
            if(info.title)
            {
                fprintf(file, "<h1>%.*s</h1>", DD_StringExpand(info.title->string));
            }
            if(info.desc)
            {
                fprintf(file, "<h2>%.*s</h2>", DD_StringExpand(info.desc->string));
            }
            if(info.date)
            {
                DD_Node *year = 0;
                DD_Node *month = 0;
                DD_Node *day = 0;
                
                for(DD_Node *child = info.date->children.first; child; child = child->next)
                {
                    if(child->kind == DD_NodeKind_NumericLiteral)
                    {
                        if      (year  == 0) year  = child;
                        else if (month == 0) month = child;
                        else if (day   == 0) day   = child;
                        else
                        {
                            break;
                        }
                    }
                }
                
                if(year && month && day)
                {
                    char *month_names[] =
                    {
                        "January", "February", "March", "April", "May", "June", "July", "August",
                        "September", "October", "November", "December",
                    };
                    int month_idx = DD_IntFromString(month->string)-1;
                    if(month_idx >= 0 && month_idx < sizeof(month_names)/sizeof(month_names[0]))
                    {
                        fprintf(file, "<h3>%.*s %s %.*s</h3>",
                                DD_StringExpand(day->string),
                                month_names[month_idx],
                                DD_StringExpand(year->string));
                    }
                }
            }
            for(DD_Node *node = parse.root; node; node = node->next)
            {
                Generate(&info, file, node);
            }
            fprintf(file, "</body>\n");
            fprintf(file, "</html>\n");
            fclose(file);
        }
    }
    return 0;
}