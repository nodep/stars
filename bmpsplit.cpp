#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>

#include "mm_file.h"

inline bool gray2bool(unsigned char p)
{
	if (p == 0)			
		return true;
	else if (p == 255)	
		return false;

	return (rand() / (RAND_MAX / 255)) > p;
}

void do_help()
{
	printf("params: {infilename} {outfilename} /ref {x y} /size {width height}\n");
	printf("        infilename -- input file name (must be 8bpp grayscale BMP)\n");
	printf("        outfilename -- output file name (will be 1bpp BMP)\n");
	printf("        x y -- starting point in the input bmp\n");
	printf("        width height -- width and height of the resulting bmp\n");
}

int main(int argc, wchar_t** argv)
{
	wchar_t* in_fname = 0;
	wchar_t* out_fname = 0;

	int out_x = 0;
	int out_y = 0;
	int out_width = -1;
	int out_height = -1;

	// read the arguments
	for (int a = 1; a < argc; ++a)
	{
		if (wcscmp(argv[a], L"/ref") == 0)
		{
			if (a + 2 >= argc)
			{
				do_help();
				return 0;
			}

			out_x = wcstol(argv[a + 1], NULL, 10);
			out_y = wcstol(argv[a + 2], NULL, 10);
		} else if (wcscmp(argv[a], L"/size") == 0) {

			if (a + 2 >= argc)
			{
				do_help();
				return 0;
			}

			out_width = wcstol(argv[a + 1], NULL, 10);
			out_height = wcstol(argv[a + 2], NULL, 10);
		} else {
			if (in_fname == 0)
				in_fname = argv[a];
			else
				out_fname = argv[a];
		}
	}

	if (in_fname == 0)
	{
		printf("Error: no input filename\n");
		do_help();
		return 0;
	} else if (out_fname == 0) {
		printf("Error: no output filename\n");
		do_help();
		return 0;
	}

	HANDLE infile, outfile, inmemobj, outmemobj;
	char* pIn;
	char* pOut;

	pIn = open_mm_file(in_fname, 0, infile, inmemobj);

	// read the input file headers
	BITMAPFILEHEADER* pBmpHeader = (BITMAPFILEHEADER*) pIn;
	BITMAPINFOHEADER* pBmpInfoHeader = (BITMAPINFOHEADER*) (pIn + sizeof(BITMAPFILEHEADER));

	const int in_width = pBmpInfoHeader->biWidth;
	const int in_height = pBmpInfoHeader->biHeight;

	if (out_x >= in_width  ||  out_y >= in_height)
	{
		printf("Error: reference point out of image\n");
		do_help();
		return 0;
	}

	if (in_height < out_y + out_height  ||  out_height == -1)
		out_height = in_height - out_y;

	if (in_width < out_x + out_width  ||  out_width == -1)
		out_width = in_width - out_x;

	const int in_row_size_bytes = ((in_width * 8 + 31) & ~31) >> 3;

	const int out_row_size_bytes = ((out_width + 31) & ~31) >> 3;
	const int out_file_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + out_height * out_row_size_bytes + sizeof(RGBQUAD) * 2;

	// open the output file
	pOut = open_mm_file(out_fname, out_file_size, outfile, outmemobj);

	BITMAPFILEHEADER* pOutBmpHeader = (BITMAPFILEHEADER*) pOut;
	BITMAPINFOHEADER* pOutBmpInfoHeader = (BITMAPINFOHEADER*) (pOut + sizeof(BITMAPFILEHEADER));
	RGBQUAD* pColors = (RGBQUAD*) (pOut + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	pColors[0].rgbBlue = 255;
	pColors[0].rgbRed = 255;
	pColors[0].rgbGreen = 255;

	pColors[1].rgbBlue = 0;
	pColors[1].rgbRed = 0;
	pColors[1].rgbGreen = 0;

	::memset(pOutBmpHeader, 0, sizeof(BITMAPFILEHEADER));
	::memset(pOutBmpInfoHeader, 0, sizeof(BITMAPINFOHEADER));

	pOutBmpHeader->bfType = 0x4D42;
	pOutBmpHeader->bfSize = out_file_size;
	pOutBmpHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	pOutBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	pOutBmpInfoHeader->biWidth = out_width;
	pOutBmpInfoHeader->biHeight = out_height;
	pOutBmpInfoHeader->biPlanes = 1;
	pOutBmpInfoHeader->biBitCount = 1;
	pOutBmpInfoHeader->biCompression = BI_RGB; 
	pOutBmpInfoHeader->biXPelsPerMeter = pBmpInfoHeader->biXPelsPerMeter;
	pOutBmpInfoHeader->biYPelsPerMeter = pBmpInfoHeader->biYPelsPerMeter;

	// start copying the pixels
	for (int out_row = 0; out_row < out_height; ++out_row)
	{
		unsigned char* pInRow = (unsigned char*) (pIn + pBmpHeader->bfOffBits + (in_height - out_y - out_row - 1) * in_row_size_bytes);
		unsigned char* pOutRow = (unsigned char*) (pOut + pOutBmpHeader->bfOffBits + (out_height - out_row - 1) * out_row_size_bytes);

		unsigned char* pin = pInRow + out_x;
		unsigned char* pout = pOutRow;

		int pxcnt = 0;
		int bit;
		while (pxcnt < out_width)
		{
			bit = pxcnt % 8;

			if (gray2bool(*pin))
				*pout |= 0x80 >> bit;
			else
				*pout &= 0xff ^ (0x80 >> bit);

			++pin;
			++pxcnt;

			if (pxcnt % 8 == 0)
				++pout;
		}
	}

	::UnmapViewOfFile(pIn);
	::CloseHandle(inmemobj);
	::CloseHandle(infile);

	::UnmapViewOfFile(pOut);
	::CloseHandle(outmemobj);
	::CloseHandle(outfile);


	return 0;
}
