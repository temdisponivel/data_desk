// NOTE(rjf): Platform-Specific
#if BUILD_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif BUILD_LINUX
#include <dlfcn.h>
#endif

// NOTE(rjf): C Runtime Library
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// NOTE(rjf): Data Desk Code
#include "data_desk.h"
#include "data_desk_utilities.c"
#include "data_desk_debug.c"
#include "data_desk_tokenizer.c"
#include "data_desk_custom.c"
#include "data_desk_parse.c"
#include "data_desk_ast_traverse.c"

static void
ProcessFile(DataDeskCustom custom, char *file, char *filename)
{
    Tokenizer tokenizer = {0};
    {
        tokenizer.at = file;
        tokenizer.filename = filename;
        tokenizer.line = 1;
    }
    
    ParseContext context = {0};
    
    ASTNode *root = ParseCode(&tokenizer, &context);
    PatchASTSymbols(&context, root);
    GenerateNullTerminatedStringsForAST(&context, root);
    if(custom.FileCallback)
    {
        custom.FileCallback(root, filename);
    }
    TraverseASTAndCallCustomParseCallbacks(&context, root, custom, filename);
    
    for(int i = 0; i < context.error_stack_size; ++i)
    {
        fprintf(stderr,
                "ERROR (%s:%i): %s\n",
                context.error_stack[i].file,
                context.error_stack[i].line,
                context.error_stack[i].string);
    }
    
    // NOTE(rjf): ParseContextCleanUp shouldn't be called, because often time, code
    // will depend on ASTs persisting between files (which should totally work).
    // So, we won't clean up anything, and let the operating system do it ond
    // program exit. We can still let a ParseContext go out of scope, though,
    // because the memory will stay allocated. Usually, this is called a leak and
    // thus harmful, but because we want memory for all ASTs ever parsed within
    // a given runtime to persist (because we store pointers to them, etc.), we
    // actually don't care; this is /exactly/ what we want. The operating system
    // frees the memory on exit, and for this reason, there is literally no reason
    // to care about AST clean-up at all.
    // ParseContextCleanUp(&context);
    
    // NOTE(rjf): This is a reason why non-nuanced and non-context-specific programming
    // rules suck.
}

int
main(int argument_count, char **arguments)
{
    if(argument_count > 1)
    {
        if(StringMatchCaseInsensitive(arguments[1], "help"  ) ||
           StringMatchCaseInsensitive(arguments[1], "-help" ) ||
           StringMatchCaseInsensitive(arguments[1], "--help") ||
           StringMatchCaseInsensitive(arguments[1], "-h"    ) ||
           StringMatchCaseInsensitive(arguments[1], "--h"   ) ||
           StringMatchCaseInsensitive(arguments[1], "/?"    ) ||
           StringMatchCaseInsensitive(arguments[1], "?"     ) ||
           StringMatchCaseInsensitive(arguments[1], "-?"    ))
        {
            printf("Data Desk Flags\n");
            printf("--custom, -c        Specify the path to a custom layer to which parse information is to be sent.\n");
            printf("--log, -l           Enable logging.\n");
        }
        else
        {
            
            Log("Data Desk v0.1");
            
            DataDeskCustom custom = {0};
            char *custom_layer_dll_path = 0;
            
            // NOTE(rjf): Load command line arguments and set all non-file arguments
            // to zero, so that we know the arguments to process in the file-processing
            // loop.
            {
                int argument_read_mode = 0;
                enum
                {
                    ARGUMENT_READ_MODE_files,
                    ARGUMENT_READ_MODE_custom_layer_dll,
                };
                
                for(int i = 1; i < argument_count; ++i)
                {
                    if(argument_read_mode == ARGUMENT_READ_MODE_files)
                    {
                        if(StringMatchCaseInsensitive(arguments[i], "-c") ||
                           StringMatchCaseInsensitive(arguments[i], "--custom"))
                        {
                            argument_read_mode = ARGUMENT_READ_MODE_custom_layer_dll;
                            arguments[i] = 0;
                        }
                        else if(StringMatchCaseInsensitive(arguments[i], "-l") ||
                                StringMatchCaseInsensitive(arguments[i], "--log"))
                        {
                            global_log_enabled = 1;
                            arguments[i] = 0;
                        }
                    }
                    else if(argument_read_mode == ARGUMENT_READ_MODE_custom_layer_dll)
                    {
                        custom_layer_dll_path = arguments[i];
                        arguments[i] = 0;
                        argument_read_mode = ARGUMENT_READ_MODE_files;
                    }
                }
            }
            
            // NOTE(rjf): Load custom code DLL if needed.
            if(custom_layer_dll_path)
            {
                Log("Loading custom layer from \"%s\"", custom_layer_dll_path);
                custom = DataDeskCustomLoad(custom_layer_dll_path);
            }
            else
            {
                Log("WARNING: No custom layer loaded");
            }
            
            if(custom.InitCallback)
            {
                custom.InitCallback();
            }
            
            for(int i = 1; i < argument_count; ++i)
            {
                if(arguments[i] != 0)
                {
                    char *filename = arguments[i];
                    Log("Processing file at \"%s\"", filename);
                    char *file = LoadEntireFileAndNullTerminate(filename);
                    if(file)
                    {
                        ProcessFile(custom, file, filename);
                    }
                    else
                    {
                        Log("ERROR: Could not load \"%s\"", filename);
                    }
                }
            }
            
            if(custom.CleanUpCallback)
            {
                custom.CleanUpCallback();
            }
            
            DataDeskCustomUnload(&custom);
        }
    }
    else
    {
        fprintf(stderr, "USAGE: %s [-c|--custom <path to custom layer DLL>] [-l|--log] <files to process>\n",
                arguments[0]);
    }
    
    return 0;
}
