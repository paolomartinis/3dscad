QT += core gui widgets
CONFIG += c++17 console
CONFIG -= app_bundle
TARGET = scadcheck
TEMPLATE = app
RESOURCES += ../../hardware/hardware.qrc

PARENT = ../..
INCLUDEPATH += $$PARENT

SOURCES += \
    main.cpp \
    $$PARENT/openscadparser.cpp \
    $$PARENT/openscadgenerator.cpp \
    $$PARENT/scenedocument.cpp \
    $$PARENT/scenemesh.cpp \
    $$PARENT/scenetree.cpp \
    $$PARENT/scenetreetoolmetadata.cpp \
    $$PARENT/hardwarelibrary.cpp \
    $$PARENT/manifoldcsg.cpp

win32:contains(QT_ARCH, x86_64) {
    MANIFOLD_BUILD_DIR = $$PWD/$$PARENT/build/manifold-build-64
} else:win32 {
    MANIFOLD_BUILD_DIR = $$PWD/$$PARENT/build/manifold-build-32
} else {
    MANIFOLD_BUILD_DIR = $$PWD/$$PARENT/build/manifold-build
}

!exists($$MANIFOLD_BUILD_DIR/src/libmanifold.a): error("scadcheck needs Manifold: run scripts/build-manifold.ps1 first")
DEFINES += HAVE_MANIFOLD_CSG
INCLUDEPATH += $$PWD/$$PARENT/build/manifold-src/include
LIBS += $$MANIFOLD_BUILD_DIR/src/libmanifold.a
