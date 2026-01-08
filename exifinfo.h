#ifndef EXIFINFO_H
#define EXIFINFO_H

#include "exiv2/image.hpp"
#include "exiv2/exif.hpp"
using namespace Exiv2;

#include <string>
#include <iostream>
#include <regex>
#include <map>
#include <vector>
#include <assert.h>
#include <iostream>

//using namespace std;

class ExifInfo
{
public:
    ExifInfo();
    bool getExifInfo(std::string& fileName);
    void reset();
    std::string makeString();

public:
    std::vector<std::string> m_exifKeyList;
    std::map<std::string, std::string> m_exifMap;

    std::string make;
    std::string Model;
    std::string DateTime;
    std::string ExposureTime;
    std::string FNumber;
    std::string Orientation;

    std::string PixelXDimension;
    std::string PixelYDimension;

    std::string GPSLatitudeRef;
    std::string GPSLatitude;
    std::string GPSLongitudeRef;
    std::string GPSLongitude;
    std::string GPSAltitudeRef;
    std::string GPSAltitude;

};
/*
{
    "ApertureValue": {"value": "193685/85136"},      //拍照时镜头的光圈. 单位是 APEX. 为了转换成普通的 F-number(F-stop), 则要先计算出根号2 2 (=1.4142)的ApertureValue次幂. 例如, 如果ApertureValue 是 '5', F-number 就等于1.41425 = F5.6.
    "BrightnessValue": {"value": "100718/10007"},    //被拍摄对象的明度, 单位是 APEX.
    "ColorSpace": {"value": "1"},                    //定义色彩空间
    "DateTime": {"value": "2019:08:16 15:54:15"},    //图像最后一次被修改时的日期/时间.
    "DateTimeDigitized": {"value": "2019:08:16 15:54:15"},//照片被数字化时的日期/时间. 通常, 它与DateTimeOriginal(0x9003)具有相同的值
    "DateTimeOriginal": {"value": "2019:08:16 15:54:15"},//照片在被拍下来的日期/时间. 使用用户的软件是不能被修改这个值的
    "ExifTag": {"value": "192"},
    "ExposureBiasValue": {"value": "0/1"}, //照片拍摄时的曝光补偿
    "ExposureMode": {"value": "0"},
    "ExposureProgram": {"value": "2"},
    "ExposureTime": {"value": "1/1721"}, //曝光时间 (快门速度的倒数). 单位是秒.
    "FNumber": {"value": "11/5"}, //拍照时的光圈F-number(F-stop).
    "FileSize": {"value": "241531"},
    "Flash": {"value": "24"},                  //'0' 表示闪光灯没有闪光, '1' 表示闪光灯闪光, '5' 表示闪光但没有检测反射光, '7' 表示闪光且检测了反射光.
    "FocalLength": {"value": "83/20"},       //拍摄照片时的镜头的焦距长度. 单位是毫米.
    "FocalLengthIn35mmFilm": {"value": "29"},
    "Format": {"value": "jpg"},
    "GPSLatitude": {"value": "31deg 55' 3.870\" "},
    "GPSLongitude": {"value": "117deg 18' 4.110\" "},
    "GPSTag": {"value": "699"},
    "ISOSpeedRatings": {"value": "32"},
    "ImageHeight": {"value": "1001"},
    "ImageWidth": {"value": "750"},
    "LensMake": {"value": "Apple"},
    "LensModel": {"value": "iPhone 6 back camera 4.15mm f/2.2"},
    "LensSpecification": {"value": "83/20 83/20 11/5 11/5"},
    "Make": {"value": "Apple"},
    "MeteringMode": {"value": "5"},
    "Model": {"value": "iPhone 6"},
    "Orientation": {"value": "1"},
    "PixelXDimension": {"value": "750"},
    "PixelYDimension": {"value": "1001"},
    "ResolutionUnit": {"value": "2"},
    "SceneType": {"value": "1"},
    "SensingMethod": {"value": "2"},
    "ShutterSpeedValue": {"value": "100881/9385"},
    "Software": {"value": "12.3"},
    "SubSecTimeDigitized": {"value": "039"},
    "SubSecTimeOriginal": {"value": "039"},
    "SubjectArea": {"value": "1631 1223 1795 1077"},
    "WhiteBalance": {"value": "0"},
    "XResolution": {"value": "144/1"},
    "YResolution": {"value": "144/1"}}
*/

/*
Exif.Image.Orientation
1:正常(上/左)
2:水平翻转(上/右)
3:旋转180度(下/右)
4:垂直翻转(下/左)
5:左/上
6:顺时针90度(右/上)
7:右下
8:逆时针90度(左下)
*/
/*
Exif.Image.Make                              0x010f Ascii       6  Apple
Exif.Image.Model                             0x0110 Ascii      14  iPhone 7 Plus
Exif.Image.Orientation                       0x0112 Short       1  6
Exif.Image.XResolution                       0x011a Rational    1  72/1
Exif.Image.YResolution                       0x011b Rational    1  72/1
Exif.Image.ResolutionUnit                    0x0128 Short       1  2
Exif.Image.Software                          0x0131 Ascii       5  14.5
Exif.Image.DateTime                          0x0132 Ascii      20  2021:08:14 10:56:20
Exif.Image.HostComputer                      0x013c Ascii      14  iPhone 7 Plus
Exif.Image.YCbCrPositioning                  0x0213 Short       1  1
Exif.Image.ExifTag                           0x8769 Long        1  234
Exif.Photo.ExposureTime                      0x829a Rational    1  1/11
Exif.Photo.FNumber                           0x829d Rational    1  9/5
Exif.Photo.ExposureProgram                   0x8822 Short       1  2
Exif.Photo.ISOSpeedRatings                   0x8827 Short       1  100
Exif.Photo.ExifVersion                       0x9000 Undefined   4  48 50 51 50
Exif.Photo.DateTimeOriginal                  0x9003 Ascii      20  2021:08:14 10:56:20
Exif.Photo.DateTimeDigitized                 0x9004 Ascii      20  2021:08:14 10:56:20
Exif.Photo.OffsetTime                        0x9010 Ascii       7  +08:00
Exif.Photo.OffsetTimeOriginal                0x9011 Ascii       7  +08:00
Exif.Photo.OffsetTimeDigitized               0x9012 Ascii       7  +08:00
Exif.Photo.ComponentsConfiguration           0x9101 Undefined   4  1 2 3 0
Exif.Photo.ShutterSpeedValue                 0x9201 SRational   1  47833/13769
Exif.Photo.ApertureValue                     0x9202 Rational    1  54823/32325
Exif.Photo.BrightnessValue                   0x9203 SRational   1  40168/39347
Exif.Photo.ExposureBiasValue                 0x9204 SRational   1  0/1
Exif.Photo.MeteringMode                      0x9207 Short       1  5
Exif.Photo.Flash                             0x9209 Short       1  16
Exif.Photo.FocalLength                       0x920a Rational    1  399/100
Exif.Photo.SubjectArea                       0x9214 Short       4  2015 1511 2217 1330

Exif.Photo.MakerNote                         0x927c Undefined

Exif.Photo.SubSecTimeOriginal                0x9291 Ascii       4  537
Exif.Photo.SubSecTimeDigitized               0x9292 Ascii       4  537
Exif.Photo.FlashpixVersion                   0xa000 Undefined   4  48 49 48 48
Exif.Photo.ColorSpace                        0xa001 Short       1  65535
Exif.Photo.PixelXDimension                   0xa002 Long        1  4032
Exif.Photo.PixelYDimension                   0xa003 Long        1  3024
Exif.Photo.SensingMethod                     0xa217 Short       1  2
Exif.Photo.SceneType                         0xa301 Undefined   1  1
Exif.Photo.ExposureMode                      0xa402 Short       1  0
Exif.Photo.WhiteBalance                      0xa403 Short       1  0
Exif.Photo.FocalLengthIn35mmFilm             0xa405 Short       1  28
Exif.Photo.SceneCaptureType                  0xa406 Short       1  0
Exif.Photo.LensSpecification                 0xa432 Rational    4  399/100 33/5 9/5 14/5
Exif.Photo.LensMake                          0xa433 Ascii       6  Apple
Exif.Photo.LensModel                         0xa434 Ascii      44  iPhone 7 Plus back dual camera 3.99mm f/1.8
Exif.Image.GPSTag                            0x8825 Long        1  2006
Exif.GPSInfo.GPSLatitudeRef                  0x0001 Ascii       2  N
Exif.GPSInfo.GPSLatitude                     0x0002 Rational    3  36/1 4/1 2634/100
Exif.GPSInfo.GPSLongitudeRef                 0x0003 Ascii       2  E
Exif.GPSInfo.GPSLongitude                    0x0004 Rational    3  120/1 22/1 5593/100
Exif.GPSInfo.GPSAltitudeRef                  0x0005 Byte        1  0
Exif.GPSInfo.GPSAltitude                     0x0006 Rational    1  87059/4119
Exif.GPSInfo.GPSSpeedRef                     0x000c Ascii       2  K
Exif.GPSInfo.GPSSpeed                        0x000d Rational    1  0/1
Exif.GPSInfo.GPSImgDirectionRef              0x0010 Ascii       2  T
Exif.GPSInfo.GPSImgDirection                 0x0011 Rational    1  291897/1747
Exif.GPSInfo.GPSDestBearingRef               0x0017 Ascii       2  T
Exif.GPSInfo.GPSDestBearing                  0x0018 Rational    1  291897/1747
Exif.GPSInfo.GPSDateStamp                    0x001d Ascii      11  2021:08:14
Exif.GPSInfo.GPSHPositioningError            0x001f Rational    1  65/1
*/


/*
Exif.Photo.SubSecTimeOriginal                0x9291 Ascii       4  011
Exif.Photo.SubSecTimeDigitized               0x9292 Ascii       4  011
Exif.Photo.FlashpixVersion                   0xa000 Undefined   4  48 49 48 48
Exif.Photo.ColorSpace                        0xa001 Short       1  65535
Exif.Photo.PixelXDimension                   0xa002 Long        1  4032
Exif.Photo.PixelYDimension                   0xa003 Long        1  3024
Exif.Photo.SensingMethod                     0xa217 Short       1  2
Exif.Photo.SceneType                         0xa301 Undefined   1  1
Exif.Photo.ExposureMode                      0xa402 Short       1  0
Exif.Photo.WhiteBalance                      0xa403 Short       1  0
Exif.Photo.FocalLengthIn35mmFilm             0xa405 Short       1  28
Exif.Photo.SceneCaptureType                  0xa406 Short       1  0
Exif.Photo.LensSpecification                 0xa432 Rational    4  4183519/1048501 33/5 9/5 14/5
Exif.Photo.LensMake                          0xa433 Ascii       6  Apple
Exif.Photo.LensModel                         0xa434 Ascii      44  iPhone 7 Plus back dual camera 3.99mm f/1.8
Exif.Image.GPSTag                            0x8825 Long        1  2116
Exif.GPSInfo.GPSLatitudeRef                  0x0001 Ascii       2  N
Exif.GPSInfo.GPSLatitude                     0x0002 Rational    3  36/1 14/1 2142/100
Exif.GPSInfo.GPSLongitudeRef                 0x0003 Ascii       2  E
Exif.GPSInfo.GPSLongitude                    0x0004 Rational    3  120/1 25/1 941/100
Exif.GPSInfo.GPSAltitudeRef                  0x0005 Byte        1  0
Exif.GPSInfo.GPSAltitude                     0x0006 Rational    1  501977/13854
Exif.GPSInfo.GPSSpeedRef                     0x000c Ascii       2  K
Exif.GPSInfo.GPSSpeed                        0x000d Rational    1  0/1
Exif.GPSInfo.GPSImgDirectionRef              0x0010 Ascii       2  T
Exif.GPSInfo.GPSImgDirection                 0x0011 Rational    1  495289/28519
Exif.GPSInfo.GPSDestBearingRef               0x0017 Ascii       2  T
Exif.GPSInfo.GPSDestBearing                  0x0018 Rational    1  495289/28519
Exif.GPSInfo.GPSHPositioningError            0x001f Rational    1  82175/4532
Exif.Thumbnail.Compression                   0x0103 Short       1  6
Exif.Thumbnail.XResolution                   0x011a Rational    1  72/1
Exif.Thumbnail.YResolution                   0x011b Rational    1  72/1
Exif.Thumbnail.ResolutionUnit                0x0128 Short       1  2
Exif.Thumbnail.JPEGInterchangeFormat         0x0201 Long        1  2460
Exif.Thumbnail.JPEGInterchangeFormatLength   0x0202 Long        1  7587
*/
#endif // EXIFINFO_H
