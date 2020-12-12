#include "dd.h"
#include "dd.c"

// TODO(allen): DD_ASSERT
#define DD_ASSERT(c) if (!c) { *(DD_u64*)0 = 0; }

void run_test_on_string(DD_String8 string)
{
    DD_String16 s16 = DD_S16FromS8(string);
    DD_String8  s8_ts16 = DD_S8FromS16(s16);
    DD_ASSERT(DD_StringMatch(s8_ts16, string, 0));
    
    DD_String32 s32 = DD_S32FromS8(string);
    DD_String8  s8_ts32 = DD_S8FromS32(s32);
    DD_ASSERT(DD_StringMatch(s8_ts32, string, 0));
}

int main(void)
{
    // TODO(allen): throw more at this.
    
    char test_string_c[] = "Foo bar; test the unicode\n\t\0Etc";
    DD_String8 test_string = DD_S8(test_string_c, sizeof(test_string_c) - 1);
    
    run_test_on_string(test_string);
    
    return 0;
}