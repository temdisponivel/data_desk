
#define MAX_PATH 260
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef int BOOL;
typedef void *HANDLE;
typedef wchar_t WCHAR;
typedef const WCHAR *LPCWSTR;

typedef struct _FILETIME _FILETIME;
struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};
typedef _FILETIME FILETIME;
typedef _FILETIME *PFILETIME;
typedef _FILETIME *LPFILETIME;

typedef struct _WIN32_FIND_DATAW _WIN32_FIND_DATAW;
struct _WIN32_FIND_DATAW
{
#define FILE_ATTRIBUTE_DIRECTORY 0x10
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwReserved0;
    DWORD    dwReserved1;
    WCHAR    cFileName[MAX_PATH];
    WCHAR    cAlternateFileName[14];
    DWORD    dwFileType;
    DWORD    dwCreatorType;
    WORD     wFinderFlags;
};

typedef _WIN32_FIND_DATAW WIN32_FIND_DATAW;
typedef _WIN32_FIND_DATAW *PWIN32_FIND_DATAW;
typedef _WIN32_FIND_DATAW *LPWIN32_FIND_DATAW;

HANDLE FindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
BOOL FindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);

#pragma comment(lib, "User32.lib")

DD_PRIVATE_FUNCTION_IMPL LPCWSTR
_DD_CWSTRFromString8(DD_String8 string)
{
    LPCWSTR result = calloc(sizeof(WCHAR), (string.size+1)*2);
    if(result)
    {
        // TODO(rjf)
    }
    return result;
}

DD_PRIVATE_FUNCTION_IMPL DD_String8
_DD_String8FromCWSTR(LPCWSTR wstr)
{
    DD_String8 result = {0};
    // TODO(rjf)
    return result;
}

DD_PRIVATE_FUNCTION_IMPL DD_b32
_DD_OS_IMPL_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info)
{
    DD_b32 result = 0;
    
    WIN32_FIND_DATAW find_data = {0};
    HANDLE state = (HANDLE)it->state;
    if(state == 0)
    {
        state = FindFirstFileW(_DD_CWSTRFromString8(path), &find_data);
        result = !!state;
    }
    else
    {
        result = !!FindNextFileW(state, &find_data);
    }
    
    it->state = (DD_u64)state;
    if(result)
    {
        out_info->flags = 0;
        if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            out_info->flags |= DD_FileFlag_Directory;
        }
        out_info->path = _DD_String8FromCWSTR(find_data.cFileName);
        out_info->extension = DD_ExtensionString(out_info->path);
        out_info->file_size = ((((DD_u64)find_data.nFileSizeHigh) << 32) |
                               ((DD_u64)find_data.nFileSizeLow));
    }
    
    return result;
}