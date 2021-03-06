#-------------------------------------------------
#
# Project created by QtCreator 2016-12-07T01:36:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

QMAKE_POST_LINK += "bash /Users/Venky/Work/Softwares/DeepCount/DeepCount/post_compile.sh"

TARGET = DeepCount
TEMPLATE = app

ICON = icon.icns

SOURCES += main.cpp\
        deepcount.cpp \
    fileio/read_mat.cpp \
    helpers/filtering.cpp \
    plot/qcustomplot.cpp \
    plot/zcustomplot.cpp \
    fileio/write.cpp

HEADERS  += deepcount.h \
    helpers/includes.h \
    fileio/read_mat.h \
    fileio/MATio \
    helpers/structs.h \
    helpers/filtering.h \
    plot/qcustomplot.h \
    plot/zcustomplot.h \
    fileio/write.h

FORMS    += deepcount.ui

RESOURCES += qdarkstyle/style.qrc

macx: LIBS += -L$$PWD/../matio/lib/ -lmatio

INCLUDEPATH += $$PWD/../matio/include
DEPENDPATH += $$PWD/../matio/include

macx: PRE_TARGETDEPS += $$PWD/../matio/lib/libmatio.a


macx: LIBS += -L$$PWD/../../../../../../usr/local/Cellar/armadillo/7.500.2/lib/ -larmadillo.7.50.2

INCLUDEPATH += $$PWD/../../../../../../usr/local/Cellar/armadillo/7.500.2/include
DEPENDPATH += $$PWD/../../../../../../usr/local/Cellar/armadillo/7.500.2/include

#macx: INCLUDEPATH += /usr/local/Cellar/eigen/3.3.0/include/eigen3

macx: LIBS += -L$$PWD/../../../../../../usr/local/Cellar/fann/2.2.0/lib/ -lfann.2.2.0

INCLUDEPATH += $$PWD/../../../../../../usr/local/Cellar/fann/2.2.0/include
DEPENDPATH += $$PWD/../../../../../../usr/local/Cellar/fann/2.2.0/include

DISTFILES += \
    Icon.icns
