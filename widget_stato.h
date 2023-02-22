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

#ifndef WIDGET_STATO_H
#define WIDGET_STATO_H

#include <QWidget>
#include "ecu.h"

namespace Ui {
class WidgetStato;
}

class WidgetStato : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetStato(QWidget *parent = 0);
    ~WidgetStato();

    //Valori Stato
    qint16     RPM=0;
    float      Pressione=0;
    qint8      Gasolio=0;
    quint8     TempH2O=0;
    float      Vbat=0;
    quint8     StatoStop=0, StatoAuto=0, StatoEscl=0, StatoTimer;
    quint32    StatoM=0;
    quint8     TimerOre=0, TimerMin=0;
    bool       DatiAgg=false;

signals:
    void comando_stop();
    void comando_auto();
    void comando_escl();
    void comando_piu();
    void comando_meno();
    void comando_timer();

    void comando_piu_pressed();
    void comando_piu_released();
    void comando_meno_pressed();
    void comando_meno_released();

public slots:
    void ResizeStato();
    void RefreshStato();

private:
    Ui::WidgetStato *ui;



protected:
    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButton_Inc_clicked();
    void on_pushButton_Dec_clicked();
    void on_pushButton_STOP_clicked();
    void on_pushButton_AUTO_clicked();
    void on_pushButton_ESCL_clicked();
    void on_pushButton_PIU_clicked();
    void on_pushButton_MENO_clicked();
    void on_pushButton_TIMER_clicked();

    void on_pushButton_PIU_pressed();
    void on_pushButton_PIU_released();
    void on_pushButton_MENO_pressed();
    void on_pushButton_MENO_released();

};

#endif // Widget_Stato_H
