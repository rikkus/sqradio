TEMPLATE         = app
TARGET           = esqradio

OBJECTS_DIR = .eobj
MOC_DIR = .emoc

LIBS += \
  esd/libesd.a \
  vorbis/libvorbisfile.a \
  vorbis/libvorbis.a \
  ogg/libogg.a \
  ffmp3/libffmp3.a

QMAKE_CXXFLAGS += -DSQRADIO_EMBEDDED

HEADERS += AddDialog.h \
           Application.h \
           ByteArray.h \
           Client.h \
           Decoder.h \
           FlowManager.h \
           FormatTester.h \
           FormatTesterMP3.h \
           FormatTesterVorbis.h \
           LevelMeter.h \
           MainWidget.h \
           MainWindow.h \
           OpenDialog.h \
           Output.h \
           PCM.h \
           StationInfo.h \
           StationListViewItem.h \
           TextView.h \
           Utils.h \
           rik/Mutex.h \
           rik/Semaphore.h \
           rik/Thread.h

SOURCES += AddDialog.cpp \
           Application.cpp \
           ByteArray.cpp \
           Client.cpp \
           Decoder.cpp \
           FlowManager.cpp \
           FormatTester.cpp \
           FormatTesterMP3.cpp \
           FormatTesterVorbis.cpp \
           LevelMeter.cpp \
           main.cpp \
           MainWidget.cpp \
           MainWindow.cpp \
           OpenDialog.cpp \
           Output.cpp \
           PCM.cpp \
           StationInfo.cpp \
           StationListViewItem.cpp \
           TextView.cpp \
           Utils.cpp \
           rik/Mutex.cpp \
           rik/Semaphore.cpp \
           rik/Thread.cpp
