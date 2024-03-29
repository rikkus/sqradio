/*
 * Common bit i/o utils
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
 *
 * alternative bitstream reader & writer by Michael Niedermayer <michaelni@gmx.at>
 */
#include "avcodec.h"

const UINT8 ff_sqrt_tab[128]={
        0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11
};

/* return the number of bits output */
INT64 get_bit_count(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    return s->data_out_size * 8 + s->index;
#else
    return (s->buf_ptr - s->buf + s->data_out_size) * 8 + 32 - (INT64)s->bit_left;
#endif
}

void align_put_bits(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    put_bits(s,(  - s->index) & 7,0);
#else
    put_bits(s,s->bit_left & 7,0);
#endif
}

/* pad the end of the output stream with zeros */
void flush_put_bits(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    align_put_bits(s);
#else
    s->bit_buf<<= s->bit_left;
    while (s->bit_left < 32) {
        /* XXX: should test end of buffer */
        *s->buf_ptr++=s->bit_buf >> 24;
        s->bit_buf<<=8;
        s->bit_left+=8;
    }
    s->bit_left=32;
    s->bit_buf=0;
#endif
}

void put_string(PutBitContext * pbc, char *s)
{
    while(*s){
        put_bits(pbc, 8, *s);
        s++;
    }
    put_bits(pbc, 8, 0);
}

/* bit input functions */

void init_get_bits(GetBitContext *s,
                   UINT8 *buffer, int buffer_size)
{
    s->buffer= buffer;
    s->size= buffer_size;
    s->buffer_end= buffer + buffer_size;
#ifdef ALT_BITSTREAM_READER
    s->index=0;
#elif defined LIBMPEG2_BITSTREAM_READER
    s->buffer_ptr = buffer;
    s->bit_count = 16;
    s->cache = 0;
#elif defined A32_BITSTREAM_READER
    s->buffer_ptr = (uint32_t*)buffer;
    s->bit_count = 32;
    s->cache0 = 0;
    s->cache1 = 0;
#endif
    {
        OPEN_READER(re, s)
        UPDATE_CACHE(re, s)
//        UPDATE_CACHE(re, s)
        CLOSE_READER(re, s)
    }
#ifdef A32_BITSTREAM_READER
    s->cache1 = 0;
#endif
}

void align_get_bits(GetBitContext *s)
{
    int n= (-get_bits_count(s)) & 7;
    if(n) skip_bits(s, n);
}

int check_marker(GetBitContext *s, char *msg)
{
    int bit= get_bits1(s);
    if(!bit) printf("Marker bit missing %s\n", msg);

    return bit;
}

/* VLC decoding */

//#define DEBUG_VLC

#define GET_DATA(v, table, i, wrap, size) \
{\
    UINT8 *ptr = (UINT8 *)table + i * wrap;\
    switch(size) {\
    case 1:\
        v = *(UINT8 *)ptr;\
        break;\
    case 2:\
        v = *(UINT16 *)ptr;\
        break;\
    default:\
        v = *(UINT32 *)ptr;\
        break;\
    }\
}


static int alloc_table(VLC *vlc, int size)
{
    int index;
    index = vlc->table_size;
    vlc->table_size += size;
    if (vlc->table_size > vlc->table_allocated) {
        vlc->table_allocated += (1 << vlc->bits);
        vlc->table = realloc(vlc->table,
                             sizeof(VLC_TYPE) * 2 * vlc->table_allocated);
        if (!vlc->table)
            return -1;
    }
    return index;
}

static int build_table(VLC *vlc, int table_nb_bits,
                       int nb_codes,
                       const void *bits, int bits_wrap, int bits_size,
                       const void *codes, int codes_wrap, int codes_size,
                       UINT32 code_prefix, int n_prefix)
{
    int i, j, k, n, table_size, table_index, nb, n1, index;
    UINT32 code;
    VLC_TYPE (*table)[2];

    table_size = 1 << table_nb_bits;
    table_index = alloc_table(vlc, table_size);
#ifdef DEBUG_VLC
    printf("new table index=%d size=%d code_prefix=%x n=%d\n",
           table_index, table_size, code_prefix, n_prefix);
#endif
    if (table_index < 0)
        return -1;
    table = &vlc->table[table_index];

    for(i=0;i<table_size;i++) {
        table[i][1] = 0; //bits
        table[i][0] = -1; //codes
    }

    /* first pass: map codes and compute auxillary table sizes */
    for(i=0;i<nb_codes;i++) {
        GET_DATA(n, bits, i, bits_wrap, bits_size);
        GET_DATA(code, codes, i, codes_wrap, codes_size);
        /* we accept tables with holes */
        if (n <= 0)
            continue;
#if defined(DEBUG_VLC) && 0
        printf("i=%d n=%d code=0x%x\n", i, n, code);
#endif
        /* if code matches the prefix, it is in the table */
        n -= n_prefix;
        if (n > 0 && (code >> n) == code_prefix) {
            if (n <= table_nb_bits) {
                /* no need to add another table */
                j = (code << (table_nb_bits - n)) & (table_size - 1);
                nb = 1 << (table_nb_bits - n);
                for(k=0;k<nb;k++) {
#ifdef DEBUG_VLC
                    printf("%4x: code=%d n=%d\n",
                           j, i, n);
#endif
                    if (table[j][1] /*bits*/ != 0) {
                        fprintf(stderr, "incorrect codes\n");
                        exit(1);
                    }
                    table[j][1] = n; //bits
                    table[j][0] = i; //code
                    j++;
                }
            } else {
                n -= table_nb_bits;
                j = (code >> n) & ((1 << table_nb_bits) - 1);
#ifdef DEBUG_VLC
                printf("%4x: n=%d (subtable)\n",
                       j, n);
#endif
                /* compute table size */
                n1 = -table[j][1]; //bits
                if (n > n1)
                    n1 = n;
                table[j][1] = -n1; //bits
            }
        }
    }

    /* second pass : fill auxillary tables recursively */
    for(i=0;i<table_size;i++) {
        n = table[i][1]; //bits
        if (n < 0) {
            n = -n;
            if (n > table_nb_bits) {
                n = table_nb_bits;
                table[i][1] = -n; //bits
            }
            index = build_table(vlc, n, nb_codes,
                                bits, bits_wrap, bits_size,
                                codes, codes_wrap, codes_size,
                                (code_prefix << table_nb_bits) | i,
                                n_prefix + table_nb_bits);
            if (index < 0)
                return -1;
            /* note: realloc has been done, so reload tables */
            table = &vlc->table[table_index];
            table[i][0] = index; //code
        }
    }
    return table_index;
}


/* Build VLC decoding tables suitable for use with get_vlc().

   'nb_bits' set thee decoding table size (2^nb_bits) entries. The
   bigger it is, the faster is the decoding. But it should not be too
   big to save memory and L1 cache. '9' is a good compromise.
   
   'nb_codes' : number of vlcs codes

   'bits' : table which gives the size (in bits) of each vlc code.

   'codes' : table which gives the bit pattern of of each vlc code.

   'xxx_wrap' : give the number of bytes between each entry of the
   'bits' or 'codes' tables.

   'xxx_size' : gives the number of bytes of each entry of the 'bits'
   or 'codes' tables.

   'wrap' and 'size' allows to use any memory configuration and types
   (byte/word/long) to store the 'bits' and 'codes' tables.  
*/
int init_vlc(VLC *vlc, int nb_bits, int nb_codes,
             const void *bits, int bits_wrap, int bits_size,
             const void *codes, int codes_wrap, int codes_size)
{
    vlc->bits = nb_bits;
    vlc->table = NULL;
    vlc->table_allocated = 0;
    vlc->table_size = 0;
#ifdef DEBUG_VLC
    printf("build table nb_codes=%d\n", nb_codes);
#endif

    if (build_table(vlc, nb_bits, nb_codes,
                    bits, bits_wrap, bits_size,
                    codes, codes_wrap, codes_size,
                    0, 0) < 0) {
        av_free(vlc->table);
        return -1;
    }
    return 0;
}


void free_vlc(VLC *vlc)
{
    av_free(vlc->table);
}

int ff_gcd(int a, int b){
    if(b) return ff_gcd(b, a%b);
    else  return a;
}
