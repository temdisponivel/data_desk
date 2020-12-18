#include "data_desk.h"

FILE *file;

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
	file = fopen("output.h", "w");
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode *root, char *filename)
{
	for (DataDeskNode *node = root; node; node = node->next) {
        DataDeskFWriteGraphAsC(file, node);
    }
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
	fclose(file);
}