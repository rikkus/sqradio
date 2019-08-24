#include "errno.h"
#include "avcodec.h"

#ifndef MKTAG
#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))
#endif

// private structure used to hide all internal memory allocations
// and structures used for de/encoding - end user should
// never see any complicated structure
typedef struct
{
    AVCodec* avcodec;
    AVCodecContext avcontext;
} private_handle_t;

static private_handle_t* create_handle()
{
    private_handle_t* t = malloc(sizeof(private_handle_t));
    if (!t)
	return NULL;

    // register and fill
    avcodec_init();
    avcodec_register_all();
    return t;
}

static void destroy_handle(private_handle_t* handle)
{
    if (handle)
    {
	if (handle->avcodec)
	{
	    avcodec_close(&handle->avcontext);
	}
	free(handle);

        // count referencies
    }
}

int avcodec(void* handle, avc_cmd_t cmd, void* pout)
{
    switch (cmd)
    {
    case AVC_OPEN_BY_NAME:
	{
            // pin  char* codec name
	    private_handle_t* handle = create_handle();
	    (private_handle_t**)pout = handle;
	    if (!handle)
		return -ENOMEM;
	    if (!handle->avcodec)
	    {
		destroy_handle(handle);
		(private_handle_t**)pout = NULL;
		return -1;// better error
	    }
            return 0;
	}
    case AVC_OPEN_BY_CODEC_ID:
	{
            // pin  uint32_t codec fourcc
	    private_handle_t* handle = create_handle();
	    (private_handle_t**)pout = handle;
	    if (!handle)
		return -ENOMEM;

	    if (!handle->avcodec)
	    {
		destroy_handle(handle);
		(private_handle_t**)pout = NULL;
		return -1;// better error
	    }
            return 0;
	}

    case AVC_CLOSE:
	// uninit part
	// eventually close all allocated space if this was last
	// instance
	destroy_handle(handle);
	break;

    case AVC_FLUSH:
	break;

    case AVC_DECODE:
	break;

    case AVC_ENCODE:
	break;

    case AVC_GET_VERSION:
        (int*) pout = 500;
    default:
	return -1;

    }
    return 0;
}
