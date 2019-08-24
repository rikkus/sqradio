#ifndef AVCODEC_H
#define AVCODEC_H

#include "common.h"

enum CodecID {
    CODEC_ID_NONE, 
    CODEC_ID_MP3LAME,
};
#define CODEC_ID_MSMPEG4 CODEC_ID_MSMPEG4V3

enum CodecType {
    CODEC_TYPE_UNKNOWN = -1,
    CODEC_TYPE_AUDIO,
};

/* in bytes */
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 18432
#define FF_MAX_B_FRAMES 4

#define FRAME_RATE_BASE 10000

typedef struct AVCodecContext {
    /**
     * the average bitrate
     * encoding: set by user. unused for constant quantizer encoding
     * decoding: set by lavc. 0 or some bitrate if this info is available in the stream 
     */
    int bit_rate;

   /* audio only */
    int sample_rate; /* samples per sec */
    int channels;

    /* the following data should not be initialized */
    int frame_size;     /* in samples, initialized when calling 'init' */
    int frame_number;   /* audio or video frame number */
                           
    /**
     * 1 -> keyframe, 0-> not
     * encoding: set by lavc (for the outputed bitstream, not the input frame)
     * decoding: set by lavc (for the decoded  bitstream, not the displayed frame)
     */
    int key_frame;
    
    /* encoding parameters */
    /**
     * quality (between 1 (good) and 31 (bad)) 
     * encoding: set by user if CODEC_FLAG_QSCALE is set otherwise set by lavc
     * decoding: set by lavc
     */
    int quality;      /* quality of the previous encoded frame 
                         this is allso used to set the quality in vbr mode*/
    
    struct AVCodec *codec;
    
    void *priv_data;

    char codec_name[32];
    enum CodecType codec_type; /* see CODEC_TYPE_xxx */
    enum CodecID codec_id; /* see CODEC_ID_xxx */
    unsigned int codec_tag;  /* codec tag, only used if unknown codec */
   
#ifndef MBC
#define MBC 128
#define MBR 96
#endif
#define QP_TYPE int //FIXME note xxx this might be changed to int8_t

    QP_TYPE *quant_store; /* field for communicating with external postprocessing */

} AVCodecContext;

typedef struct AVCodec {
    char *name;
    int type;
    int id;
    int priv_data_size;
    int (*init)(AVCodecContext *);
    int (*decode)(AVCodecContext *, void *outdata, int *outdata_size,
                  UINT8 *buf, int buf_size);
    struct AVCodec *next;
} AVCodec;

/* three components are given, that's all */
typedef struct AVPicture {
    UINT8 *data[3];
    int linesize[3];
} AVPicture;

extern AVCodec mp3_decoder;

/* resample.c */

struct ReSampleContext;

typedef struct ReSampleContext ReSampleContext;

ReSampleContext *audio_resample_init(int output_channels, int input_channels, 
                                     int output_rate, int input_rate);
int audio_resample(ReSampleContext *s, short *output, short *input, int nb_samples);
void audio_resample_close(ReSampleContext *s);

/* external high level API */

extern AVCodec *first_avcodec;

void avcodec_init(void);

void avcodec_set_bit_exact(void);

void register_avcodec(AVCodec *format);
AVCodec *avcodec_find_decoder(enum CodecID id);
AVCodec *avcodec_find_decoder_by_name(const char *name);
void avcodec_string(char *buf, int buf_size, AVCodecContext *enc);

AVCodecContext *avcodec_alloc_context(void);
int avcodec_open(AVCodecContext *avctx, AVCodec *codec);
int avcodec_decode_audio(AVCodecContext *avctx, INT16 *samples, 
                         int *frame_size_ptr,
                         UINT8 *buf, int buf_size);
int avcodec_parse_frame(AVCodecContext *avctx, UINT8 **pdata, 
                        int *data_size_ptr,
                        UINT8 *buf, int buf_size);

int avcodec_close(AVCodecContext *avctx);

void avcodec_register_all(void);

void avcodec_flush_buffers(AVCodecContext *avctx);

#ifdef FF_POSTPROCESS
extern int quant_store[MBR+1][MBC+1]; // [Review]
#endif


/**
 * Interface for 0.5.0 version
 *
 * do not even think about it's usage for this moment
 */

typedef struct {
    // compressed size used from given memory buffer
    int size;
    /// I/P/B frame type
    int frame_type;
} avc_enc_result_t;

/**
 * Commands
 * order can't be changed - once it was defined
 */
typedef enum {
    // general commands
    AVC_OPEN_BY_NAME = 0xACA000,
    AVC_OPEN_BY_CODEC_ID,
    AVC_OPEN_BY_FOURCC,
    AVC_CLOSE,

    AVC_FLUSH,
    // pin - struct { uint8_t* src, uint_t src_size }
    // pout - struct { AVPicture* img, consumed_bytes,
    AVC_DECODE,
    // pin - struct { AVPicture* img, uint8_t* dest, uint_t dest_size }
    // pout - uint_t used_from_dest_size
    AVC_ENCODE, 

    // query/get video commands
    AVC_GET_VERSION = 0xACB000,
    AVC_GET_WIDTH,
    AVC_GET_HEIGHT,
    AVC_GET_DELAY,
    AVC_GET_QUANT_TABLE,
    // ...

    // query/get audio commands
    AVC_GET_FRAME_SIZE = 0xABC000,

    // maybe define some simple structure which
    // might be passed to the user - but they can't
    // contain any codec specific parts and these
    // calls are usualy necessary only few times

    // set video commands
    AVC_SET_WIDTH = 0xACD000,
    AVC_SET_HEIGHT,

    // set video encoding commands
    AVC_SET_FRAME_RATE = 0xACD800,
    AVC_SET_QUALITY,
    AVC_SET_HURRY_UP,

    // set audio commands
    AVC_SET_SAMPLE_RATE = 0xACE000,
    AVC_SET_CHANNELS,

} avc_cmd_t;

/**
 * \param handle  allocated private structure by libavcodec
 *                for initialization pass NULL - will be returned pout
 *                user is supposed to know nothing about its structure
 * \param cmd     type of operation to be performed
 * \param pint    input parameter
 * \param pout    output parameter
 *
 * \returns  command status - eventually for query command it might return
 * integer resulting value
 */
int avcodec(void* handle, avc_cmd_t cmd, void* pout);

/* memory */
void *av_malloc(int size);
void *av_mallocz(int size);
void av_free(void *ptr);
void __av_freep(void **ptr);
#define av_freep(p) __av_freep((void **)(p))

#endif /* AVCODEC_H */
