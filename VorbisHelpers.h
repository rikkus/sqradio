// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef VORBIS_HELPERS_H
#define VORBIS_HELPERS_H

#include <sys/types.h>

size_t  fread_func(void *, size_t, size_t, void *);
int     fseek_func(void *, int64_t, int);
int     fclose_func(void *);
long    ftell_func(void *);

#endif
