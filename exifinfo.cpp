#include "exifinfo.h"
#include <QDebug>

ExifInfo::ExifInfo()
{
    m_exifKeyList.push_back("Exif.Image.Make");
    m_exifKeyList.push_back("Exif.Image.Model");
    m_exifKeyList.push_back("Exif.Image.DateTime");
    m_exifKeyList.push_back("Exif.Image.Orientation");
    m_exifKeyList.push_back("Exif.Photo.PixelXDimension");
    m_exifKeyList.push_back("Exif.Photo.PixelYDimension");
    m_exifKeyList.push_back("Exif.GPSInfo.GPSLatitudeRef");
    m_exifKeyList.push_back("Exif.GPSInfo.GPSLatitude");
    m_exifKeyList.push_back("Exif.GPSInfo.GPSLongitudeRef");
    m_exifKeyList.push_back("Exif.GPSInfo.GPSLongitude");
    m_exifKeyList.push_back("Exif.GPSInfo.GPSAltitudeRef");
    m_exifKeyList.push_back("Exif.GPSInfo.GPSAltitude");
}

bool ExifInfo::getExifInfo(std::string& fileName)
{
    reset();
    //qDebug()<<"ExifInfo::getExifInfo"<<fileName.c_str();
    try {
        auto image = Exiv2::ImageFactory::open(fileName, false);
        if (image != nullptr)
        {
            image->readMetadata();
            Exiv2::ExifData& exifData = image->exifData();
            if (exifData.empty())
            {
                std::string error("No Exif data found in file");
                //throw Exiv2::Error(Exiv2::ErrorCode::kerErrorMessage, error);
                qDebug()<<"No Exif data found in file";
                return false;
            }

            auto end = exifData.end();

           /* std::stringstream ss;
            for (auto i = exifData.begin(); i != end; ++i)
            {
                //qDebug()<<i->key().c_str()<< i->toString().c_str();
                const char* tn = i->typeName();
                ss << std::setw(44) << std::setfill(' ') << std::left << i->key() << " "
                          << "0x" << std::setw(4) << std::setfill('0') << std::right << std::hex << i->tag() << " "
                          << std::setw(9) << std::setfill(' ') << std::left << (tn ? tn : "Unknown") << " " << std::dec
                          << std::setw(3) << std::setfill(' ') << std::right << i->count() << "  " << std::dec << i->toString()
                          << "\n";

            }
            qDebug()<<"ss"<<ss.str().c_str();*/


            m_exifMap["Exif.Image.Make"]              = exifData["Exif.Image.Make"].toString();
            m_exifMap["Exif.Image.Model"]             = exifData["Exif.Image.Model"].toString();
            m_exifMap["Exif.Image.DateTime"]          = exifData["Exif.Image.DateTime"].toString();
            m_exifMap["Exif.Image.Orientation"]       = exifData["Exif.Image.Orientation"].toString();
            Orientation = m_exifMap["Exif.Image.Orientation"];
            m_exifMap["Exif.Photo.PixelXDimension"]   = exifData["Exif.Photo.PixelXDimension"].toString();
            m_exifMap["Exif.Photo.PixelYDimension"]   = exifData["Exif.Photo.PixelYDimension"].toString();

            m_exifMap["Exif.GPSInfo.GPSLatitudeRef"]  = exifData["Exif.GPSInfo.GPSLatitudeRef"].toString();
            m_exifMap["Exif.GPSInfo.GPSLatitude"]     = exifData["Exif.GPSInfo.GPSLatitude"].toString();
            m_exifMap["Exif.GPSInfo.GPSLongitudeRef"] = exifData["Exif.GPSInfo.GPSLongitudeRef"].toString();
            m_exifMap["Exif.GPSInfo.GPSLongitude"]    = exifData["Exif.GPSInfo.GPSLongitude"].toString();
            m_exifMap["Exif.GPSInfo.GPSAltitudeRef"]  = exifData["Exif.GPSInfo.GPSAltitudeRef"].toString();
            m_exifMap["Exif.GPSInfo.GPSAltitude"]     = exifData["Exif.GPSInfo.GPSAltitude"].toString();

            return true;
        }
    }
    catch (Exiv2::Error& e)
    {
        std::cout << "Caught Exiv2 exception '" << e << "'\n";
        qDebug()<<"Caught Exiv2 exception "<<e.what();
    }
    return false;
}

void ExifInfo::reset()
{
    m_exifMap.clear();
}

std::string ExifInfo::makeString()
{
    std::string text;
    std::stringstream ss;

/*    for (uint i = 0; i < m_exifKeyList.size(); i++)
    {

        if (i != 0)
            text += "\r\n";
        text += m_exifKeyList[i];
        text += " : ";
        text += m_exifMap[m_exifKeyList[i]];
    }
    return text;
*/

    for (uint i = 0; i < m_exifKeyList.size(); i++)
    {
        //ss << std::setw(30) << std::setfill(' ') << std::left << m_exifKeyList[i] << ": "
        //          << std::setfill(' ') << std::right << m_exifMap[m_exifKeyList[i]]<<'\n';

        ss <<std::left << m_exifKeyList[i] << ": " << std::setfill(' ') << std::right<< std::setw(9) << m_exifMap[m_exifKeyList[i]]<<'\n';

    }
    return ss.str();
}
