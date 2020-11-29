#include "dd.h"
#include "dd.c"

typedef struct SiteInfo SiteInfo;
struct SiteInfo
{
    DD_Node *title;
    DD_Node *desc;
    DD_Node *canonical_url;
    DD_Node *author;
    DD_Node *twitter_handle;
    DD_Node *header;
    DD_Node *footer;
    DD_Node *style;
};

typedef struct PageInfo PageInfo;
struct PageInfo
{
    DD_Node *title;
    DD_Node *desc;
    DD_Node *date;
    DD_Node *parent;
};

static PageInfo ParsePageInfo(DD_Node *page);
static SiteInfo ParseSiteInfo(DD_Node *site);
static void GeneratePageContent(DD_NodeTable *index_table, SiteInfo *site_info, PageInfo *page_info, FILE *file, DD_Node *node);

int main(int argument_count, char **arguments)
{
    //~ NOTE(rjf): Parse command line arguments.
    DD_String8 site_info_path = {0};
    DD_CommandLine cmdln = DD_CommandLine_Start(argument_count, arguments);
    if(!DD_CommandLine_FlagString(&cmdln, DD_S8Lit("--siteinfo"), &site_info_path))
    {
        fprintf(stderr, "USAGE: %s --siteinfo <path to site info file> <site file 1> <site file 2> ...\n", arguments[0]);
        goto end;
    }
    
    //~ NOTE(rjf): Parse site info.
    SiteInfo site_info = {0};
    {
        DD_ParseCtx ctx = DD_Parse_Begin();
        DD_Parse_Filename(&ctx, site_info_path);
        DD_ParseResult parse = DD_Parse_End(&ctx);
        site_info = ParseSiteInfo(parse.roots.first);
    }
    
    //~ NOTE(rjf): Parse pages.
    DD_ParseResult parse = {0};
    {
        DD_ParseCtx ctx = DD_Parse_Begin();
        for(DD_String8 *filename = 0; DD_CommandLine_Increment(&cmdln, &filename);)
        {
            DD_Parse_Filename(&ctx, *filename);
        }
        parse = DD_Parse_End(&ctx);
    }
    
    //~ NOTE(rjf): Generate index table.
    DD_NodeTable index_table = {0};
    {
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
    }
    
    //~ NOTE(rjf): Generate stylesheet.
    if(site_info.style)
    {
        FILE *file = fopen("style.css", "w");
        if(file)
        {
            fprintf(file, "%.*s", DD_StringExpand(site_info.style->string));
            fclose(file);
        }
    }
    
    //~ NOTE(rjf): Generate files for all roots.
    for(DD_Node *root = parse.roots.first; root; root = root->next)
    {
        PageInfo page_info = ParsePageInfo(root);
        
        DD_String8 name_without_extension = DD_WithoutFolder(DD_WithoutExtension(root->file));
        FILE *file = fopen(DD_PushCStringF("%.*s.html", DD_StringExpand(name_without_extension)), "w");
        if(file)
        {
            fprintf(file, "<!DOCTYPE html>\n");
            fprintf(file, "<html lang=\"en\">\n");
            
            DD_String8 site_title      = site_info.title ? site_info.title->string : DD_ZeroString();
            DD_String8 title           = page_info.title ? page_info.title->string : DD_ZeroString();
            DD_String8 url             = site_info.canonical_url ? site_info.canonical_url->string : DD_ZeroString();
            DD_String8 author          = site_info.author ? site_info.author->string : DD_ZeroString();
            DD_String8 twitter_handle  = site_info.twitter_handle ? site_info.twitter_handle->string : DD_ZeroString();
            
            // NOTE(rjf): Generate heading.
            {
                fprintf(file, "<head>\n");
                fprintf(file, "<meta charset=\"utf-8\">\n");
                fprintf(file, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta name=\"author\" content=\"%.*s\">\n", DD_StringExpand(author));
                fprintf(file, "<meta property=\"og:title\" content=\"%.*s\">\n", DD_StringExpand(title));
                fprintf(file, "<meta name=\"twitter:title\" content=\"%.*s\">\n", DD_StringExpand(title));
                fprintf(file, "<link rel=\"canonical\" href=\"%.*s\">\n", DD_StringExpand(url));
                fprintf(file, "<meta property=\"og:type\" content=\"website\">\n");
                fprintf(file, "<meta property=\"og:url\" content=\"%.*s\">\n", DD_StringExpand(url));
                fprintf(file, "<meta property=\"og:site_name\" content=\"%.*s\">\n", DD_StringExpand(site_title));
                fprintf(file, "<meta name=\"twitter:card\" content=\"summary\">\n");
                fprintf(file, "<meta name=\"twitter:site\" content=\"%.*s\">\n", DD_StringExpand(twitter_handle));
                fprintf(file, "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n");
                if(!DD_StringIsZero(title))
                {
                    if(!DD_StringIsZero(site_title))
                    {
                        fprintf(file, "<title>%.*s | %.*s</title>\n", DD_StringExpand(title), DD_StringExpand(site_title));
                    }
                    else
                    {
                        fprintf(file, "<title>%.*s</title>\n", DD_StringExpand(title));
                    }
                }
                else if(!DD_StringIsZero(site_title))
                {
                    fprintf(file, "<title>%.*s</title>\n", DD_StringExpand(site_title));
                }
                fprintf(file, "</head>\n");
            }
            
            // NOTE(rjf): Generate body.
            {
                fprintf(file, "<body>\n");
                
                // NOTE(rjf): Generate header.
                if(site_info.header)
                {
                    fprintf(file, "%.*s", DD_StringExpand(site_info.header->string));
                }
                
                // NOTE(rjf): Parent page back button.
                if(page_info.parent)
                {
                    fprintf(file, "<a href=\"%.*s.html\">← Back</a>", DD_StringExpand(page_info.parent->string));
                }
                
                // NOTE(rjf): Title.
                if(!DD_StringIsZero(title))
                {
                    fprintf(file, "<h1 class=\"title\">%.*s</h1>", DD_StringExpand(title));
                }
                
                // NOTE(rjf): Main description/subtitle.
                if(page_info.desc)
                {
                    fprintf(file, "<h2 class=\"subtitle\">%.*s</h2>", DD_StringExpand(page_info.desc->string));
                }
                
                // NOTE(rjf): Date.
                if(page_info.date)
                {
                    DD_Node *year = 0;
                    DD_Node *month = 0;
                    DD_Node *day = 0;
                    
                    for(DD_Node *child = page_info.date->children.first; child; child = child->next)
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
                            fprintf(file, "<h3 class=\"date\">%.*s %s %.*s</h3>",
                                    DD_StringExpand(day->string),
                                    month_names[month_idx],
                                    DD_StringExpand(year->string));
                        }
                    }
                }
                
                // NOTE(rjf): The rest of the page content should be generated from the page nodes.
                for(DD_Node *node = root->children.first; node; node = node->next)
                {
                    GeneratePageContent(&index_table, &site_info, &page_info, file, node);
                }
                
                // NOTE(rjf): Generate footer.
                if(site_info.footer)
                {
                    fprintf(file, "%.*s", DD_StringExpand(site_info.footer->string));
                }
                
                fprintf(file, "</body>\n");
            }
            
            fprintf(file, "</html>\n");
            fclose(file);
        }
    }
    
    end:;
    return 0;
}

static PageInfo
ParsePageInfo(DD_Node *page)
{
    PageInfo info = {0};
    for(DD_Node *node = page->children.first; node; node = node->next)
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
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("parent")))
            {
                info.parent = node->children.first;
            }
        }
    }
    return info;
}

static SiteInfo
ParseSiteInfo(DD_Node *site)
{
    SiteInfo info = {0};
    for(DD_Node *node = site->children.first; node; node = node->next)
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
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("canonical_url")))
            {
                info.canonical_url = node->children.first;
            }
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("author")))
            {
                info.author = node->children.first;
            }
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("twitter_handle")))
            {
                info.twitter_handle = node->children.first;
            }
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("header")))
            {
                info.header = node->children.first;
            }
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("footer")))
            {
                info.footer = node->children.first;
            }
            else if(DD_StringMatchCaseInsensitive(node->string, DD_S8Lit("style")))
            {
                info.style = node->children.first;
            }
        }
    }
    return info;
}

static void
GeneratePageContent(DD_NodeTable *index_table, SiteInfo *site_info, PageInfo *page_info, FILE *file, DD_Node *node)
{
    switch(node->kind)
    {
        case DD_NodeKind_StringLiteral:
        case DD_NodeKind_CharLiteral:
        {
            char *html_tag = "p";
            char *style = "paragraph";
            if(DD_NodeHasTag(node, DD_S8Lit("title")))
            {
                html_tag = "h1";
                style = "title";
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("subtitle")))
            {
                html_tag = "h2";
                style = "subtitle";
            }
            else if(DD_NodeHasTag(node, DD_S8Lit("code")))
            {
                html_tag = "pre";
                style = "code";
            }
            
            DD_String8 splits[] =
            {
                DD_S8Lit("\n\n"),
            };
            DD_String8List strlist = DD_SplitString(node->string, sizeof(splits)/sizeof(splits[0]), splits);
            
            for(DD_String8Node *node = strlist.first; node; node = node->next)
            {
                fprintf(file, "<%s class=\"%s\">", html_tag, style);
                fprintf(file, "%.*s", DD_StringExpand(node->string));
                fprintf(file, "</%s>\n", html_tag);
            }
        }break;
        
        case DD_NodeKind_Set:
        {
            if(DD_NodeHasTag(node, DD_S8Lit("list")))
            {
                fprintf(file, "<ul class=\"list\">\n");
                for(DD_Node *child = node->children.first; child; child = child->next)
                {
                    if(child->children.first == 0)
                    {
                        fprintf(file, "<li class=\"list_item\">\n");
                    }
                    GeneratePageContent(index_table, site_info, page_info, file, child);
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
                    fprintf(file, "<img class=\"image\" src=\"%.*s\"></img>", DD_StringExpand(src->string));
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
                fprintf(file, "<input></input>");
                fprintf(file, "<ul class=\"lister\">\n");
                DD_Node *index_string = 0;
                for(DD_u64 idx = 0; DD_RequireNodeChild(node, idx, &index_string); idx += 1)
                {
                    for(DD_NodeTableSlot *slot = DD_NodeTable_Lookup(index_table, index_string->string);
                        slot; slot = slot->next)
                    {
                        if(slot->node)
                        {
                            PageInfo info = ParsePageInfo(slot->node);
                            fprintf(file, "<li class=\"lister_item\">");
                            DD_String8 filename = slot->node->file;
                            DD_String8 filename_no_ext = DD_WithoutExtension(DD_WithoutFolder(filename));
                            DD_String8 link = DD_PushStringF("%.*s.html", DD_StringExpand(filename_no_ext));
                            DD_String8 name = info.title->string;
                            fprintf(file, "<a href=\"%.*s\">%.*s</a>", DD_StringExpand(link), DD_StringExpand(name));
                            fprintf(file, "</li>\n");
                        }
                    }
                }
                fprintf(file, "</ul>\n");
            }
        }break;
        
        default: break;
    }
}
