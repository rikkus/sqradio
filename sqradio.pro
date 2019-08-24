TEMPLATE         = app
TARGET           = sqradio
CONFIG          += debug thread

OBJECTS_DIR      = .obj
MOC_DIR          = .moc

unix {

  target.path = /usr/bin
  INSTALLS += target

  contains (CONFIG, static) {

    UNAME = $$system(uname -s)

    contains( UNAME, [lL]inux )
    {
      message( Doing Linux static build )

      LIBS += \
        esd/libesd.a \
        vorbis/libvorbisfile.a \
        vorbis/libvorbis.a \
        ogg/libogg.a \
        ffmp3/libffmp3.a

        QMAKE_CXXFLAGS += -DSQRADIO_STATIC_BUILD

    } else {

      error( Non-Linux static build requested )
    }

  } else {

    LIBS += -lesd -lvorbisfile -lvorbis -logg -lavcodec

  }
}

exists ( /usr/local/include/alsa/asoundlib.h ) {

  exists ( /usr/local/lib/libasound.so ) {

    message( Building ALSA output driver )

    QMAKE_CXXFLAGS  += -I/usr/local/include
    QMAKE_CXXFLAGS  += -DSQRADIO_USE_DRIVER_ALSA
    LIBS            += -L/usr/local/lib
    LIBS            += -lasound
  }

} else {

  exists ( /usr/include/alsa/asoundlib.h ) {

    exists ( /usr/lib/libasound.so ) {

      message( Building ALSA output driver )

      QMAKE_CXXFLAGS  *= -DSQRADIO_USE_DRIVER_ALSA
      LIBS            *= -lasound
    }
  }
}

exists ( /usr/include/sys/soundcard.h ) {

  message( Building OSS output driver )

  QMAKE_CXXFLAGS  += -DSQRADIO_USE_DRIVER_OSS
}

exists ( /usr/local/include/esd.h ) {

  exists ( /usr/local/lib/libesd.so ) {

    message( Building esound output driver )

    QMAKE_CXXFLAGS  += -I/usr/local/include
    QMAKE_CXXFLAGS  += -DSQRADIO_USE_DRIVER_ESD
    LIBS            += -L/usr/local/lib
    LIBS            += -lesd
  }

} else {

  exists ( /usr/include/esd.h ) {

    exists ( /usr/lib/libesd.so ) {

      message( Building esound output driver )

      QMAKE_CXXFLAGS  += -DSQRADIO_USE_DRIVER_ESD
      LIBS            += -lesd
    }
  }
}

exists ( /usr/local/include/portaudio.h ) {

  exists ( /usr/local/lib/libportaudio.so ) {

    message( Building PortAudio output driver )

    QMAKE_CXXFLAGS  += -I/usr/local/include
    QMAKE_CXXFLAGS  += -DSQRADIO_USE_DRIVER_PORT_AUDIO
    LIBS            += -L/usr/local/lib
    LIBS            += -lportaudio
  }

} else {

  exists ( /usr/include/portaudio.h ) {

    exists ( /usr/lib/libportaudio.so ) {

      message( Building PortAudio output driver )

      QMAKE_CXXFLAGS  += -DSQRADIO_USE_DRIVER_PORT_AUDIO
      LIBS            += -lportaudio
    }
  }
}

HEADERS += AddDialog.h \
           Application.h \
           ByteArray.h \
           Client.h \
           DataBlock.h \
           Decoder.h \
           DecoderDriver.h \
           DecoderDriverFactory.h \
           DecoderDriverMP3.h \
           DecoderDriverVorbis.h \
           DecoderThread.h \
           FlowManager.h \
           FormatTester.h \
           FormatTesterMP3.h \
           FormatTesterVorbis.h \
           KillMeEvent.h \
           LevelMeter.h \
           MainWidget.h \
           MainWindow.h \
           MetaDataEvent.h \
           OpenDialog.h \
           Output.h \
           OutputDriver.h \
           OutputDriverFactory.h \
           OutputDriverALSA.h \
           OutputDriverESD.h \
           OutputDriverOSS.h \
           OutputDriverPortAudio.h \
           OutputDriverNULL.h \
           OutputThread.h \
           PCM.h \
           QThreadPipe.h \
           RingBuffer.h \
           SettingsDialog.h \
           StationInfo.h \
           StationListViewItem.h \
           StatusView.h \
           TestClient.h \
           TextView.h \
           Throbber.h \
           UDPListener.h \
           Utils.h

SOURCES += AddDialog.cpp \
           Application.cpp \
           ByteArray.cpp \
           Client.cpp \
           Decoder.cpp \
           DecoderDriver.cpp \
           DecoderDriverFactory.cpp \
           DecoderDriverMP3.cpp \
           DecoderDriverVorbis.cpp \
           DecoderThread.cpp \
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
           OutputDriver.cpp \
           OutputDriverFactory.cpp \
           OutputDriverALSA.cpp \
           OutputDriverESD.cpp \
           OutputDriverOSS.cpp \
           OutputDriverPortAudio.cpp \
           OutputDriverNULL.cpp \
           OutputThread.cpp \
           PCM.cpp \
           QThreadPipe.cpp \
           RingBuffer.cpp \
           SettingsDialog.cpp \
           StationInfo.cpp \
           StationListViewItem.cpp \
           StatusView.cpp \
           TestClient.cpp \
           TextView.cpp \
           Throbber.cpp \
           UDPListener.cpp \
           Utils.cpp
