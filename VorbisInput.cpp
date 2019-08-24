/*
  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h> // For memset.

#include <vorbis/vorbisfile.h>

#include <qfile.h>
#include <qfileinfo.h>

#include "PlayList.h"
#include "Defines.h"
#include "VorbisInput.h"
#include "VorbisHelpers.h"
#include "BufferItem.h"

static int vorbisSection;
static OggVorbis_File * vorbisFile;

VorbisInput::VorbisInput(QObject * parent)
	: Input					(parent),
		trackLength_	(0),
		sampleRate_		(-1),
		channels_			(-1)
{
	vorbisFile = new OggVorbis_File;
	memset(vorbisFile, 0, sizeof(*vorbisFile));
}

VorbisInput::~VorbisInput()
{
	ov_clear(vorbisFile);
	delete vorbisFile;
	vorbisFile = 0;
}

	void
VorbisInput::_seek(long sec)
{
	if (!currentTrack())
		return;

	if (long(ov_time_total(vorbisFile, -1)) == sec)
		--sec;

	if (0 == sec)
		ov_time_seek(vorbisFile, sec);
	else
		ov_time_seek_page(vorbisFile, sec);
}

	void
VorbisInput::_changeTrack(const QString & id, const QString & filename)
{
	ov_clear(vorbisFile);

	file_.setName(QFile::encodeName(filename));

	if (!file_.open(IO_ReadOnly))
	{
		qWarning("Could not open file `%s'", filename.ascii());
		return;
	}

	// Mad hack.
	char buf[4096];
	while (!file_.atEnd())
	{
		file_.readBlock(buf, 4096);
	}
	file_.at(0);

	ov_callbacks callbacks;

	callbacks.read_func		= fread_func;
	callbacks.seek_func		= fseek_func;
	callbacks.close_func	= fclose_func;
	callbacks.tell_func		= ftell_func;

	if (ov_open_callbacks(&file_, vorbisFile, NULL, 0, callbacks) < 0)
	{
		qWarning("Could not ov_open_callbacks");
		return;
	}

	trackLength_ = long(ov_time_total(vorbisFile, -1));

	vorbisSection = 0;
	setCurrentTrack(id);

	vorbis_info * vi = ov_info(vorbisFile, -1);

	sampleRate_	= vi->rate;
	channels_		= vi->channels;
}

	bool
VorbisInput::_read(BufferItem * i)
{
	if (!currentTrack())
		return false;

	int max = BufferItem::maxLength();

	int len = ov_read(vorbisFile, i->buffer(), max, 0, 2, 1, &vorbisSection);

	long t	= long(ov_time_tell(vorbisFile));
	long br	= ov_bitrate_instant(vorbisFile);

	i->setLength(len);
	i->setTime(t);
	i->setTrackLength(trackLength_);
	i->setTrackID(currentTrack());
	i->setBitRate(br);
	i->setSampleRate(sampleRate_);
	i->setChannels(channels_);

	return len > 0;
}

// vim:ts=2:sw=2:tw=78:noet
