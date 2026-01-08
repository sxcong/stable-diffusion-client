/*
 * QPhotoView core classes
 *
 * License: GPL V2. See file COPYING for details.
 *
 * Author:  Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QDebug>

#include "PhotoMetaData.h"
#include "Photo.h"

#include <iostream>
#include <regex>
#include <set>
#include <vector>
#include <assert.h>
#include <iostream>
using namespace std;

int test_parse(const char* input_filename);

PhotoMetaData::PhotoMetaData( Photo * photo )
{
    _isEmpty = true;

    if ( photo )
    {
	_photoFullPath = photo->fullPath();
	_size = photo->size();
	readExifData( _photoFullPath );
    }
}


void PhotoMetaData::readExifData( const QString & fileName )
{
    //test_parse(_photoFullPath.toStdString().c_str());
 /*   try
    {
    Exiv2::Image::UniquePtr image =
	    Exiv2::ImageFactory::open( _photoFullPath.toStdString() );

	image->readMetadata();
	Exiv2::ExifData &exifData = image->exifData();

	if ( exifData.empty() )
	    return;

	_isEmpty = false;

    _exposureTime = exifFract( exifData, "Exif.Photo.ExposureTime" );//曝光时间 (快门速度的倒数). 单位是秒.
    _aperture     = exifFract( exifData, "Exif.Photo.FNumber"      );//拍照时的光圈F-number(F-stop).
    _iso	      = exifInt	 ( exifData, "Exif.Photo.ISOSpeedRatings" );//CCD 的感光度, 等效于 Ag-Hr 胶片的速率.
    _focalLength  = exifFract( exifData, "Exif.Photo.FocalLength" ).toDouble();//拍摄照片时的镜头的焦距长度. 单位是毫米.
	_focalLength35mmEquiv = exifInt( exifData, "Exif.Photo.FocalLengthIn35mmFilm" );

	int origWidth  = exifInt( exifData, "Exif.Photo.PixelXDimension" );
	int origHeight = exifInt( exifData, "Exif.Photo.PixelYDimension" );
	_origSize = QSize( origWidth, origHeight );

    QString dateTimeStr = exifString( exifData, "Exif.Photo.DateTimeOriginal" );//照片在被拍下来的日期/时间. 使用用户的软件是不能被修改这个值的
	_dateTimeTaken = QDateTime::fromString( dateTimeStr, Qt::ISODate );
    }
    catch ( Exiv2::Error& exception )
    {
	qWarning() << "Caught Exiv2 exception:" << exception.what()
		   << "for" << fileName;
    }
*/
}


int PhotoMetaData::exifInt( Exiv2::ExifData &  exifData,
			    const char *       exifKey )
{
    int val = 0;
    Exiv2::ExifData::const_iterator it =
	exifData.findKey( Exiv2::ExifKey( exifKey ) );

    if ( it != exifData.end() )
    {
        val = (int) it->value().toInt64();
    }

    return val;
}


QString PhotoMetaData::exifString( Exiv2::ExifData &  exifData,
                   const char *	 exifKey )
{
    QString val;
    Exiv2::ExifData::const_iterator it =
	exifData.findKey( Exiv2::ExifKey( exifKey ) );
    if ( it != exifData.end() )
    {
        val = it->value().toString().c_str();
    }
    return val;
}


// copied from src/tiffvisitor_int.cpp
static const Exiv2::TagInfo* findTag(const Exiv2::TagInfo* pList, uint16_t tag) {
    while (pList->tag_ != 0xffff && pList->tag_ != tag)
        pList++;
    return pList->tag_ != 0xffff ? pList : nullptr;
}

int test_parse(const char* input_filename)
{
    try {
        setlocale(LC_CTYPE, ".utf8");
        bool bRet = Exiv2::XmpParser::initialize();

        ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
        Exiv2::enableBMFF();
#endif


        int rc = EXIT_SUCCESS;
        const char* file = input_filename;
        bool bLint = false;

        //std::vector<std::regex> keys;
        //Exiv2::dumpLibraryInfo(std::cout, keys);
        //return 0;
       /* FILE* pf = ::fopen(file, "rb");
        if (pf != NULL)
        {
            fclose(pf);
        }*/

        auto image = Exiv2::ImageFactory::open(file, false);
        image->readMetadata();

        Exiv2::ExifData& exifData = image->exifData();
        qDebug()<<"image->exifData()"<<exifData.empty();
        if (exifData.empty())
        {
            std::string error("No Exif data found in file");
           // throw Exiv2::Error(Exiv2::ErrorCode::kerErrorMessage, error);
            return -1;
        }


        std::set<std::string> shortLong;
        shortLong.insert("Exif.Photo.PixelXDimension");
        shortLong.insert("Exif.Photo.PixelYDimension");
        shortLong.insert("Exif.Photo.ImageLength");
        shortLong.insert("Exif.Photo.ImageWidth");
        shortLong.insert("Exif.Photo.RowsPerStrip");
        shortLong.insert("Exif.Photo.StripOffsets");
        shortLong.insert("Exif.Photo.StripByteCounts");

        auto end = exifData.end();
        for (auto i = exifData.begin(); i != end; ++i) {
            if (!bLint) {
                const char* tn = i->typeName();
                std::cout << std::setw(44) << std::setfill(' ') << std::left << i->key() << " "
                    << "0x" << std::setw(4) << std::setfill('0') << std::right << std::hex << i->tag() << " "
                    << std::setw(9) << std::setfill(' ') << std::left << (tn ? tn : "Unknown") << " " << std::dec
                    << std::setw(3) << std::setfill(' ') << std::right << i->count() << "  " << std::dec << i->toString()
                    << "\n";
            }
            else {
                const Exiv2::TagInfo* tagInfo = findTag(Exiv2::ExifTags::tagList(i->groupName()), i->tag());
                if (tagInfo) {
                    Exiv2::TypeId type = i->typeId();
                    if (type != tagInfo->typeId_ &&
                        (tagInfo->typeId_ != Exiv2::comment || type != Exiv2::undefined)  // comment is stored as undefined
                        && (shortLong.find(i->key()) == shortLong.end() ||
                        (type != Exiv2::unsignedShort && type != Exiv2::unsignedLong))  // can be short or long!
                        ) {
                        qDebug() << i->key().c_str() << " type " << i->typeName() << " (" << type << ")"
                            << " expected " << Exiv2::TypeInfo::typeName(tagInfo->typeId_) << " (" << tagInfo->typeId_ << ")";

                        rc = 2;
                    }
                }
            }
        }

        return rc;
    }
    catch (Exiv2::Error& e) {
        std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
        return EXIT_FAILURE;
    }
}
