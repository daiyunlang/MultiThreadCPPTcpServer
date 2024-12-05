QT = core concurrent

CONFIG += c++11 cmdline

win32:{
    message("windows")
    CONFIG(debug, debug | release){
        message("debug")
        contains(DEFINES, WIN64){
            message("x64")
            DESTDIR=$$PWD/bin/win64/debug
        }
        else{
            message("x32")
            DESTDIR=$$PWD/bin/win32/debug
        }
    }
    else{
        message("release")
        contains(DEFINES, WIN64){
            message("x64")
            DESTDIR=$$PWD/bin/win64/release
        }
        else
        {
            message("x32")
            DESTDIR=$$PWD/bin/win32/release
        }
    }

}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        tcpserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    tcpserver.h
