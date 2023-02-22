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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTcpSocket>
#include <QTextEdit>
#include <QDir>
#include <QListWidget>
#include <QTimer>
#include <QQmlContext>
#include "ecu.h"
#include "mapwidget.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

typedef enum{
    NESSUN_COMANDO=0,
    DOWNLOAD_STATO_MACCHINA=2,
    DOWNLOAD_ELENCO_MACCHINE=11,
    DOWNLOAD_DETTAGLI_MACCHINE=12,
    UPLOAD_COMANDO_MACCHINA=13,
    DOWNLOAD_COMANDO_MACCHINA=15,
}Comandi_Server_t;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() override;

signals:
    void default_page_prog();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

public slots:
    void stateChanged(Qt::ApplicationState newState);

private slots:
    void on_listWidget_ECU_itemClicked(QListWidgetItem *item);

    void on_pushButton_Connect_clicked();
    void on_pushButton_Back_clicked();
    void on_pushButton_Logo_clicked();
    void on_pushButton_User_clicked();
    void on_pushButton_Login_clicked();
    void on_pushButton_Elenco_clicked();
    void on_pushButton_Stato_clicked();
    void on_pushButton_Startec_clicked();
    void on_pushButton_Mappa_clicked();
    void on_pushButton_Wifi_clicked();

    void IncomingData();
    void TCPConnesso();
    void TCPDisconnesso();

    void ComandoStop();
    void ComandoAuto();
    void ComandoEscl();
    void ComandoPiu();
    void ComandoMeno();
    void ComandoTimer();

    void ComandoPiuPressed();
    void ComandoPiuReleased();
    void ComandoMenoPressed();
    void ComandoMenoReleased();

    void CaricaMappa();

    void on_stackedWidget_App_currentChanged(int arg1);

    void RichiediElencoMacchine(void);
    void InvioHTTP(Comandi_Server_t  ComandoAttuale,int IndiceMacchina);
    void startRequest(const QUrl &requestedUrl);
    void httpReadyRead();
    void httpFinished();
    void HTTPTimeout();

    void StatoMacchinaWifi();
    void WifiTimeout();


private:
    Ui::MainWindow *ui;

    QTimer *timerInitTCP = new QTimer(this);
    QTimer *timerDatoTCP = new QTimer(this);
    QTimer *timerKeepAlive = new QTimer(this);
    QTimer *timerHTML = new QTimer(this);
    QTimer *timerInitWifi = new QTimer(this);

    QList<ECU_t*> ListaECU;

    QUrl                  url;
    QNetworkAccessManager qnam;
    QNetworkReply*        reply;
    quint8                StepHTTP=0;

    unsigned char ContatoreResize=0;
    int IndiceECU=-1;
    unsigned char TCPconnected=OFF_LINE;
    bool MQTTprotocol=true;
    QByteArray DataRX;
    quint8 Pulsanti[6]={0,0,0,0,0,0};
    quint8 SaltoRit=0;

    QString GPS_Nord, GPS_Est;

    // Variabili uso ini
    QDir            ProjectPath;
    QString         UserName, Password;

    // Client TCP
    QTcpSocket tcpClient;

    QString ComandoTCP, RispostaTCP;

    QByteArray BufferTX_MQTT, BufferRX_MQTT;

    void Publish_MQTT();

    void Refresh_Lista_ECU();

    void ShowTerminalMessage(QTextEdit* Edit, QString Message);
    void ShowTerminalErrorMessage(QTextEdit* Edit, QString Message);
    void ShowTerminalAssertMessage(QTextEdit* Edit, QString Message);

    MapWidget*  Mappa=nullptr;

#ifdef Q_OS_ANDROID
    void keep_screen_on(bool on);
#endif

};

#endif // MAINWINDOW_H
