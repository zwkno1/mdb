TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    Database.cpp \
    main.cpp \
    PosixSharedMemory.cpp

HEADERS += \
    CommSrc.h \
    Database.h \
    DataService.h \
    Resource.h \
    PosixSharedMemory.h \
    DatabaseReader.h
