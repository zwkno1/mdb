TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    ConcurrentDatabase.cpp \
    Database.cpp \
    main.cpp \
    PosixSharedMemory.cpp

HEADERS += \
    CommSrc.h \
    ConcurrentDatabase.h \
    Database.h \
    DataService.h \
    Resource.h \
    PosixSharedMemory.h
