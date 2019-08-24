TARGET       = ffmp3
TEMPLATE     = lib
CONFIG      += staticlib
CONFIG      -= debug
CONFIG      -= qt

QMAKE_CFLAGS += -O3 -Wall -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE

SOURCES +=\
allcodecs.c\
avcodec.c\
common.c\
mem.c\
mpegaudio.c\
mpegaudiodec.c\
utils.c
