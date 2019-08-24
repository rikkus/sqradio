/*
 * The simplest mpeg audio layer 2 encoder
 * Copyright (c) 2000, 2001 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "avcodec.h"
#include "mpegaudio.h"

/* currently, cannot change these constants (need to modify
   quantization stage) */
#define FRAC_BITS 15
#define WFRAC_BITS  14
#define MUL(a,b) (((INT64)(a) * (INT64)(b)) >> FRAC_BITS)
#define FIX(a)   ((int)((a) * (1 << FRAC_BITS)))

#define SAMPLES_BUF_SIZE 4096

typedef struct MpegAudioContext {
    PutBitContext pb;
    int nb_channels;
    int freq, bit_rate;
    int lsf;           /* 1 if mpeg2 low bitrate selected */
    int bitrate_index; /* bit rate */
    int freq_index;
    int frame_size; /* frame size, in bits, without padding */
    INT64 nb_samples; /* total number of samples encoded */
    /* padding computation */
    int frame_frac, frame_frac_incr, do_padding;
    short samples_buf[MPA_MAX_CHANNELS][SAMPLES_BUF_SIZE]; /* buffer for filter */
    int samples_offset[MPA_MAX_CHANNELS];       /* offset in samples_buf */
    int sb_samples[MPA_MAX_CHANNELS][3][12][SBLIMIT];
    unsigned char scale_factors[MPA_MAX_CHANNELS][SBLIMIT][3]; /* scale factors */
    /* code to group 3 scale factors */
    unsigned char scale_code[MPA_MAX_CHANNELS][SBLIMIT];       
    int sblimit; /* number of used subbands */
    const unsigned char *alloc_table;
} MpegAudioContext;

/* define it to use floats in quantization (I don't like floats !) */
//#define USE_FLOATS

#include "mpegaudiotab.h"

int MPA_encode_init(AVCodecContext *avctx)
{
    MpegAudioContext *s = avctx->priv_data;
    int freq = avctx->sample_rate;
    int bitrate = avctx->bit_rate;
    int channels = avctx->channels;
    int i, v, table;
    float a;

    if (channels > 2)
        return -1;
    bitrate = bitrate / 1000;
    s->nb_channels = channels;
    s->freq = freq;
    s->bit_rate = bitrate * 1000;
    avctx->frame_size = MPA_FRAME_SIZE;
    avctx->key_frame = 1; /* always key frame */

    /* encoding freq */
    s->lsf = 0;
    for(i=0;i<3;i++) {
        if (mpa_freq_tab[i] == freq) 
            break;
        if ((mpa_freq_tab[i] / 2) == freq) {
            s->lsf = 1;
            break;
        }
    }
    if (i == 3)
        return -1;
    s->freq_index = i;

    /* encoding bitrate & frequency */
    for(i=0;i<15;i++) {
        if (mpa_bitrate_tab[s->lsf][1][i] == bitrate) 
            break;
    }
    if (i == 15)
        return -1;
    s->bitrate_index = i;

    /* compute total header size & pad bit */
    
    a = (float)(bitrate * 1000 * MPA_FRAME_SIZE) / (freq * 8.0);
    s->frame_size = ((int)a) * 8;

    /* frame fractional size to compute padding */
    s->frame_frac = 0;
    s->frame_frac_incr = (int)((a - floor(a)) * 65536.0);
    
    /* select the right allocation table */
    table = l2_select_table(bitrate, s->nb_channels, freq, s->lsf);

    /* number of used subbands */
    s->sblimit = sblimit_table[table];
    s->alloc_table = alloc_tables[table];

#ifdef DEBUG
    printf("%d kb/s, %d Hz, frame_size=%d bits, table=%d, padincr=%x\n", 
           bitrate, freq, s->frame_size, table, s->frame_frac_incr);
#endif

    for(i=0;i<s->nb_channels;i++)
        s->samples_offset[i] = 0;

    for(i=0;i<257;i++) {
        int v;
        v = mpa_enwindow[i];
#if WFRAC_BITS != 16
        v = (v + (1 << (16 - WFRAC_BITS - 1))) >> (16 - WFRAC_BITS);
#endif
        filter_bank[i] = v;
        if ((i & 63) != 0)
            v = -v;
        if (i != 0)
            filter_bank[512 - i] = v;
    }

    for(i=0;i<64;i++) {
        v = (int)(pow(2.0, (3 - i) / 3.0) * (1 << 20));
        if (v <= 0)
            v = 1;
        scale_factor_table[i] = v;
#ifdef USE_FLOATS
        scale_factor_inv_table[i] = pow(2.0, -(3 - i) / 3.0) / (float)(1 << 20);
#else
#define P 15
        scale_factor_shift[i] = 21 - P - (i / 3);
        scale_factor_mult[i] = (1 << P) * pow(2.0, (i % 3) / 3.0);
#endif
    }
    for(i=0;i<128;i++) {
        v = i - 64;
        if (v <= -3)
            v = 0;
        else if (v < 0)
            v = 1;
        else if (v == 0)
            v = 2;
        else if (v < 3)
            v = 3;
        else 
            v = 4;
        scale_diff_table[i] = v;
    }

    for(i=0;i<17;i++) {
        v = quant_bits[i];
        if (v < 0) 
            v = -v;
        else
            v = v * 3;
        total_quant_bits[i] = 12 * v;
    }

    return 0;
}

#undef FIX
