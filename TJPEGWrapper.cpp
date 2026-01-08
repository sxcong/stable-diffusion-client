

#include "TJPEGWrapper.h"
#include "include\turbojpeg.h"
//#include "MaxImageDef.h"
#include <math.h>
#include <new>
#include <Windows.h>
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QDebug>

#include "include/FreeImage.h"
#include "include/FreeImagePlus.h"


const unsigned int MAX_IMAGE_PIXELS = 1024 * 1024 * 500;
const unsigned int MAX_IMAGE_DIMENSION = 65535;

void * TurboJpeg::ReadImage(int &width,
					   int &height,
					   int &nchannels,
					   TJSAMP &chromoSubsampling,
					   bool &outOfMemory,
					   const void *buffer,
					   int sizebytes)
{
	outOfMemory = false;
	width = height = 0;
	nchannels = 3;
	chromoSubsampling = TJSAMP_420;

	tjhandle hDecoder = tjInitDecompress();
	if (hDecoder == NULL) {
		return NULL;
	}

	unsigned char* pPixelData = NULL;
	int nSubSampling;
	int nResult = tjDecompressHeader2(hDecoder, (unsigned char*)buffer, sizebytes, &width, &height, &nSubSampling);
	if (nResult == 0){
		chromoSubsampling = (TJSAMP)nSubSampling;
		if (abs((double)width * height) > MAX_IMAGE_PIXELS) {
			outOfMemory = true;
		} else if (width <= MAX_IMAGE_DIMENSION && height <= MAX_IMAGE_DIMENSION) {
			pPixelData = new(std::nothrow) unsigned char[TJPAD(width * 3) * height];
			if (pPixelData != NULL) {
				nResult = tjDecompress2(hDecoder, (unsigned char*)buffer, sizebytes, pPixelData, width, TJPAD(width * 3), height,
					TJPF_BGR, 0);
				if (nResult != 0) {
					delete[] pPixelData;
					pPixelData = NULL;
				}
			} else {
				outOfMemory = true;
			}
		}
	}

	tjDestroy(hDecoder);

	return pPixelData;
}

void * TurboJpeg::Compress(const void *source,
					  int width,
					  int height,
					  int &len,
					  bool &outOfMemory,
					  int quality)
{
	outOfMemory = false;
	len = 0;
	tjhandle hEncoder = tjInitCompress();
	if (hEncoder == NULL) {
		return NULL;
	}

	unsigned char* pJPEGCompressed = NULL;
	unsigned long nCompressedLen = 0;
	int nResult = tjCompress2(hEncoder, (unsigned char*)source, width, TJPAD(width * 3), height, TJPF_BGR,
		&pJPEGCompressed, &nCompressedLen, TJSAMP_420, quality, 0);
	if (nResult != 0) {
		if (pJPEGCompressed != NULL) {
			tjFree(pJPEGCompressed);
			pJPEGCompressed = NULL;
		} else {
			outOfMemory = true;
		}
	}

	len = nCompressedLen;

	tjDestroy(hEncoder);

	return pJPEGCompressed;
}


// calculate CRT table
void CalcCRCTable(unsigned int crc_table[256]) {
     for (int n = 0; n < 256; n++) {
       unsigned int c = (unsigned int) n;
       for (int k = 0; k < 8; k++) {
         if (c & 1)
           c = 0xedb88320L ^ (c >> 1);
         else
           c = c >> 1;
       }
       crc_table[n] = c;
     }
}


void* FindJPEGMarker(void* pJPEGStream, int nStreamLength, unsigned char nMarker)
{
    uint8_t* pStream = (uint8_t*) pJPEGStream;
    if (pStream == NULL || nStreamLength < 3 || pStream[0] != 0xFF || pStream[1] != 0xD8)
    {
        return NULL; // not a JPEG
    }
    int nIndex = 2;
    do {
        if (pStream[nIndex] == 0xFF) {
            // block header found, skip padding bytes
            while (pStream[nIndex] == 0xFF && nIndex < nStreamLength) nIndex++;
            if (pStream[nIndex] == 0 || pStream[nIndex] == nMarker) {
                break; // 0xFF 0x00 is part of pixel block, break
            } else {
                // it's a block marker, read length of block and skip the block
                nIndex++;
                if (nIndex+1 < nStreamLength) {
                    nIndex += pStream[nIndex]*256 + pStream[nIndex+1];
                } else {
                    nIndex = nStreamLength;
                }
            }
        }
        else
        {
            break; // block with pixel data found, start hashing from here
        }
    } while (nIndex < nStreamLength);

    if (nMarker == 0 || (pStream[nIndex] == nMarker && pStream[nIndex-1] == 0xFF))
    {
        return &(pStream[nIndex-1]); // place on marker start
    }

    return NULL;
}
__int64 CalculateJPEGFileHash(void* pJPEGStream, int nStreamLength)
{
    uint8_t* pStream = (uint8_t*) pJPEGStream;
    void* pPixelStart = FindJPEGMarker(pJPEGStream, nStreamLength, 0);
    if (pPixelStart == NULL)
    {
        return 0;
    }
    int nIndex = (int)((uint8_t*)pPixelStart - (uint8_t*)pJPEGStream + 1);

    // take whole stream in case of inconsistency or if remaining part is too small
    if (nStreamLength - nIndex < 4)
    {
        nIndex = 0;
        assert(false);
    }

    // now we can calculate the hash over the compressed pixel data
    // do not look at every byte due to performance reasons
    const int nTotalLookups = 10000;
    int nIncrement = (nStreamLength - nIndex)/nTotalLookups;
    nIncrement = max(1, nIncrement);

    unsigned int crc_table[256];
    CalcCRCTable(crc_table);
    uint32_t crcValue = 0xffffffff;
    unsigned int sumValue = 0;
    while (nIndex < nStreamLength) {
        sumValue += pStream[nIndex];
        crcValue = crc_table[(crcValue ^ pStream[nIndex]) & 0xff] ^ (crcValue >> 8);
        nIndex += nIncrement;
    }

    return ((__int64)crcValue << 32) + sumValue;
}

//(FreeImage_GetLine(dib) + 3 & ~3);
//FreeImage_GetLine(FIBITMAP *dib) {
//return  ((width * bpp) + 7)/8
//#include <string>
//#include <locale>
//#include <codecvt>
/*
//convert string to wstring
inline std::wstring to_wide_string(const std::string& input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input);
}

//convert wstring to string
inline std::string to_byte_string(const std::wstring& input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}
*/

int DoPadding(int value, int padvalue) {
        return (value + padvalue - 1) & ~(padvalue - 1);
}
#define ALPHA_OPAQUE 0xFF000000
void* Convert3To4Channels(int nWidth, int nHeight, const void* pIJLPixels) {
    if (pIJLPixels == NULL) {
        return NULL;
    }
    int nPadSrc = DoPadding(nWidth*3, 4) - nWidth*3;
    uint32_t* pNewDIB = new(std::nothrow) uint32_t[nWidth * nHeight];
    if (pNewDIB == NULL) return NULL;
    uint32_t* pTarget = pNewDIB;
    const uint8_t* pSource = (uint8_t*)pIJLPixels;
    for (int j = 0; j < nHeight; j++) {
        for (int i = 0; i < nWidth; i++) {
            *pTarget++ = pSource[0] + pSource[1] * 256 + pSource[2] * 65536 + ALPHA_OPAQUE;
            pSource += 3;
        }
        pSource += nPadSrc;
    }
    return pNewDIB;
}



// 180 degrees rotation
static void* Rotate32bpp180(int nWidth, int nHeight, const void* pDIBPixels) {
    uint32_t* pTarget = new(std::nothrow) uint32_t[nWidth * nHeight];
    if (pTarget == NULL) return NULL;
    const uint32_t* pSource = (uint32_t*)pDIBPixels;
    for (uint32_t i = 0; i < nHeight; i++) {
        uint32_t* pTgt = pTarget + nWidth*(nHeight - 1 - i) + (nWidth - 1);
        const uint32_t* pSrc = pSource + nWidth*i;
        for (uint32_t j = 0; j < nWidth; j++) {
            *pTgt = *pSrc;
            pTgt -= 1;
            pSrc += 1;
        }
    }
    return pTarget;
}

// Rotate a block of a 32 bit DIB from source to target by 90 or 270 degrees
static void RotateBlock32bpp(const uint32_t* pSrc, uint32_t* pTgt, int nWidth, int nHeight,
                             int nXStart, int nYStart, int nBlockWidth, int nBlockHeight, bool bCW) {
    int nIncTargetLine = nHeight;
    int nIncSource = nWidth - nBlockWidth;
    const uint32_t* pSource = pSrc + nWidth * nYStart + nXStart;
    uint32_t* pTarget = bCW ? pTgt + nIncTargetLine * nXStart + (nHeight - 1 - nYStart) :
        pTgt + nIncTargetLine * (nWidth - 1 - nXStart) + nYStart;
    uint32_t* pStartYPtr = pTarget;
    if (!bCW) nIncTargetLine = -nIncTargetLine;
    int nIncStartYPtr = bCW ? -1 : +1;

    for (uint32_t i = 0; i < nBlockHeight; i++) {
        for (uint32_t j = 0; j < nBlockWidth; j++) {
            *pTarget = *pSource;
            pTarget += nIncTargetLine;
            pSource++;
        }
        pStartYPtr += nIncStartYPtr;
        pTarget = pStartYPtr;
        pSource += nIncSource;
    }
}


void* Rotate32bpp(int nWidth, int nHeight, const void* pDIBPixels, int nRotationAngleCW) {
    if (pDIBPixels == NULL) {
        return NULL;
    }
    if (nRotationAngleCW != 90 && nRotationAngleCW != 180 && nRotationAngleCW != 270) {
        return NULL; // not allowed
    }
    else if (nRotationAngleCW == 180)
    {
        return Rotate32bpp180(nWidth, nHeight, pDIBPixels);
    }

    uint32_t* pTarget = new(std::nothrow) uint32_t[nHeight * nWidth];
    if (pTarget == NULL) return NULL;
    const uint32_t* pSource = (uint32_t*)pDIBPixels;

    const int cnBlockSize = 32;
    int nX = 0, nY = 0;
    while (nY < nHeight)
    {
        nX = 0;
        while (nX < nWidth)
        {
            RotateBlock32bpp(pSource, pTarget, nWidth, nHeight,
                nX, nY,
                min(cnBlockSize, nWidth - nX),
                min(cnBlockSize, nHeight - nY), nRotationAngleCW == 90);
            nX += cnBlockSize;
        }
        nY += cnBlockSize;
    }

    return pTarget;
}


int TurboLoadImage(const std::wstring& FileName, ReadData& readData, int nOrientation)
{
    static unsigned int MAX_JPEG_FILE_SIZE = 1024 * 1024 * 300;
    bool bOutOfMemory = false;
    HGLOBAL hFileBuffer = NULL;
    void* pBuffer = NULL;

    //ExifInfo m_exifInfo;

    //std::string sfile = std::string(FileName.begin(), FileName.end());
    //bool ret = m_exifInfo.getExifInfo(sfile);
    //qDebug()<<"exifInfo.Orientation"<< m_exifInfo.Orientation.c_str();


    HANDLE hFile = ::CreateFile(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    try
    {
        // Don't read too huge files
        unsigned int nFileSize = ::GetFileSize(hFile, NULL);
        if (nFileSize > MAX_JPEG_FILE_SIZE)
        {
            bOutOfMemory = true;
            ::CloseHandle(hFile);
            return -2;
        }
        hFileBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, nFileSize);
        pBuffer = (hFileBuffer == NULL) ? NULL : ::GlobalLock(hFileBuffer);
        if (pBuffer == NULL)
        {
            if (hFileBuffer) ::GlobalFree(hFileBuffer);
            bOutOfMemory = true;
            ::CloseHandle(hFile);
            return -3;
        }
        unsigned int nNumBytesRead;
        if (::ReadFile(hFile, pBuffer, nFileSize, (LPDWORD) &nNumBytesRead, NULL) && nNumBytesRead == nFileSize)
        {
             void* pNewOriginalPixels;
            int nWidth, nHeight, nBPP;
            TJSAMP eChromoSubSampling;
            void* pPixelData = TurboJpeg::ReadImage(nWidth, nHeight, nBPP, eChromoSubSampling, bOutOfMemory, pBuffer, nFileSize);
            int buffSize =nWidth * 3 * nHeight;

            qDebug()<<"TurboJpeg::ReadImage"<<FileName.c_str()<<nWidth<<nHeight<<nBPP<<nFileSize<<buffSize<<eChromoSubSampling;
            if (pPixelData != NULL)
            {
                if (nBPP == 1)
                {

                }
                else if (nBPP == 3)
                {
                    pNewOriginalPixels = Convert3To4Channels(nWidth, nHeight, pPixelData);
                    if (pNewOriginalPixels != NULL)
                    {
                        delete[] pPixelData;
                        pPixelData = pNewOriginalPixels;
                        nBPP = 4;
                    }
                }
                else if (nBPP == 4)
                {

                }


                if (nOrientation == 6)
                {
                    void* pNewOriginalPixels2 = Rotate32bpp(nWidth, nHeight, pNewOriginalPixels, 90);
                    if (pNewOriginalPixels == NULL)
                        return false;
                    delete[] pPixelData;
                    pPixelData = pNewOriginalPixels2;

                    //if (nRotation != 180)
                    {
                        // swap width and height
                        int nTemp = nWidth;
                        nWidth = nHeight;
                        nHeight = nTemp;
                    }
                }

                QImage img((uchar*)pPixelData, nWidth, nHeight, QImage::Format_ARGB32);
                //img = img.rgbSwapped();
                readData.pixmap = QPixmap::fromImage(img);
                readData.absoluteFilePath = QString::fromStdWString(FileName);
                readData.fileSize = nFileSize;
                readData.imageSize = QSize(nWidth, nHeight);
                //qDebug()<<"readData.absoluteFilePath"<<readData.absoluteFilePath<<readData.fileSize<<readData.pixmap.isNull()<<img.isNull();
                if (readData.pixmap.isNull())
                {
                    return false;
                }
            }
            else if (bOutOfMemory)
            {
                //OutOfMemory = true;
            }
            else
            {
                delete[] pPixelData;
            }
        }
        ::CloseHandle(hFile);
        if (pBuffer) ::GlobalUnlock(pBuffer);
        if (hFileBuffer) ::GlobalFree(hFileBuffer);
        return 0;

    }
    catch(...)
    {

    }
    return 0;
}
