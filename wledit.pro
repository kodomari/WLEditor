QT += core quick qml

CONFIG += c++17

TARGET = wledit
TEMPLATE = app

SOURCES += \
    src/core/TextEngine.cpp \
    src/mobile/main.cpp

HEADERS += \
    src/core/TextEngine.h

RESOURCES += \
    mobile.qrc

# Android specific settings
android {
    CONFIG += qml_debug
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    ANDROID_MIN_SDK_VERSION = 26
    ANDROID_TARGET_SDK_VERSION = 33
    ANDROID_VERSION_NAME = 1.3.0
    ANDROID_VERSION_CODE = 13
}

# Desktop specific settings
!android {
    SOURCES += \
        src/desktop/main.cpp \
        src/desktop/MainWindow.cpp
    
    HEADERS += \
        src/desktop/MainWindow.h
    
    QT += widgets
}