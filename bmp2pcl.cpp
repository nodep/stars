#include <windows.h>
#include <stdio.h>

#include "mm_file.h"

void do_help()
{
	printf("params: {infilename} {outfilename} [/c]\n");
	printf("        infilename -- input file name (must be 8bpp grayscale BMP)\n");
	printf("        outfilename -- output file name (will be 1bpp BMP)\n");
	printf("        /c -- enable simple run-length compression\n");
}

int main(int argc, wchar_t** argv)
{
	// open the bmp file
	wchar_t* bmp_fname = 0;
	wchar_t* pcl_fname = 0;
	bool should_compress = false;

	// read the arguments
	for (int a = 1; a < argc; ++a)
	{
		if (wcscmp(argv[a], L"/c") == 0)
		{
			should_compress = true;
		} else {
			if (bmp_fname == 0)
				bmp_fname = argv[a];
			else 
				pcl_fname = argv[a];
		}
	}

	if (bmp_fname == 0)
	{
		printf("Error: no input filename\n");
		do_help();
		return 0;
	} else if (pcl_fname == 0) {
		printf("Error: no output filename\n");
		do_help();
		return 0;
	}

	HANDLE hFile, hMemObj;
	char* pBmp = open_mm_file(bmp_fname, 0, hFile, hMemObj);

	BITMAPFILEHEADER* pBmpHeader = (BITMAPFILEHEADER*) pBmp;
	BITMAPINFOHEADER* pBmpInfoHeader = (BITMAPINFOHEADER*) (pBmp + sizeof(BITMAPFILEHEADER));

	FILE* fout = NULL;
	const int err = _wfopen_s(&fout, pcl_fname, L"w");
	if (err != 0)
	{
		char buffer[256];
		const int err2 = strerror_s(buffer, sizeof(buffer), err);
		perror(buffer);
		return err;
	}

	// set the raster resolution
	fputs("\x1b*t1200R", fout);				// DPI

	fprintf(fout, "\x1b*r%iT", pBmpInfoHeader->biHeight);				// raster height
	fprintf(fout, "\x1b*r%iS", pBmpInfoHeader->biWidth);				// raster width

	fputs("\x1b*r0A", fout);					// start raster graphics

	if (should_compress)
		fputs("\x1b&*b1M", fout);				// set raster compression (run-length data)
	else
		fputs("\x1b&*b0M", fout);				// set raster compression (unencoded)

	const int row_size_bytes = ((pBmpInfoHeader->biWidth + 31) & ~31) >> 3;

	char* pRow = (pBmp + pBmpHeader->bfOffBits) + (pBmpInfoHeader->biHeight - 1) * row_size_bytes;

	char comp_row[1024 * 100];
	for (int row = 0; row < pBmpInfoHeader->biHeight; ++row)
	{
		if (should_compress)
		{
			char* pIn = pRow + 1;
			char* pOut = comp_row;
			char last = *pRow;
			int c = 0;
			while (pIn < pRow + row_size_bytes)
			{
				if (last != *pIn  ||  c == 255)
				{
					*pOut = (unsigned char) c;
					++pOut;
					*pOut = last;
					++pOut;

					c = 0;
					last = *pIn;

				} else {
					c++;
				}

				++pIn;
			}

			*pOut = (unsigned char) c;
			++pOut;
			*pOut = last;
			++pOut;

			fprintf(fout, "\x1b*b%iW", (int)(pOut - comp_row));
			fwrite(comp_row, pOut - comp_row, 1, fout);
		} else {
			// write the uncompressed row
			fprintf(fout, "\x1b*b%iW", (int)row_size_bytes);
			fwrite(pRow, row_size_bytes, 1, fout);
		}

		pRow -= row_size_bytes;
	}

	fputs("\x1b*t1200R", fout);				// end raster graphics
	fclose(fout);

	return 0;
}
