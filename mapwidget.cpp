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

#include "mapwidget.h"
#include "ui_mapwidget.h"
#include <QObject>
#include <QQuickItem>
#include <QDir>
#include <QStandardPaths>
#include <QtMath>

MapWidget::MapWidget(QString Latitude, QString Longitude, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);

    //MAP QML
    ContextMappa=ui->quickWidget_Mappa->rootContext();
    ContextMappa->setContextProperty("mainWidget", this);
    QQuickItem* QML_Mappa_Item = qobject_cast<QQuickItem *>(ui->quickWidget_Mappa->rootObject());
    // connect our C++ signal to our QML slot
    // NOTE: if we want to pass an parameter to our QML slot, it has to be
    // a QVariant.
    QObject::connect(this, SIGNAL(ReloadMap()), QML_Mappa_Item, SLOT(reloadMap()));
    QObject::connect(this, SIGNAL(EraseMap()), QML_Mappa_Item, SLOT(eraseMap()));
    QObject::connect(this, SIGNAL(TurnOffMap()), QML_Mappa_Item, SLOT(turnOff()));
    QObject::connect(this, SIGNAL(TurnOnMap()), QML_Mappa_Item, SLOT(turnOn()));

//    //Inserisci le coordinate (se presenti) nel file html altrimenti pagina html vuota
//    FileHTML.clear();
//    if(Latitude!=""){
//        FileHTML=FileHTMLModel;
//        QString Coordinate=tr("(%1,%2)").arg(Latitude).arg(Longitude);
//        FileHTML.replace("(44.9016223,9.851193599999988)",Coordinate);
//    }
//    else {
//        FileHTML=FileHTMLBlank;
//        qDebug()<<"PAGINA VUOTA!";
//    }


//    // PROBLEMA WEBVIEW --> Apertura HTML da file
//    QDir ProjectPath=QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
//    QFile FileMap;
//    FileMap.setFileName(ProjectPath.absolutePath()+QString("/Map.html"));
//    FileMap.open(QFile::WriteOnly|QFile::Truncate);
//    FileMap.write(FileHTML.toUtf8());
//    FileMap.close();

//    PercorsoMap = QString("file:///"+QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/Map.html");

    //*******************************************************************************************************
    //******* Chiamata al file Map.php online ***************************************************************
    //*******************************************************************************************************

    PercorsoMap = QString("http://nomeserver.com/Map.php");
    if(Latitude!=""){
        PercorsoMap = PercorsoMap + QString("?Lat=%1").arg(Latitude) + QString("&Lon=%1").arg(Longitude);
        PercorsoMap = PercorsoMap + QString("&Lat1=0") + QString("&Lon1=0");
        PercorsoMap = PercorsoMap + QString("&Auto=0");
    }

    ContextMappa->setContextProperty("mainWidget", this);
    emit FileHTMLChanged();
    QApplication::processEvents();
    emit ReloadMap();

}

MapWidget::~MapWidget()
{
    delete ui;
}
