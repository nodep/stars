#define NOMINMAX
#include <windows.h>

#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <assert.h>

#include "mm_file.h"

class printer_t
{
private:
	HANDLE	_hPrinter;
public:

	printer_t(const std::wstring& printer_name)
	{
		BOOL result = ::OpenPrinter((LPWSTR)printer_name.c_str(), &_hPrinter, NULL);
		if (result == 0)
			throw "::OpenPrinter() failed";
	}

	HANDLE get()
	{
		return _hPrinter;
	}

	~printer_t()
	{
		::ClosePrinter(_hPrinter);
	}
};

int main()
{
	std::wstring print_to(L"HP LaserJet 5100 PS");
	//std::string print_to("\\\\HUPTYWS005\\HP LaserJet 5100 PS");
	//std::string print_to("Microsoft Office Document Image Writer");

	printer_t printer(print_to);
	size_t buff_size = ::DocumentProperties(0, printer.get(), (LPWSTR)print_to.c_str(), NULL, NULL, 0);
	std::vector<char> buffer(buff_size, '\0');

	DEVMODE* devMode;
	devMode = (DEVMODE*) &buffer.front();

	devMode->dmSize = sizeof(DEVMODE);
	devMode->dmFields = DM_PAPERSIZE | DM_ORIENTATION | DM_PAPERWIDTH | DM_PAPERLENGTH | DM_PELSWIDTH;

	// get the default DEVMODE
	LONG result = ::DocumentProperties(0, printer.get(), (LPWSTR)print_to.c_str(), devMode, 0, DM_OUT_BUFFER);

	// create the printer device context
	HDC hPrinterDC = ::CreateDC(NULL, print_to.c_str(), NULL, devMode);

	size_t horz = ::GetDeviceCaps(hPrinterDC, HORZRES);
	size_t vert = ::GetDeviceCaps(hPrinterDC, VERTRES);

	// open the document
	DOCINFO di;
	di.cbSize = sizeof(di);
	di.lpszDocName = L"Testing BMP";
	di.lpszOutput = NULL;
	di.lpszDatatype = NULL;
	di.fwType = 0;

	int doc_result = ::StartDoc(hPrinterDC, &di);
	assert(doc_result > 0);

	int page_result = ::StartPage(hPrinterDC);


	// open the bitmap
	HBITMAP hBitmap = (HBITMAP) ::LoadImage(::GetModuleHandle(0), L"p2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	BITMAP bmpData;
	::GetObject(hBitmap, sizeof(bmpData), &bmpData);

	HDC hMemoryDC = ::CreateCompatibleDC(hPrinterDC);

	HGDIOBJ soresult = ::SelectObject(hMemoryDC, hBitmap);

	LONG y = 0, stripe_height;
	const int step = 500;
	while (y < bmpData.bmHeight)
	{
		stripe_height = step;
		if (y + stripe_height > bmpData.bmHeight)
			stripe_height = bmpData.bmHeight - y;

		::BitBlt(hPrinterDC, 0, y, bmpData.bmWidth, std::min(y, bmpData.bmHeight - y), hMemoryDC, 0, y, SRCCOPY);

		y += stripe_height;

		break;
	}

	::DeleteDC(hMemoryDC);

	::EndPage(hPrinterDC);
	::EndDoc(hPrinterDC);

	::DeleteDC(hPrinterDC);

	return 0;
}


	/*
	// print the pixels
	HANDLE infile, inmemobj;
	char* pBmp = open_mm_file("c:\\projects\\stars\\p2.bmp", 0, infile, inmemobj);

	BITMAPFILEHEADER* pBmpHeader = (BITMAPFILEHEADER*) pBmp;
	BITMAPINFOHEADER* pBmpInfoHeader = (BITMAPINFOHEADER*) (pBmp + sizeof(BITMAPFILEHEADER));

	unsigned char* pDataBits = (unsigned char*) (pBmp + pBmpHeader->bfOffBits);
	const int row_size_bytes = ((pBmpInfoHeader->biWidth + 31) & ~31) >> 3;

	for (int y = 0; y < vert; ++y)
	{
		unsigned char* pRow = (unsigned char*) (pDataBits + (pBmpInfoHeader->biHeight - y - 1) * row_size_bytes);

		int x = 0;
		while (x < horz)
		{
			int bit = x % 8;

			if (*pRow & (1 << bit) != 0)
			{
				::SetPixel(hdc, x, y, RGB(0, 0, 0));
			}

			++x;

			if (x % 8 == 0)
				++pRow;
		}
	}
	*/

