QT       += core gui
QT += datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lmfwrapper.cpp \
    main.cpp \
    mainwindow.cpp \
    pages/calibrationpage.cpp \
    pages/datameasurementpage.cpp

HEADERS += \
    lmfwrapper.h \
    mainwindow.h \
    pages/calibrationpage.h \
    pages/datameasurementpage.h

FORMS += \
    mainwindow.ui \
    pages/calibrationpage.ui \
    pages/datameasurementpage.ui

INCLUDEPATH += $$PWD\include
LIBS += -L$$PWD\include -lProject2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
