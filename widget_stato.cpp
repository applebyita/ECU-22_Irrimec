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

#include "widget_stato.h"
#include "ui_widget_stato.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QTimer>

WidgetStato::WidgetStato(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetStato)
{
    ui->setupUi(this);

//    ui->label_RPM->installEventFilter(this);


}

WidgetStato::~WidgetStato()
{
    delete ui;
}

void WidgetStato::resizeEvent(QResizeEvent *event){

    QWidget::resizeEvent(event);

    QTimer::singleShot(50,this,SLOT(ResizeStato()));

}

void WidgetStato::ResizeStato()
{
    // Dimensioni font e icone
    int HeightL = ui->label_RPM->height()*0.65;
    QFont FontLabel = QFont("DigifaceWide");
    FontLabel.setPixelSize(HeightL);
    FontLabel.setBold(false);
    ui->label_RPM->setFont(FontLabel);
    ui->label_Press->setFont(FontLabel);
    ui->label_Temp->setFont(FontLabel);
    ui->label_Batt->setFont(FontLabel);

    int HeightT = ui->lineEdit_Timer->height()*0.7;
    QFont FontTimer = qApp->font();
    FontTimer.setPixelSize(HeightT);
    FontTimer.setBold(true);
    ui->lineEdit_Timer->setFont(FontTimer);

    QSize IconSizes;
    IconSizes.setHeight(ui->pushButton_RPM->height()*0.8);
    IconSizes.setWidth(ui->pushButton_RPM->width()*0.8);
    ui->pushButton_RPM->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_Press->height()*0.8);
    IconSizes.setWidth(ui->pushButton_Press->width()*0.8);
    ui->pushButton_Press->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_Temp->height()*0.8);
    IconSizes.setWidth(ui->pushButton_Temp->width()*0.8);
    ui->pushButton_Temp->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_Batt->height()*0.8);
    IconSizes.setWidth(ui->pushButton_Batt->width()*0.8);
    ui->pushButton_Batt->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_Gasolio->height()*0.8);
    IconSizes.setWidth(ui->pushButton_Gasolio->width()*0.8);
    ui->pushButton_Gasolio->setIconSize(IconSizes);

    IconSizes.setHeight(ui->pushButton_STOP->height()*0.9);
    IconSizes.setWidth(ui->pushButton_STOP->width()*0.9);
    ui->pushButton_STOP->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_AUTO->height()*0.9);
    IconSizes.setWidth(ui->pushButton_AUTO->width()*0.9);
    ui->pushButton_AUTO->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_ESCL->height()*0.9);
    IconSizes.setWidth(ui->pushButton_ESCL->width()*0.9);
    ui->pushButton_ESCL->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_PIU->height()*0.9);
    IconSizes.setWidth(ui->pushButton_PIU->width()*0.9);
    ui->pushButton_PIU->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_MENO->height()*0.9);
    IconSizes.setWidth(ui->pushButton_MENO->width()*0.9);
    ui->pushButton_MENO->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_TIMER->height()*0.9);
    IconSizes.setWidth(ui->pushButton_TIMER->width()*0.9);
    ui->pushButton_TIMER->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_Dec->height()*0.9);
    IconSizes.setWidth(ui->pushButton_Dec->width()*0.9);
    ui->pushButton_Dec->setIconSize(IconSizes);
    IconSizes.setHeight(ui->pushButton_Inc->height()*0.9);
    IconSizes.setWidth(ui->pushButton_Inc->width()*0.9);
    ui->pushButton_Inc->setIconSize(IconSizes);

}

void WidgetStato::RefreshStato()
{
    // Pagina senza dati
    if(!DatiAgg){
        ui->stackedWidget_Stato->setCurrentIndex(ui->stackedWidget_Stato->indexOf(ui->page_Motore));
//        ui->label_Vel->setFocus();
        ui->label_RPM->setText("");
        ui->label_Press->setText("");
        ui->label_Temp->setText("");
        ui->label_Batt->setText("");
        ui->progressBar_Gasolio->setValue(0);
        ui->lineEdit_Timer->setText("");

        ui->pushButton_RPM->setEnabled(false);
        ui->pushButton_Press->setEnabled(false);
        ui->pushButton_Temp->setEnabled(false);
        ui->pushButton_Batt->setEnabled(false);
        ui->pushButton_Gasolio->setEnabled(false);

        ui->pushButton_STOP->setEnabled(false);
        ui->pushButton_AUTO->setEnabled(false);
        ui->pushButton_ESCL->setEnabled(false);
        ui->pushButton_PIU->setEnabled(false);
        ui->pushButton_MENO->setEnabled(false);
        ui->pushButton_TIMER->setEnabled(false);

    }
    // Pagina con dati
    else {
        if(StatoM==0){
            ui->label_RPM->setText(tr("%1").arg(RPM));
            ui->label_Press->setText(tr("%1").arg(Pressione, 0, 'f', 1));
            ui->label_Temp->setText(tr("%1").arg(TempH2O));
            ui->label_Batt->setText(tr("%1").arg(Vbat, 0, 'f', 1));
            ui->progressBar_Gasolio->setValue(Gasolio);
            ui->lineEdit_Timer->setText(tr("%1:%2").arg(TimerOre, 2, 10, QChar('0')).arg(TimerMin, 2, 10, QChar('0')));

            ui->pushButton_RPM->setEnabled(true);
            ui->pushButton_Press->setEnabled(true);
            ui->pushButton_Temp->setEnabled(true);
            ui->pushButton_Batt->setEnabled(true);
            ui->pushButton_Gasolio->setEnabled(true);

            ui->pushButton_STOP->setEnabled(true);
            ui->pushButton_AUTO->setEnabled(true);
            ui->pushButton_ESCL->setEnabled(true);
            if(StatoEscl==0){
                ui->pushButton_PIU->setEnabled(false);
                ui->pushButton_MENO->setEnabled(false);
            }
            else {
                ui->pushButton_PIU->setEnabled(true);
                ui->pushButton_MENO->setEnabled(true);
            }
            ui->pushButton_TIMER->setEnabled(true);

            if(StatoStop==0) {
                QPixmap ImgP=QPixmap(tr(":/Resources/stop.png"));
                ui->pushButton_STOP->setIcon(ImgP);
             }
            else {
                QPixmap ImgP=QPixmap(tr(":/Resources/stop_on.png"));
                ui->pushButton_STOP->setIcon(ImgP);
            }
            if(StatoAuto==0) {
                QPixmap ImgP=QPixmap(tr(":/Resources/auto.png"));
                ui->pushButton_AUTO->setIcon(ImgP);
             }
            else {
                QPixmap ImgP=QPixmap(tr(":/Resources/auto_on.png"));
                ui->pushButton_AUTO->setIcon(ImgP);
            }
            if(StatoEscl==0) {
                QPixmap ImgP=QPixmap(tr(":/Resources/esclusione.png"));
                ui->pushButton_ESCL->setIcon(ImgP);
             }
            else {
                QPixmap ImgP=QPixmap(tr(":/Resources/esclusione_on.png"));
                ui->pushButton_ESCL->setIcon(ImgP);
            }
            if(StatoTimer==0) {
                QPixmap ImgP=QPixmap(tr(":/Resources/timer.png"));
                ui->pushButton_TIMER->setIcon(ImgP);
             }
            else {
                QPixmap ImgP=QPixmap(tr(":/Resources/timer_on.png"));
                ui->pushButton_TIMER->setIcon(ImgP);
            }
        }
        else {
            ui->stackedWidget_Stato->setCurrentIndex(ui->stackedWidget_Stato->indexOf(ui->page_Allarme));
            QSize IconSizes;
            IconSizes.setHeight(ui->pushButton_Allarme->height()*0.9);
            IconSizes.setWidth(ui->pushButton_Allarme->width()*0.9);
            ui->pushButton_Allarme->setIconSize(IconSizes);

            if(StatoM<20){
                switch(StatoM){
                case ALLARME_RADIATORE:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme radiatore!"));
                    break;
                case ALLARME_PRESSIONE:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme pressione!"));
                    break;
                case ALLARME_ACQUA_MOTORE:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme acqua motore!"));
                    break;
                case ALLARME_ALTERNATORE:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme alternatore!"));
                    break;
                case ALLARME_OLIO:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme olio!"));
                    break;
                case ALLARME_RISERVA:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme riserva gasolio!"));
                    break;
                case FINE_LAVORO:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Warning.png"));
                    ui->label_Allarme->setText(tr("Fine lavoro"));
                    break;
                case ALLARME_FILTRO_GASOLIO:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme filtro gasolio!"));
                    break;
                case ALLARME_GIRI_MOTORE:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme giri motore!"));
                    break;
                case GUASTO_SENSORE_PRESSIONE:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                    ui->label_Allarme->setText(tr("Allarme sensore pressione!"));
                    break;
                case RISPARMIO_ENERGIA:
                    ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Warning.png"));
                    ui->label_Allarme->setText(tr("Risparmio energetico"));
                    break;
                 default:
                    break;
                }
            }
            // Allarme CAN
            else {
                ui->pushButton_Allarme->setIcon(QIcon(":/Resources/Allarme.png"));
                ui->label_Allarme->setText(tr("SPN: %1 - FMI: %2").arg(StatoM/100).arg(StatoM%100));
            }
        }

    }

}

bool WidgetStato::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
//        if (object == ui->lineEdit_AngI)
//        {
//            AngI_sel=true;
//            ui->lineEdit_AngI->setStyleSheet(Stile_Selez);
//            AngF_sel=false;
//            if(AngF_changed) ui->lineEdit_AngF->setStyleSheet(Stile_Mod);
//            else ui->lineEdit_AngF->setStyleSheet(Stile_Normale);
//            Vel_sel=false;
//            if(Vel_changed) ui->lineEdit_Vel->setStyleSheet(Stile_Mod);
//            else ui->lineEdit_Vel->setStyleSheet(Stile_Normale);
//        }
//        else if (object == ui->lineEdit_AngF)
//        {
//            AngI_sel=false;
//            if(AngI_changed) ui->lineEdit_AngI->setStyleSheet(Stile_Mod);
//            else ui->lineEdit_AngI->setStyleSheet(Stile_Normale);
//            AngF_sel=true;
//            ui->lineEdit_AngF->setStyleSheet(Stile_Selez);
//            Vel_sel=false;
//            if(Vel_changed) ui->lineEdit_Vel->setStyleSheet(Stile_Mod);
//            else ui->lineEdit_Vel->setStyleSheet(Stile_Normale);
//        }
//        else if (object == ui->lineEdit_Vel)
//        {
//            AngI_sel=false;
//            if(AngI_changed) ui->lineEdit_AngI->setStyleSheet(Stile_Mod);
//            else ui->lineEdit_AngI->setStyleSheet(Stile_Normale);
//            AngF_sel=false;
//            if(AngF_changed) ui->lineEdit_AngF->setStyleSheet(Stile_Mod);
//            else ui->lineEdit_AngF->setStyleSheet(Stile_Normale);
//            Vel_sel=true;
//            ui->lineEdit_Vel->setStyleSheet(Stile_Selez);
//        }
//    }
//    else  if (event->type() == QEvent::FocusOut)
//    {

    }

    return false;
}


void WidgetStato::on_pushButton_Inc_clicked()
{
    qint16 Tempo = ((quint16)TimerOre)*60 + TimerMin;

    if(Tempo<(60*60)) Tempo += 10;
    TimerMin = Tempo%60;
    TimerOre = Tempo/60;

    ui->lineEdit_Timer->setText(tr("%1:%2").arg(TimerOre, 2, 10, QChar('0')).arg(TimerMin, 2, 10, QChar('0')));

}

void WidgetStato::on_pushButton_Dec_clicked()
{
    qint16 Tempo = ((quint16)TimerOre)*60 + TimerMin;

    if(Tempo>0) Tempo -= 10;
    if(Tempo<0) Tempo = 0;
    TimerMin = Tempo%60;
    TimerOre = Tempo/60;

    ui->lineEdit_Timer->setText(tr("%1:%2").arg(TimerOre, 2, 10, QChar('0')).arg(TimerMin, 2, 10, QChar('0')));
}

void WidgetStato::on_pushButton_STOP_clicked()
{
    emit comando_stop();
}

void WidgetStato::on_pushButton_AUTO_clicked()
{
    emit comando_auto();
}

void WidgetStato::on_pushButton_ESCL_clicked()
{
    emit comando_escl();
}

void WidgetStato::on_pushButton_PIU_clicked()
{
    emit comando_piu();
}

void WidgetStato::on_pushButton_MENO_clicked()
{
    emit comando_meno();
}

void WidgetStato::on_pushButton_TIMER_clicked()
{
    emit comando_timer();
}


void WidgetStato::on_pushButton_PIU_pressed()
{
    emit comando_piu_pressed();
}

void WidgetStato::on_pushButton_PIU_released()
{
    emit comando_piu_released();
}

void WidgetStato::on_pushButton_MENO_pressed()
{
    emit comando_meno_pressed();
}

void WidgetStato::on_pushButton_MENO_released()
{
    emit comando_meno_released();
}
