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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QQmlContext>
#include <QDir>

namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QString Latitude, QString Longitude, QWidget *parent = 0);
    ~MapWidget();

//    Q_PROPERTY(QString fileHTML MEMBER FileHTML NOTIFY FileHTMLChanged)
    Q_PROPERTY(QString percorsoMap MEMBER PercorsoMap NOTIFY FileHTMLChanged)

signals:
    void FileHTMLChanged();
    void ReloadMap();
    void EraseMap();
    void TurnOffMap(void);
    void TurnOnMap(void);

private:
    Ui::MapWidget *ui;

    QQmlContext* ContextMappa;

    QString FileHTML, PercorsoMap;
    QString FieldDirectory;
    QFile FileKML;

    QString FileHTMLModel= "\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<style>\n"
            "html, body, #gmapcanvas {\n"
            "    width: 100%;\n"
            "    height: 100%;\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "}\n"
            "#gmapcanvas {\n"
            "    position: relative;\n"
            "}\n"
            "</style>\n"
            "<script src='https://maps.googleapis.com/maps/api/js?key=API_KEY'></script>\n"
            "<script type='text/javascript'>\n"
            "function initMap(){\n"
            "       var myOptions = {\n"
#ifdef Q_OS_IOS
            "       zoom:18,\n"
#else
            "       zoom:17,\n"
#endif
            "       center:new google.maps.LatLng(44.9016223,9.851193599999988),\n"
            "       mapTypeId: google.maps.MapTypeId.SATELLITE,\n"
            "       streetViewControl: false\n"
            "    };\n"
            "    map = new google.maps.Map(document.getElementById(\'gmapcanvas\'), myOptions);\n"
            "    marker = new google.maps.Marker({map: map,position: new google.maps.LatLng(44.9016223,9.851193599999988)});\n"
            "}\n"
            "</script>\n"
            "</head>\n"
            "<body onload=\"initMap();\">\n"
            "<div id=\"gmapcanvas\"></div>\n"
            "</body>\n"
            "</html>";

    QString FileHTMLBlank="\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head></head>\n"
            "<body></body>\n"
            "</html>";

};

#endif // MAPWIDGET_H
