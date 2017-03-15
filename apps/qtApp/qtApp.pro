# apps/qtExample/qtExample.pro --
#
# Initial software
# Authors: Nicolas Roussel
# Copyright © Inria

TEMPLATE  = app
CONFIG   += qt warn_on link_prl

QT += opengl
LIBS += -L/home/vvkdhkl/Downloads/libpointing-0.9.6/pointing/libpointing.a
//LIBS += -L/home/vvkdhkl/Downloads/libpointing-0.9.6/build-QsLogSharedLibrary-Qt_4_8-Debug/QsLog
//LIBS += -lQsLog

#POINTING = ../..
#include($$POINTING/pointing/pointing.pri)

include(../PointingLogger/PointingLogger.pri)

HEADERS   += BallisticsPlayground.h \
    sessionmanager.h
SOURCES   += BallisticsPlayground.cpp ballistics.cpp \
    sessionmanager.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../pointing/release/ -lpointing
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../pointing/debug/ -lpointing
else:unix:!macx: LIBS += -L$$PWD/../../pointing/ -lpointing

INCLUDEPATH += $$PWD/../../pointing
DEPENDPATH += $$PWD/../../pointing

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../pointing/release/libpointing.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../pointing/debug/libpointing.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../pointing/release/pointing.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../pointing/debug/pointing.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../../pointing/libpointing.a
