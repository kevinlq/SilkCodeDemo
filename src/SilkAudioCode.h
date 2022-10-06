#pragma once

#include <string>
#include <vector>

/*! \brief Various codec settings */
struct silk_codec_settings
{
    int32_t useinbandfec;
    int32_t usedtx;
    int32_t maxaveragebitrate;
    int32_t plpct;
};

/*! an abstract handle of a fmtp parsed by codec */
struct DSCodecFmtp
{
    /*! actual samples transferred per second for those who are not moron g722 RFC writers */
    uint32_t actual_samples_per_second = 2000;
    /*! bits transferred per second */
    int bits_per_second;
    /*! number of microseconds of media in one packet (ptime * 1000) */
    int microseconds_per_packet;
    /*! stereo  */
    int stereo = 1;
    /*! silk_codec_settings */
    silk_codec_settings default_codec_settings = {
        /*.useinbandfec */ 1,
        /*.usedtx */ 0,
        /*.maxaveragebitrate */ 0,
        /*.plpct */ 20, // 20% for now

    };

    std::string inFileName = "";
    double FileTime = 0;
    double decodTime = 0;
    std::vector<uint16_t> outBuffer;
};

class SilkAudioCode
{
public:

    enum FileErrorCode
    {
        NoError = 0,            // ok
        FileNoExitError,        // file not exit
        HeaderError,            // file header error
        FileOpenError,
        ReadError,
        WriteError,
        DecodeError,
        EncodeError
    };

    enum SilkSampleType
    {
        SilkSample8KHZ = 0,         // 8000
        SilkSample12KHZ,            // 12000
        SilkSample16KHZ,            // 16000
        SilkSample24KHZ,            // 24000
        SilkSampleSize
    };

    SilkAudioCode();

    /*! decode silk audio file to buffer*/
    int decodeFile(const std::string &inFileName, std::vector<uint16_t> &buffer);
    int decode(const std::string &inFileName, const std::string &outFileName);

    int encode(const std::string &inFileName, const std::string &outFileName);

protected:
    void printSilkError(const uint16_t &ret);
    bool isValidHeader(FILE *bitInFile);
};

