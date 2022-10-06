#include "SilkAudioCode.h"

#include "SKP_Silk_SDK_API.h"
#include "SKP_Silk_SigProc_FIX.h"

#if (defined(_WIN32) || defined(_WINCE))
#include <windows.h>	/* timer */
#else    // Linux or Mac
#include <sys/time.h>
#endif

/* Define codec specific settings should be moved to h file */
#define MAX_BYTES_PER_FRAME     1024
#define MAX_INPUT_FRAMES        5
#define MAX_FRAME_LENGTH        480
#define FRAME_LENGTH_MS         20
#define MAX_API_FS_KHZ          48
#define MAX_LBRR_DELAY          2

#define SILK_BUFFER_SIZE_BYTES 5120 /* MAX_BYTES * MAX_FRAMES */
#define SLIN_BUFFER_SIZE_BYTES 9600 /* 100 ms @ 48KHZ * 2 bytes  */

#ifdef _WIN32
unsigned long GetHighResolutionTime() /* O: time in usec*/
{
    /* Returns a time counter in microsec	*/
    /* the resolution is platform dependent */
    /* but is typically 1.62 us resolution  */
    LARGE_INTEGER lpPerformanceCount;
    LARGE_INTEGER lpFrequency;
    QueryPerformanceCounter(&lpPerformanceCount);
    QueryPerformanceFrequency(&lpFrequency);
    return (unsigned long)((1000000*(lpPerformanceCount.QuadPart)) / lpFrequency.QuadPart);
}
#else    // Linux or Mac
unsigned long GetHighResolutionTime() /* O: time in usec*/
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return((tv.tv_sec*1000000)+(tv.tv_usec));
}
#endif // _WIN32

#ifdef _SYSTEM_IS_BIG_ENDIAN
/* Function to convert a little endian int16 to a */
/* big endian int16 or vica verca                 */
void swap_endian(
        SKP_int16       vec[],              /*  I/O array of */
        SKP_int         len                 /*  I   length      */
        )
{
    SKP_int i;
    SKP_int16 tmp;
    SKP_uint8 *p1, *p2;

    for( i = 0; i < len; i++ )
    {
        tmp = vec[ i ];
        p1 = (SKP_uint8 *)&vec[ i ]; p2 = (SKP_uint8 *)&tmp;
        p1[ 0 ] = p2[ 1 ]; p1[ 1 ] = p2[ 0 ];
    }
}
#endif

struct DS_SilkCoder_pvt
{
    void* psEnc;
    SKP_SILK_SDK_EncControlStruct encControl;
    void* psDec;
    SKP_SILK_SDK_DecControlStruct decControl;
    int16_t buf[SLIN_BUFFER_SIZE_BYTES / 2];
};

static uint8_t ex_silk8[] = {
  0x29, 0xE4, 0xFD, 0x3E, 0x2D, 0xC8, 0x3C,
  0xEE, 0x23, 0xB9, 0x95, 0xF4, 0x35, 0x8A,
  0x18, 0xF2, 0xF2, 0x6D, 0x75, 0xBB, 0xA8,
  0x75, 0x27, 0x29, 0x35, 0x84, 0x95, 0x59,
  0xF7, 0x39, 0x43, 0x68, 0xE5, 0xA3, 0xA8,
  0x4A, 0xB8, 0xE3, 0xFD, 0xA6, 0x73, 0x3A,
  0xEA, 0x88, 0x82, 0x44, 0xC2, 0x64, 0x20,
  0xA7, 0x18, 0xCD, 0xA1
};

//static struct ast_frame *silk8_sample(void)
//{
//  static struct ast_frame f = {
//    .datalen = sizeof(ex_silk8),
//    .samples = 60 * 8000 / 1000,
//    .mallocd = 0,
//    .offset = 0,
//    .src = __PRETTY_FUNCTION__,
//    .data.ptr = ex_silk8
//  };
//  ast_format_set(&f.subclass.format, AST_FORMAT_SILK, 1,
//                 SILK_ATTR_KEY_SAMP_RATE,
//                 SILK_ATTR_VAL_SAMP_8KHZ,
//                 AST_FORMAT_ATTR_END);
//  return &f;

//}


SilkAudioCode::SilkAudioCode()
{
}

int SilkAudioCode::decodeFile(const std::string &inFileName, std::vector<uint16_t> &buffer)
{
    unsigned long tottime = 0, starttime = 0;
    double    filetime = 0;
    size_t    counter = 0;
    SKP_int32 totPackets = 0, i = 0, k = 0;
    SKP_int16 ret = 0, len = 0, tot_len = 0;
    SKP_int16 nBytes = 0;
    SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES * ( MAX_LBRR_DELAY + 1 ) ];
    SKP_uint8 *payloadEnd = nullptr, *payloadToDec = nullptr;
    SKP_uint8 FECpayload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES ], *payloadPtr;
    SKP_int16 nBytesFEC = 0;
    SKP_int16 nBytesPerPacket[ MAX_LBRR_DELAY + 1 ] = {0}, totBytes = 0, maxBytes = sizeof(payload);
    SKP_int16 out[ ( ( FRAME_LENGTH_MS * MAX_API_FS_KHZ ) << 1 ) * MAX_INPUT_FRAMES ], *outPtr = nullptr;
    FILE      *bitInFile = nullptr;
    SKP_int32 packetSize_ms = 0, API_Fs_Hz = 16000;
    SKP_int32 decSizeBytes = 0;
    void      *psDec = nullptr;
    SKP_float loss_prob = 0;
    SKP_int32 frames = 0, lost = 0, quiet = 0;
    SKP_SILK_SDK_DecControlStruct DecControl;
    /* Seed for the random number generator, which is used for simulating packet loss */
    static SKP_int32 rand_seed = 1;

    DecControl.API_sampleRate = API_Fs_Hz;

    /* Initialize to one frame per packet, for proper concealment before first packet arrives */
    DecControl.framesPerPacket = 1;

    bitInFile = fopen( inFileName.c_str(), "rb" );
    if(nullptr == bitInFile)
    {
        printf( "Error: could not open input file %s\n", inFileName.c_str() );
        return FileNoExitError;
    }

    /* Check Silk header */
    if (!isValidHeader(bitInFile))
    {
        return HeaderError;
    }

    /* Create decoder */
    ret = SKP_Silk_SDK_Get_Decoder_Size( &decSizeBytes );
    if( ret  != SKP_SILK_NO_ERROR)
    {
        printf( "\nSKP_Silk_SDK_Get_Decoder_Size returned %d", ret );
        return DecodeError;
    }

    psDec = malloc( decSizeBytes );

    /* Reset decoder */
    ret = SKP_Silk_SDK_InitDecoder( psDec );
    if( ret )
    {
        printf( "\nSKP_Silk_InitDecoder returned %d", ret );
    }

    totPackets = 0;
    tottime    = 0;
    payloadEnd = payload;

    /* Simulate the jitter buffer holding MAX_FEC_DELAY packets */
    for( i = 0; i < MAX_LBRR_DELAY; i++ )
    {
        /* Read payload size */
        counter = fread( &nBytes, sizeof( SKP_int16 ), 1, bitInFile );
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( &nBytes, 1 );
#endif
        /* Read payload */
        counter = fread( payloadEnd, sizeof( SKP_uint8 ), nBytes, bitInFile );

        if( ( SKP_int16 )counter < nBytes )
        {
            break;
        }

        nBytesPerPacket[ i ] = nBytes;
        payloadEnd          += nBytes;
        totPackets++;
    }

    while( 1 )
    {
        /* Read payload size */
        counter = fread( &nBytes, sizeof( SKP_int16 ), 1, bitInFile );
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( &nBytes, 1 );
#endif
        if( nBytes < 0 || counter < 1 )
        {
            break;
        }

        /* Read payload */
        counter = fread( payloadEnd, sizeof( SKP_uint8 ), nBytes, bitInFile );
        if( ( SKP_int16 )counter < nBytes )
        {
            break;
        }

        /* Simulate losses */
        rand_seed = SKP_RAND( rand_seed );
        if( ( ( ( float )( ( rand_seed >> 16 ) + ( 1 << 15 ) ) ) / 65535.0f >= ( loss_prob / 100.0f ) ) && ( counter > 0 ) )
        {
            nBytesPerPacket[ MAX_LBRR_DELAY ] = nBytes;
            payloadEnd                       += nBytes;
        }
        else
        {
            nBytesPerPacket[ MAX_LBRR_DELAY ] = 0;
        }

        if( nBytesPerPacket[ 0 ] == 0 )
        {
            /* Indicate lost packet */
            lost = 1;

            /* Packet loss. Search after FEC in next packets. Should be done in the jitter buffer */
            payloadPtr = payload;
            for( i = 0; i < MAX_LBRR_DELAY; i++ )
            {
                if( nBytesPerPacket[ i + 1 ] > 0 )
                {
                    starttime = GetHighResolutionTime();
                    SKP_Silk_SDK_search_for_LBRR( payloadPtr, nBytesPerPacket[ i + 1 ], ( i + 1 ), FECpayload, &nBytesFEC );
                    tottime += GetHighResolutionTime() - starttime;
                    if( nBytesFEC > 0 )
                    {
                        payloadToDec = FECpayload;
                        nBytes = nBytesFEC;
                        lost = 0;
                        break;
                    }
                }
                payloadPtr += nBytesPerPacket[ i + 1 ];
            }
        }
        else
        {
            lost = 0;
            nBytes = nBytesPerPacket[ 0 ];
            payloadToDec = payload;
        }

        /* Silk decoder */
        outPtr = out;
        tot_len = 0;
        starttime = GetHighResolutionTime();

        if( lost == 0 )
        {
            /* No Loss: Decode all frames in the packet */
            frames = 0;
            do
            {
                /* Decode 20 ms */
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 0, payloadToDec, nBytes, outPtr, &len );
                if( ret )
                {
                    printf( "\nSKP_Silk_SDK_Decode returned %d", ret );
                }

                frames++;
                outPtr  += len;
                tot_len += len;
                if( frames > MAX_INPUT_FRAMES )
                {
                    /* Hack for corrupt stream that could generate too many frames */
                    outPtr  = out;
                    tot_len = 0;
                    frames  = 0;
                }
                /* Until last 20 ms frame of packet has been decoded */
            } while( DecControl.moreInternalDecoderFrames );
        }
        else
        {
            /* Loss: Decode enough frames to cover one packet duration */
            for( i = 0; i < DecControl.framesPerPacket; i++ )
            {
                /* Generate 20 ms */
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 1, payloadToDec, nBytes, outPtr, &len );
                if( ret )
                {
                    printf( "\nSKP_Silk_Decode returned %d", ret );
                }
                outPtr  += len;
                tot_len += len;
            }
        }

        packetSize_ms = tot_len / ( DecControl.API_sampleRate / 1000 );
        tottime += GetHighResolutionTime() - starttime;
        totPackets++;

        /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( out, tot_len );
#endif
        for(int i = 0; i < tot_len; i++)
        {
            buffer.push_back(out[i]);
        }

        //fwrite( out, sizeof( SKP_int16 ), tot_len, speechOutFile );

        /* Update buffer */
        totBytes = 0;
        for( i = 0; i < MAX_LBRR_DELAY; i++ )
        {
            totBytes += nBytesPerPacket[ i + 1 ];
        }
        /* Check if the received totBytes is valid */
        if (totBytes < 0 || totBytes > maxBytes )
        {
            fprintf( stderr, "\rPackets decoded:             %d", totPackets );
            return WriteError;
        }

        SKP_memmove( payload, &payload[ nBytesPerPacket[ 0 ] ], totBytes * sizeof( SKP_uint8 ) );
        payloadEnd -= nBytesPerPacket[ 0 ];
        SKP_memmove( nBytesPerPacket, &nBytesPerPacket[ 1 ], MAX_LBRR_DELAY * sizeof( SKP_int16 ) );

        if( !quiet )
        {
            fprintf( stderr, "\rPackets decoded:             %d", totPackets );
        }
    }

    /* Empty the recieve buffer */
    for( k = 0; k < MAX_LBRR_DELAY; k++ )
    {
        if( nBytesPerPacket[ 0 ] == 0 )
        {
            /* Indicate lost packet */
            lost = 1;

            /* Packet loss. Search after FEC in next packets. Should be done in the jitter buffer */
            payloadPtr = payload;
            for( i = 0; i < MAX_LBRR_DELAY; i++ )
            {
                if( nBytesPerPacket[ i + 1 ] > 0 )
                {
                    starttime = GetHighResolutionTime();
                    SKP_Silk_SDK_search_for_LBRR( payloadPtr, nBytesPerPacket[ i + 1 ], ( i + 1 ), FECpayload, &nBytesFEC );
                    tottime += GetHighResolutionTime() - starttime;
                    if( nBytesFEC > 0 )
                    {
                        payloadToDec = FECpayload;
                        nBytes = nBytesFEC;
                        lost = 0;
                        break;
                    }
                }

                payloadPtr += nBytesPerPacket[ i + 1 ];
            }
        }
        else
        {
            lost = 0;
            nBytes = nBytesPerPacket[ 0 ];
            payloadToDec = payload;
        }

        /* Silk decoder */
        outPtr  = out;
        tot_len = 0;
        starttime = GetHighResolutionTime();

        if( lost == 0 )
        {
            /* No loss: Decode all frames in the packet */
            frames = 0;
            do
            {
                /* Decode 20 ms */
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 0, payloadToDec, nBytes, outPtr, &len );
                if( ret )
                {
                    printf( "\nSKP_Silk_SDK_Decode returned %d", ret );
                }

                frames++;
                outPtr  += len;
                tot_len += len;
                if( frames > MAX_INPUT_FRAMES )
                {
                    /* Hack for corrupt stream that could generate too many frames */
                    outPtr  = out;
                    tot_len = 0;
                    frames  = 0;
                }
                /* Until last 20 ms frame of packet has been decoded */
            } while( DecControl.moreInternalDecoderFrames );
        }
        else
        {
            /* Loss: Decode enough frames to cover one packet duration */

            /* Generate 20 ms */
            for( i = 0; i < DecControl.framesPerPacket; i++ )
            {
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 1, payloadToDec, nBytes, outPtr, &len );
                if( ret )
                {
                    printf( "\nSKP_Silk_Decode returned %d", ret );
                }
                outPtr  += len;
                tot_len += len;
            }
        }

        packetSize_ms = tot_len / ( DecControl.API_sampleRate / 1000 );
        tottime += GetHighResolutionTime() - starttime;
        totPackets++;

        /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( out, tot_len );
#endif
        for(int i = 0; i < tot_len; i++)
        {
            buffer.push_back(out[i]);
        }

        /* Update Buffer */
        totBytes = 0;
        for( i = 0; i < MAX_LBRR_DELAY; i++ )
        {
            totBytes += nBytesPerPacket[ i + 1 ];
        }

        /* Check if the received totBytes is valid */
        if (totBytes < 0 || totBytes > maxBytes )
        {
            fprintf( stderr, "\rPackets decoded:              %d", totPackets );
            return WriteError;
        }

        SKP_memmove( payload, &payload[ nBytesPerPacket[ 0 ] ], totBytes * sizeof( SKP_uint8 ) );
        payloadEnd -= nBytesPerPacket[ 0 ];
        SKP_memmove( nBytesPerPacket, &nBytesPerPacket[ 1 ], MAX_LBRR_DELAY * sizeof( SKP_int16 ) );

        if( !quiet )
        {
            fprintf( stderr, "\rPackets decoded:              %d", totPackets );
        }
    }

    printf( "\nDecoding Finished \n" );

    /* Free decoder */
    free( psDec );
    psDec = nullptr;

    /* Close files */
    fclose( bitInFile );

    filetime = totPackets * 1e-3 * packetSize_ms;

    if( !quiet )
    {
        printf("\nFile length:                 %.3f s", filetime);
        printf("\nTime for decoding:           %.3f s (%.3f%% of realtime)", 1e-6 * tottime, 1e-4 * tottime / filetime);
        printf("\n\n");
    }
    else
    {
        /* print time and % of realtime */
        printf( "%.3f %.3f %d\n", 1e-6 * tottime, 1e-4 * tottime / filetime, totPackets );
    }

    return NoError;
}


int SilkAudioCode::decode(const std::string &inFileName, const std::string &outFileName)
{
    std::vector<uint16_t> buffer;
    int nResult = decodeFile(inFileName, buffer);
    if(nResult == NoError)
    {
        size_t length = buffer.size();
        FILE *pOutFile = fopen( outFileName.c_str(), "wb" );
        if( nullptr == pOutFile )
        {
            printf( "Error: could not open output file %s\n", outFileName.c_str() );
            return FileNoExitError;
        }

        fwrite(buffer.data() , sizeof( SKP_int16 ), length, pOutFile );
    }

    return nResult;
}

int SilkAudioCode::encode(const std::string &inFileName, const std::string &outFileName)
{
    unsigned long tottime, starttime;
    double    filetime;
    size_t    counter;
    SKP_int32 k, totPackets, totActPackets, ret;
    SKP_int16 nBytes;
    double    sumBytes, sumActBytes, avg_rate, act_rate, nrg;
    SKP_uint8 payload[ MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES ];
    SKP_int16 in[ FRAME_LENGTH_MS * MAX_API_FS_KHZ * MAX_INPUT_FRAMES ];
    FILE      *bitOutFile = nullptr, *speechInFile = nullptr;
    SKP_int32 encSizeBytes;
    void      *psEnc;
#ifdef _SYSTEM_IS_BIG_ENDIAN
    SKP_int16 nBytes_LE;
#endif

    /* default settings */
    SKP_int32 API_fs_Hz = 16000;
    SKP_int32 max_internal_fs_Hz = 24000;
    SKP_int32 targetRate_bps = 25000;
    SKP_int32 smplsSinceLastPacket, packetSize_ms = 20;
    SKP_int32 frameSizeReadFromFile_ms = 20;
    SKP_int32 packetLoss_perc = 0;
#if LOW_COMPLEXITY_ONLY
    SKP_int32 complexity_mode = 0;
#else
    SKP_int32 complexity_mode = 2;
#endif
    SKP_int32 DTX_enabled = 0, INBandFEC_enabled = 0, quiet = 0, tencent = 0;
    SKP_SILK_SDK_EncControlStruct encControl; // Struct for input to encoder
    SKP_SILK_SDK_EncControlStruct encStatus;  // Struct for status of encoder

    /* If no max internal is specified, set to minimum of API fs and 24 kHz */
    if( max_internal_fs_Hz == 0 )
    {
        max_internal_fs_Hz = 16000;
        if( API_fs_Hz < max_internal_fs_Hz )
        {
            max_internal_fs_Hz = API_fs_Hz;
        }
    }

    /* Print options */
    if( !quiet )
    {
        printf("********** Silk Encoder (Fixed Point) v %s ********************\n", SKP_Silk_SDK_get_version());
        printf("********** Compiled for %d bit cpu ******************************* \n", (int)sizeof(void*) * 8 );
        printf( "Input:                          %s\n",     inFileName.c_str() );
        printf( "Output:                         %s\n",     outFileName.c_str() );
        printf( "API sampling rate:              %d Hz\n",  API_fs_Hz );
        printf( "Maximum internal sampling rate: %d Hz\n",  max_internal_fs_Hz );
        printf( "Packet interval:                %d ms\n",  packetSize_ms );
        printf( "Inband FEC used:                %d\n",     INBandFEC_enabled );
        printf( "DTX used:                       %d\n",     DTX_enabled );
        printf( "Complexity:                     %d\n",     complexity_mode );
        printf( "Target bitrate:                 %d bps\n", targetRate_bps );
    }

    /* Open files */
    speechInFile = fopen( inFileName.c_str(), "rb" );
    if( nullptr == speechInFile )
    {
        printf( "Error: could not open input file %s\n", inFileName.c_str() );
        return FileOpenError;
    }

    bitOutFile = fopen( outFileName.c_str(), "wb" );
    if( nullptr == bitOutFile )
    {
        printf( "Error: could not open output file %s\n", outFileName.c_str() );
        return FileOpenError;
    }

    /* Add Silk header to stream */
    {
        if( tencent ) {
            static const char Tencent_break[] = "";
            fwrite( Tencent_break, sizeof( char ), strlen( Tencent_break ), bitOutFile );
        }

        static const char Silk_header[] = "#!SILK_V3";
        fwrite( Silk_header, sizeof( char ), strlen( Silk_header ), bitOutFile );
    }

    /* Create Encoder */
    ret = SKP_Silk_SDK_Get_Encoder_Size( &encSizeBytes );
    if( ret != SKP_SILK_NO_ERROR)
    {
        printf( "\nError: SKP_Silk_create_encoder returned %d\n", ret );
        return EncodeError;
    }

    psEnc = malloc( encSizeBytes );

    /* Reset Encoder */
    ret = SKP_Silk_SDK_InitEncoder( psEnc, &encStatus );
    if( ret != SKP_SILK_NO_ERROR)
    {
        printf( "\nError: SKP_Silk_reset_encoder returned %d\n", ret );
        return EncodeError;
    }

    /* Set Encoder parameters */
    encControl.API_sampleRate        = API_fs_Hz;
    encControl.maxInternalSampleRate = max_internal_fs_Hz;
    encControl.packetSize            = ( packetSize_ms * API_fs_Hz ) / 1000;
    encControl.packetLossPercentage  = packetLoss_perc;
    encControl.useInBandFEC          = INBandFEC_enabled;
    encControl.useDTX                = DTX_enabled;
    encControl.complexity            = complexity_mode;
    encControl.bitRate               = ( targetRate_bps > 0 ? targetRate_bps : 0 );

    if( API_fs_Hz > MAX_API_FS_KHZ * 1000 || API_fs_Hz < 0 )
    {
        printf( "\nError: API sampling rate = %d out of range, valid range 8000 - 48000 \n \n", API_fs_Hz );
        return HeaderError;
    }

    tottime              = 0;
    totPackets           = 0;
    totActPackets        = 0;
    smplsSinceLastPacket = 0;
    sumBytes             = 0.0;
    sumActBytes          = 0.0;
    smplsSinceLastPacket = 0;

    while( 1 )
    {
        /* Read input from file */
        counter = fread( in, sizeof( SKP_int16 ), ( frameSizeReadFromFile_ms * API_fs_Hz ) / 1000, speechInFile );
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( in, counter );
#endif
        if( ( SKP_int )counter < ( ( frameSizeReadFromFile_ms * API_fs_Hz ) / 1000 ) ) {
            break;
        }

        /* max payload size */
        nBytes = MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES;

        starttime = GetHighResolutionTime();

        /* Silk Encoder */
        ret = SKP_Silk_SDK_Encode( psEnc, &encControl, in, (SKP_int16)counter, payload, &nBytes );
        if( ret )
        {
            printf( "\nSKP_Silk_Encode returned %d", ret );
        }

        tottime += GetHighResolutionTime() - starttime;

        /* Get packet size */
        packetSize_ms = ( SKP_int )( ( 1000 * ( SKP_int32 )encControl.packetSize ) / encControl.API_sampleRate );

        smplsSinceLastPacket += ( SKP_int )counter;

        if( ( ( 1000 * smplsSinceLastPacket ) / API_fs_Hz ) == packetSize_ms )
        {
            /* Sends a dummy zero size packet in case of DTX period  */
            /* to make it work with the decoder test program.        */
            /* In practice should be handled by RTP sequence numbers */
            totPackets++;
            sumBytes  += nBytes;
            nrg = 0.0;
            for( k = 0; k < ( SKP_int )counter; k++ )
            {
                nrg += in[ k ] * (double)in[ k ];
            }

            if( ( nrg / ( SKP_int )counter ) > 1e3 )
            {
                sumActBytes += nBytes;
                totActPackets++;
            }

            /* Write payload size */
#ifdef _SYSTEM_IS_BIG_ENDIAN
            nBytes_LE = nBytes;
            swap_endian( &nBytes_LE, 1 );
            fwrite( &nBytes_LE, sizeof( SKP_int16 ), 1, bitOutFile );
#else
            fwrite( &nBytes, sizeof( SKP_int16 ), 1, bitOutFile );
#endif

            /* Write payload */
            fwrite( payload, sizeof( SKP_uint8 ), nBytes, bitOutFile );

            smplsSinceLastPacket = 0;

            if( !quiet )
            {
                fprintf( stderr, "\rPackets encoded:                %d", totPackets );
            }
        }
    }

    /* Write dummy because it can not end with 0 bytes */
    nBytes = -1;

    /* Write payload size */
    if( !tencent )
    {
        fwrite( &nBytes, sizeof( SKP_int16 ), 1, bitOutFile );
    }

    /* Free Encoder */
    free( psEnc );

    fclose( speechInFile );
    fclose( bitOutFile );

    filetime  = totPackets * 1e-3 * packetSize_ms;
    avg_rate  = 8.0 / packetSize_ms * sumBytes       / totPackets;
    act_rate  = 8.0 / packetSize_ms * sumActBytes    / totActPackets;
    if( !quiet )
    {
        printf( "\nFile length:                    %.3f s", filetime );
        printf( "\nTime for encoding:              %.3f s (%.3f%% of realtime)", 1e-6 * tottime, 1e-4 * tottime / filetime );
        printf( "\nAverage bitrate:                %.3f kbps", avg_rate  );
        printf( "\nActive bitrate:                 %.3f kbps", act_rate  );
        printf( "\n\n" );
    }
    else
    {
        /* print time and % of realtime */
        printf("%.3f %.3f %d ", 1e-6 * tottime, 1e-4 * tottime / filetime, totPackets );
        /* print average and active bitrates */
        printf( "%.3f %.3f \n", avg_rate, act_rate );
    }

    return NoError;
}

void SilkAudioCode::printSilkError(const uint16_t &ret)
{
    std::string message;
    switch (ret)
    {
    case SKP_SILK_NO_ERROR: message = "No errors";
        break;
    case SKP_SILK_ENC_INPUT_INVALID_NO_OF_SAMPLES:
        message = "Input length is not multiplum of 10 ms, or length is longer than the packet length";
        break;
    case SKP_SILK_ENC_FS_NOT_SUPPORTED:
        message = "Sampling frequency not 8000 , 12000, 16000 or 24000 Hertz";
        break;
    case SKP_SILK_ENC_PACKET_SIZE_NOT_SUPPORTED:
        message ="Packet size not 20, 40 , 60 , 80 or 100 ms ";
        break;
    case SKP_SILK_ENC_PAYLOAD_BUF_TOO_SHORT:
        message = "Allocated payload buffer too short";
        break;
    case SKP_SILK_ENC_INVALID_LOSS_RATE:
        message = " Loss rate not between  0 and 100 % ";
        break;
    case SKP_SILK_ENC_INVALID_COMPLEXITY_SETTING:
        message = "Complexity setting not valid, use 0 ,1 or 2";
        break;
    case SKP_SILK_ENC_INVALID_INBAND_FEC_SETTING:
        message = "Inband FEC setting not valid, use 0 or 1	";
        break;
    case SKP_SILK_ENC_INVALID_DTX_SETTING:
        message = "DTX setting not valid, use 0 or 1";
        break;
    case SKP_SILK_ENC_INTERNAL_ERROR:
        message = "Internal Encoder Error ";
        break;
    case SKP_SILK_DEC_INVALID_SAMPLING_FREQUENCY:
        message = "Output sampling frequency lower than internal decoded sampling frequency";
        break;
    case SKP_SILK_DEC_PAYLOAD_TOO_LARGE:
        message = "Payload size exceeded the maximum allowed 1024 bytes";
        break;
    case  SKP_SILK_DEC_PAYLOAD_ERROR	:
        message = "Payload has bit errors";
        break;
    default:
        message = "unknown";
        break;
    }

    printf("Silk Error: %s\n", message.c_str());
}

bool SilkAudioCode::isValidHeader(FILE *bitInFile)
{
    if(nullptr == bitInFile)
    {
        return false;
    }

    fseek(bitInFile, 0L, SEEK_SET);

    char te = '\u0002';//STX

    size_t    counter = 0;
    /* Check Silk header */
    char header_buf[ 50 ];
    fread(header_buf, sizeof(char), 1, bitInFile);
    header_buf[strlen("")] = '\0';/* Terminate with a null character */

    if(memchr(header_buf, te, 0) == nullptr)
    //if (strcmp(header_buf, te) != 0)
    {
        counter = fread( header_buf, sizeof( char ), strlen( "!SILK_V3" ), bitInFile );
        header_buf[ strlen( "!SILK_V3" ) ] = '\0'; /* Terminate with a null character */
        if( strcmp( header_buf, "!SILK_V3" ) != 0 )
        {
            /* Non-equal strings */
            printf("Error: Wrong Header %s\n", header_buf);
            return false;
        }
    }
    else
    {
        counter = fread( header_buf, sizeof( char ), strlen( "#!SILK_V3" ), bitInFile );
        header_buf[ strlen( "#!SILK_V3" ) ] = '\0'; /* Terminate with a null character */
        if( strcmp( header_buf, "#!SILK_V3" ) != 0 )
        {
            /* Non-equal strings */
            printf( "Error: Wrong Header %s\n", header_buf );
            return false;
        }
    }

    return true;
}
