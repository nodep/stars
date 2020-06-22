#include <windows.h>
#include <assert.h>

char* open_mm_file(const wchar_t* fname, int fsize, HANDLE& hFile, HANDLE& hMemMapObj)
{
	hFile = ::CreateFile(fname,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	assert(hFile != INVALID_HANDLE_VALUE);

	if (fsize == 0)
		fsize = ::GetFileSize(hFile, NULL);

	// create the mem-map object
	hMemMapObj = ::CreateFileMapping(hFile,
										NULL,
										PAGE_READWRITE,
										0,
										fsize,
										NULL);

	assert(hMemMapObj != 0);

	// create the mapping
	char* ret_val = (char*) ::MapViewOfFile(hMemMapObj, FILE_MAP_WRITE, 0, 0, 0);

	return ret_val;
}

