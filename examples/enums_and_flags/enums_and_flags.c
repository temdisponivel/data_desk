#include "data_desk.h"

FILE *global_generation_file;

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    global_generation_file = fopen("generated_enums_and_flags.c", "w");
}

DATA_DESK_FUNC void
DataDeskCustomFileCallback(DataDeskASTNode *root, char *filename) {}

DATA_DESK_FUNC void
DataDeskCustomConstantCallback(DataDeskConstant constant_info, char *filename) {}

DATA_DESK_FUNC void
DataDeskCustomStructCallback(DataDeskStruct struct_info, char *filename) {}

DATA_DESK_FUNC void
DataDeskCustomEnumCallback(DataDeskEnum enum_info, char *filename)
{
    DataDeskFWriteASTFromRootAsC(global_generation_file, enum_info.root, 0);
}

DATA_DESK_FUNC void
DataDeskCustomFlagsCallback(DataDeskFlags flags_info, char *filename)
{
    DataDeskFWriteASTFromRootAsC(global_generation_file, flags_info.root, 0);
}

DATA_DESK_FUNC void
DataDeskCustomDeclarationCallback(DataDeskDeclaration declaration_info, char *filename) {}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
    fclose(global_generation_file);
}