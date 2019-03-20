TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    database.cpp

HEADERS += \
    CommSrc.h \
    sharedmemory.h \
    database.h \
    databasereader.h \
    exampledatabase.h \
    databaseconfig.h

LIBS += -pthread -lrt
