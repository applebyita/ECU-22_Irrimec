#-------------------------------------------------
#
# Project created by QtCreator 2017-06-16T10:40:57
#
#-------------------------------------------------

QT       += core gui network quickwidgets qml quick webview widgets quickcontrols2
android{
QT      += androidextras
}

ios {
    QMAKE_INFO_PLIST = ios/Info.plist

    QMAKE_TARGET_BUNDLE_PREFIX = it.appleby
    QMAKE_BUNDLE = ECU22Irrimec

    QMAKE_ASSET_CATALOGS = $$PWD/ios/Images.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"

    OBJECTIVE_SOURCES += \
        keepon.mm
}

TARGET = ECU-22_Irrimec
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    widget_stato.cpp \
    mapwidget.cpp \
    backbuttonhandler.cpp \
    widgetecu.cpp

HEADERS += \
    ecu.h \
        mainwindow.h \
    widget_stato.h \
    mapwidget.h \
    backbuttonhandler.h \
    keepon.h \
    widgetecu.h

FORMS += \
        mainwindow.ui \
    widget_stato.ui \
    mapwidget.ui \
    widgetecu.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    android/AndroidManifest.xml \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    resources.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

defineReplace(droidVersionCode) {
        segments = $$split(1, ".")
        for (segment, segments): vCode = "$$first(vCode)$$format_number($$segment, width=3 zeropad)"

        contains(ANDROID_TARGET_ARCH, arm64-v8a): \
            suffix = 1
        else:contains(ANDROID_TARGET_ARCH, armeabi-v7a): \
            suffix = 0
        # add more cases as needed

        return($$first(vCode)$$first(suffix))
}

VERSION = 1.1.0
ANDROID_VERSION_NAME = $$VERSION
ANDROID_VERSION_CODE = $$droidVersionCode($$ANDROID_VERSION_NAME)
