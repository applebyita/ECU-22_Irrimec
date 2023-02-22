/*
Copyright (C) 2022 Paolo Gaboardi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3.0 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include <QApplication>
#include <QtWebView/QtWebView>
#include "backbuttonhandler.h"
#include "keepon.h"

#if defined (Q_OS_ANDROID)
#include <QtAndroid>

bool requestAndroidPermissions(){
    //Request requiered permissions at runtime

    const QVector<QString> permissions({"android.permission.WRITE_EXTERNAL_STORAGE",
                                        "android.permission.READ_EXTERNAL_STORAGE"});

    for(const QString &permission : permissions){
        auto result = QtAndroid::checkPermission(permission);
        if(result == QtAndroid::PermissionResult::Denied){
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
                return false;
        }
    }

    return true;
}
#endif

int main(int argc, char *argv[])
{

#ifndef Q_OS_IOS
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
#endif

#ifdef Q_OS_IOS
    // Keep screen ON
    KeepON yc;
    yc.setTimerDisabled();
#endif

#if defined (Q_OS_ANDROID)
    if(!requestAndroidPermissions())
        return -1;
#endif

    QApplication a(argc, argv);
    QtWebView::initialize();

    BackButtonHandler BackButtonObj;
    MainWindow w;

    QObject::connect(&BackButtonObj,SIGNAL(backButtonPressed()),&w,SLOT(close()));

    w.showMaximized();
 //   w.show();

    return a.exec();
}
