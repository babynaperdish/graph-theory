#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += c++11

TARGET = GraphLab
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        randomgenerator.cpp \
        logdistribution.cpp \
        graph.cpp \
        graphanalyzer.cpp \
        weightedgraph.cpp \
        pathfinder.cpp \
    maxflow.cpp \
    skeleton.cpp

HEADERS += \
        mainwindow.h \
        randomgenerator.h \
        logdistribution.h \
        graph.h \
        graphanalyzer.h \
        weightedgraph.h \
        pathfinder.h \
    maxflow.h \
    skeleton.h

FORMS += \
        mainwindow.ui
