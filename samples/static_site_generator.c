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
Generate(DD_NodeTable *index_table, PageInfo *info, FILE *file, DD_Node *node)
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
            
            DD_String8 splits[] =
            {
                DD_S8Lit("\n\n"),
            };
            DD_String8List strlist = DD_SplitString(node->string, sizeof(splits)/sizeof(splits[0]), splits);
            
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
                    if(child->children.first == 0)
                    {
                        fprintf(file, "<li>\n");
                    }
                    Generate(index_table, info, file, child);
                    if(child->children.first == 0)
                    {
                        fprintf(file, "</li>\n");
                    }
                }
                fprintf(file, "</ul>\n");
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("img")))
            {
                DD_Node *src = 0;
                DD_Node *alt = 0;
                if(DD_RequireNodeChild(node, 0, &src))
                {
                    DD_RequireNodeChild(node, 0, &alt);
                    fprintf(file, "<img src=\"%.*s\">\n", DD_StringExpand(src->string));
                    fprintf(file, "</img>\n");
                }
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("youtube")))
            {
                DD_Node *id = 0;
                if(DD_RequireNodeChild(node, 0, &id))
                {
                    fprintf(file, "<iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/%.*s\" frameborder=\"0\" allow=\"accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture\" allowfullscreen></iframe>\n",
                            DD_StringExpand(id->string));
                }
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("lister")))
            {
                fprintf(file, "<ul>\n");
                DD_Node *index_string = 0;
                for(DD_u64 idx = 0; DD_RequireNodeChild(node, idx, &index_string); idx += 1)
                {
                    for(DD_NodeTableSlot *slot = DD_NodeTable_Lookup(index_table, index_string->string);
                        slot; slot = slot->next)
                    {
                        if(slot->node)
                        {
                            fprintf(file, "<li>");
                            fprintf(file, "<a href=\"#\">%.*s</a>", DD_StringExpand(slot->node->file));
                            fprintf(file, "</li>");
                        }
                    }
                }
                fprintf(file, "</ul>\n");
            }
        }break;
        
        default: break;
    }
}

int main(int argument_count, char **arguments)
{
    //~ NOTE(rjf): Do parse.
    DD_ParseCtx ctx = DD_Parse_Begin();
    for(int i = 1; i < argument_count; i += 1)
    {
        DD_Parse_Filename(&ctx, DD_S8CString(arguments[i]));
    }
    DD_ParseResult parse = DD_Parse_End(&ctx);
    
    //~ NOTE(rjf): Generate index table.
    DD_NodeTable index_table = {0};
    for(DD_Node *root = parse.roots.first; root; root = root->next)
    {
        for(DD_Node *node = root->children.first; node; node = node->next)
        {
            if(node->kind == DD_NodeKind_Set && node->children.first &&
               DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("index")))
            {
                for(DD_Node *index_string = node->children.first; index_string; index_string = index_string->next)
                {
                    DD_NodeTable_Insert(&index_table, DD_NodeTableCollisionRule_Chain, index_string->string, root);
                }
                goto end_index_build;
            }
        }
        end_index_build:;
    }
    
    //~ NOTE(rjf): Generate files for all roots.
    for(DD_Node *root = parse.roots.first; root; root = root->next)
    {
        PageInfo info = {0};
        for(DD_Node *node = root->children.first; node; node = node->next)
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
        
        DD_String8 name_without_extension = DD_WithoutExtension(root->file);
        FILE *file = fopen(DD_PushCStringF("%.*s.html", DD_StringExpand(name_without_extension)), "w");
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
            
            for(DD_Node *node = root->children.first; node; node = node->next)
            {
                Generate(&index_table, &info, file, node);
            }
            fprintf(file, "</body>\n");
            fprintf(file, "</html>\n");
            fclose(file);
        }
    }
    
    return 0;
}