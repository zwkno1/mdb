TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    database.cpp

HEADERS += \
    comm.h \
    sharedmemory.h \
    database.h \
    exampledatabase.h \
    snapshot.h

LIBS += -pthread -lrt -lboost_filesystem
