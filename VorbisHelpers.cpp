// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <unistd.h>

#include "VorbisHelpers.h"
#include "QThreadPipe.h"

  size_t
fread_func(void * ptr, size_t size, size_t n, void * stream)
{
  QThreadPipe * pipe = reinterpret_cast<QThreadPipe *>(stream);

  QByteArray a = pipe->readFromMainThread(size * n);

  memcpy(ptr, a.data(), a.size());

  return a.size();
}

  int
fseek_func(void * /* stream */, int64_t /* offset */, int /* whence */)
{
  return -1;
}

  int
fclose_func(void * /* stream */)
{
  d->pipe->subThreadFinished();
  return 1;
}

  long
ftell_func(void * /* stream */)
{
  return -1;
}

