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
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QScreen>
#include <QSize>
#include <QQuickItem>
#include "widgetecu.h"
#include "widget_stato.h"
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <qscroller.h>
#endif
#include <QFontDatabase>

#define SOFTWARE_VERSION "1.1"

/**
  revisioni
  1.0   15-12-21    Prima versione da progetto precedente
  1.1   08-04-22    Aggiunto allarme CAN con SPN e FMI


TODO:

 * Costanti QT per i vari sistemi operativi:
    * Q_OS_ANDROID
    * Q_OS_CYGWIN
    * Q_OS_FREEBSD
    * Q_OS_IOS
    * Q_OS_LINUX
    * Q_OS_MAC
    * Q_OS_OSX
    * Q_OS_QNX
    * Q_OS_UNIX
    * Q_OS_WIN32
    * Q_OS_WIN64
    * Q_OS_WIN
    * Q_OS_WINCE
    * Q_OS_WINPHONE
    * Q_OS_WINRT

********************************************************
 Main
*******************************************************/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->frame_Mappa->setVisible(false);

    QFontDatabase::addApplicationFont(":/Resources/digifaw.ttf");

    connect(qApp,  SIGNAL(applicationStateChanged(Qt::ApplicationState)),this, SLOT(stateChanged(Qt::ApplicationState)));

    connect(&tcpClient, SIGNAL(readyRead()),this, SLOT(IncomingData()));
    connect(&tcpClient, SIGNAL(connected()),this, SLOT(TCPConnesso()));
    connect(&tcpClient, SIGNAL(disconnected()),this, SLOT(TCPDisconnesso()));

    connect( timerInitTCP, SIGNAL(timeout()), this, SLOT( TCPDisconnesso() ));
    timerInitTCP->setInterval( 10000 );

    connect( timerDatoTCP, SIGNAL(timeout()), this, SLOT( TCPDisconnesso() ));
    timerDatoTCP->setInterval( 30000 );

//    connect( timerKeepAlive, SIGNAL(timeout()), this, SLOT( InvioKeepAlive() ));
//    timerKeepAlive->setInterval( 30000 );

    connect( timerHTML, SIGNAL(timeout()), this, SLOT( HTMLTimeout() ));
    timerHTML->setInterval( 15000 );

    connect( timerInitWifi, SIGNAL(timeout()), this, SLOT( WifiTimeout() ));
    timerInitWifi->setInterval( 5000 );

    // Segnali widget stato
    connect(ui->W_Stato, SIGNAL(comando_stop()),this, SLOT(ComandoStop()));
    connect(ui->W_Stato, SIGNAL(comando_auto()),this, SLOT(ComandoAuto()));
    connect(ui->W_Stato, SIGNAL(comando_escl()),this, SLOT(ComandoEscl()));
    connect(ui->W_Stato, SIGNAL(comando_piu()),this, SLOT(ComandoPiu()));
    connect(ui->W_Stato, SIGNAL(comando_meno()),this, SLOT(ComandoMeno()));
    connect(ui->W_Stato, SIGNAL(comando_timer()),this, SLOT(ComandoTimer()));

    connect(ui->W_Stato, SIGNAL(comando_piu_pressed()),this, SLOT(ComandoPiuPressed()));
    connect(ui->W_Stato, SIGNAL(comando_piu_released()),this, SLOT(ComandoPiuReleased()));
    connect(ui->W_Stato, SIGNAL(comando_meno_pressed()),this, SLOT(ComandoMenoPressed()));
    connect(ui->W_Stato, SIGNAL(comando_meno_released()),this, SLOT(ComandoMenoReleased()));

    // Caricamento widget mappa (pagina vuota)
    Mappa= new MapWidget("", "");
    //set layout programatically
    ui->page_Mappa->layout()->addWidget(Mappa);

    ContatoreResize=0;

    int width= ui->lineEdit_Username->width();
    ui->lineEdit_Username->setGeometry(0,0,width,static_cast<int>(qApp->fontMetrics().height()*1.2));
    ui->lineEdit_Password->setGeometry(0,0,width,static_cast<int>(qApp->fontMetrics().height()*1.2));
    width= ui->pushButton_Login->width();
    ui->pushButton_Login->setGeometry(0,0,width,qApp->fontMetrics().height()*3);


    ui->label_Titolo->setText(tr("rev\n")+tr(SOFTWARE_VERSION));
    ui->pushButton_User->setFocus();

    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Elenco));

#ifdef Q_OS_ANDROID
    keep_screen_on(true);
#endif

#ifdef Q_OS_ANDROID
    //definizione scroller
    QScroller* scroller=QScroller::scroller(ui->listWidget_ECU);
    scroller->grabGesture(ui->listWidget_ECU,QScroller::LeftMouseButtonGesture);
    QScrollerProperties ActualProperties;
    ActualProperties=scroller->scrollerProperties();
    QVariant VerticalOvershoot=QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
    ActualProperties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, VerticalOvershoot);
    scroller->setScrollerProperties(ActualProperties);
#endif

    //---------- FILE INI contenente i dati utente -------------------------------------------------------------------------
    //Setta la cartella di lavoro
    QString   WorkingDirectory;
    WorkingDirectory=QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));

    ProjectPath.setPath(WorkingDirectory);
    if (!ProjectPath.exists()) {
        ProjectPath.mkpath(WorkingDirectory);
    }

    QFile FileIni(ProjectPath.absolutePath()+QString("/Settings.ini"));

    if (FileIni.exists()){
       QSettings settings (ProjectPath.absolutePath()+QString("/Settings.ini"), QSettings::IniFormat);
       settings.beginGroup("User_Settings");
       UserName=settings.value("UserName").toString();
       Password=settings.value("Password").toString();
       settings.endGroup();
       RichiediElencoMacchine();
    }
    else {
        //Se il file non esiste svuota i campi e attendi l'utente
        UserName="";
        Password="";
        ui->lineEdit_Username->setText(UserName);
        ui->lineEdit_Password->setText(Password);
        emit ui->pushButton_User->click();
    }
    //-----------------------------------------------------------------------------------------------------------------------

    // DA SVILUPPARE!!
    ui->pushButton_Startec->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event){

    QMainWindow::resizeEvent(event);

#ifdef Q_OS_ANDROID
    if (ContatoreResize==1){

    //ridimensiona tasto logo
        QSize IconSizes;
        IconSizes.setHeight(ui->pushButton_Logo->height()*0.9);
        IconSizes.setWidth(ui->pushButton_Logo->width()*0.9);
        ui->pushButton_Logo->setIconSize(IconSizes);

        IconSizes.setHeight(ui->pushButton_ECU->height()*0.9);
        IconSizes.setWidth(ui->pushButton_ECU->width()*0.9);
        ui->pushButton_ECU->setIconSize(IconSizes);

    //ridimensiona tasto user
        int Height;
        Height=ui->pushButton_User->height();
        ui->pushButton_User->setFixedWidth(Height);
        IconSizes.setHeight(ui->pushButton_User->height()*0.9);
        IconSizes.setWidth(ui->pushButton_User->height()*0.9);
        ui->pushButton_User->setIconSize(IconSizes);

        int Width;
        Width=ui->pushButton_Elenco->width();
        ui->pushButton_Elenco->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Elenco->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));
        ui->pushButton_Stato->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Stato->setIconSize(QSize((int)(0.9*Width),(int)(0.9*Width)));
        ui->pushButton_Startec->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Startec->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));
        ui->pushButton_Mappa->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Mappa->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));
        ui->pushButton_Wifi->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Wifi->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));

//        Refresh_Lista_ECU();
    }
#endif

#ifdef Q_OS_IOS
    if (ContatoreResize==2){

    //ridimensiona tasto logo
        QSize IconSizes;
        IconSizes.setHeight((int)(ui->pushButton_Logo->height()*0.9));
        IconSizes.setWidth((int)(ui->pushButton_Logo->width()*0.9));
        ui->pushButton_Logo->setIconSize(IconSizes);

    //ridimensiona tasto user
        int Height;
        Height=ui->pushButton_User->height();
        ui->pushButton_User->setFixedWidth(Height);
        IconSizes.setHeight(ui->pushButton_User->height()*0.9);
        IconSizes.setWidth(ui->pushButton_User->height()*0.9);
        ui->pushButton_User->setIconSize(IconSizes);

        int Width;
        Width=ui->pushButton_Elenco->width();
        ui->pushButton_Elenco->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Elenco->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));
        ui->pushButton_Stato->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Stato->setIconSize(QSize((int)(0.9*Width),(int)(0.9*Width)));
        ui->pushButton_Startec->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Startec->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));
        ui->pushButton_Mappa->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Mappa->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));
        ui->pushButton_Wifi->setFixedHeight((int)(1.0*Width));
        ui->pushButton_Wifi->setIconSize(QSize((int)(0.8*Width),(int)(0.8*Width)));

//        Refresh_Lista_ECU();
    }
#endif

if (ContatoreResize<10) ContatoreResize++;
}

void MainWindow::on_pushButton_User_clicked()
{
    ui->label_errore_credenziali->setText(tr(""));
    if (ui->pushButton_User->isChecked()){
        ui->lineEdit_Username->setText(UserName);
        ui->lineEdit_Password->setText(Password);
        ui->stackedWidget_Core->setCurrentIndex(ui->stackedWidget_Core->indexOf(ui->page_Login));
    }else{
        if (ListaECU.count()){
            ui->stackedWidget_Core->setCurrentIndex(ui->stackedWidget_Core->indexOf(ui->page_Core));
        }else{
            ui->pushButton_User->setChecked(true);
        }
    }
}

void MainWindow::on_pushButton_Login_clicked()
{
    UserName=ui->lineEdit_Username->text();
    Password=ui->lineEdit_Password->text();
    QSettings settings (ProjectPath.absolutePath()+QString("/Settings.ini"), QSettings::IniFormat);
    settings.beginGroup("User_Settings");
    settings.setValue("UserName",UserName);
    settings.setValue("Password",Password);
    settings.endGroup();
    StepHTTP=0;
    RichiediElencoMacchine();
}

//-------------------------------------------------------------------------------------------------------------------------------
//--------------------- HTTP ----------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------

void MainWindow::RichiediElencoMacchine(void)
{
    if(StepHTTP==0){
        ListaECU.clear();
        InvioHTTP(DOWNLOAD_ELENCO_MACCHINE, 0);
    }
    else if(StepHTTP>0){
        InvioHTTP(DOWNLOAD_DETTAGLI_MACCHINE, StepHTTP-1);
    }

}

void MainWindow::InvioHTTP(Comandi_Server_t  ComandoAttuale,int IndiceMacchina)
{
    QString StringaComando;
    QString MD5String;

    //Componi la stringa a seconda del comando
    switch (ComandoAttuale){
    case DOWNLOAD_ELENCO_MACCHINE:
        MD5String = QString(QCryptographicHash::hash(Password.toUtf8(),QCryptographicHash::Md5).toHex());
        StringaComando=tr("http://www.nomeserver.com/11.php?01=%1&02=%2").arg(UserName).arg(MD5String);
        break;

    case DOWNLOAD_DETTAGLI_MACCHINE:
        MD5String = QString(QCryptographicHash::hash(Password.toUtf8(),QCryptographicHash::Md5).toHex());
        StringaComando=tr("http://www.nomeserver.com/12.php?01=%1&02=%2&03=%3").arg(UserName).arg(MD5String).arg(ListaECU.at(IndiceMacchina)->NumeroSerialeSW);
        break;

    default:
        break;
    }

    QUrl url=QUrl::fromUserInput(StringaComando);
    startRequest(url);
}

void MainWindow::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;

    if (!url.isValid()) {
        ShowTerminalErrorMessage(ui->textEdit,tr("Invalid URL: %1: %2").arg(url.toString(), url.errorString()));
        return;
    }

    ShowTerminalMessage(ui->textEdit,tr("Request URL: %1").arg(url.toString()));

    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

    timerHTML->start();
}

void MainWindow::httpFinished()
{
    timerHTML->stop();

    if (reply->error()) {
        ShowTerminalErrorMessage(ui->textEdit,tr("URL ERROR: %1").arg(reply->errorString()));
        reply->deleteLater();
        reply = Q_NULLPTR;
        ui->stackedWidget_Core->setCurrentIndex(ui->stackedWidget_Core->indexOf(ui->page_Core));
        emit HTTPTimeout();
        return;
    }

    reply->deleteLater();
    reply = Q_NULLPTR;

    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Elenco));

    if(StepHTTP==0){
        if (ListaECU.count()){
            ui->stackedWidget_Core->setCurrentIndex(ui->stackedWidget_Core->indexOf(ui->page_Core));
            ui->pushButton_User->setChecked(false);
            StepHTTP=1;
            RichiediElencoMacchine();
        }
        else {
        //Elenco non corretto
            if (!(ui->pushButton_User->isChecked()))  emit ui->pushButton_User->click();
            ui->label_errore_credenziali->setText(tr("Credenziali errate!"));
        }
    }
    else {
        Refresh_Lista_ECU();
        if(StepHTTP<ListaECU.count()){
            StepHTTP++;
            RichiediElencoMacchine();
        }
    }

}

void MainWindow::httpReadyRead()
{
    QString replyString=QString::fromStdString(reply->readAll().toStdString());

    ShowTerminalMessage(ui->textEdit,tr("answer: %1").arg(replyString));

    if(StepHTTP==0){
        if((replyString.left(2))==QString("GO")){
            QString Go=replyString.section('|', 0, 0, QString::SectionSkipEmpty);
            quint8 NumeroMacchine=(Go.remove(0,2).toInt());
            QString NumeriSerie=replyString.section('|', 1, 1, QString::SectionSkipEmpty);
            for(quint8 i=0; i<NumeroMacchine; i++){
                QString NumeroSerie=NumeriSerie.section('*', i, i, QString::SectionSkipEmpty);
                //Creo una lista ECU di prova
                ECU_t* NewECU=new ECU_t;
                NewECU->Nome=tr("");
                NewECU->NumeroSerialeSW=NumeroSerie;
                NewECU->Descrizione=tr("");
                NewECU->Status=OFF_LINE;
                NewECU->GiriMotore=0;
                NewECU->Pressione=0;
                NewECU->Depressione=0;
                NewECU->TempH2O=0;
                NewECU->VBat=0;
                NewECU->PressRec=0;
                NewECU->PressRecImp=0;
                NewECU->Gasolio=0;
                NewECU->StatoMotore=ALLARME_NO;
                ListaECU.append(NewECU);
            }
        }
    }
    else {
        if((replyString.left(2))==QString("GO")){
            ECU_t* ECU=ListaECU.at(StepHTTP-1);
            //Tutto ok procedi
            if (replyString.mid(2,10)==ECU->NumeroSerialeSW){
                //Macchina giusta, procedi
                ECU->NumeroSerialeHW=replyString.section('|', 1, 1, QString::SectionSkipEmpty);
                ECU->NumeroSIM=replyString.section('|', 2, 2, QString::SectionSkipEmpty);
                ECU->Nome=replyString.section('|', 3, 3, QString::SectionSkipEmpty);
                ECU->Descrizione=replyString.section('|', 4, 4, QString::SectionSkipEmpty);
                if (!ECU->Descrizione.contains("GMP")){
                    ECU->Descrizione=replyString.section('|', 4, 4, QString::SectionSkipEmpty)+tr(" ")+replyString.section('|', 5, 5, QString::SectionSkipEmpty)+tr("-")+replyString.section('|', 6, 6, QString::SectionSkipEmpty);
                }
            }
        }
    }

}

void MainWindow::HTTPTimeout()
{
    QSize IconSizes;
    IconSizes.setHeight(200);
    IconSizes.setWidth(200);
    ui->pushButton_Error->setIconSize(IconSizes);
    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Error));

    ui->pushButton_Wifi->setEnabled(true);
}

//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------

void MainWindow::Refresh_Lista_ECU(void){

    // Svuota il listview
    ui->listWidget_ECU->clear();

    foreach(ECU_t* Elemento, ListaECU){
        QListWidgetItem* item;
        item = new QListWidgetItem(ui->listWidget_ECU);
        WidgetECU* NewWidgetECU=new WidgetECU(this,Elemento);

        QSize Size;
        Size.setHeight(NewWidgetECU->height);
        Size.setWidth(ui->listWidget_ECU->width()-4);
        item->setSizeHint(Size);

        ui->listWidget_ECU->setItemWidget(item, NewWidgetECU);
        ui->listWidget_ECU->addItem(item);
    }

}

void MainWindow::on_listWidget_ECU_itemClicked(QListWidgetItem *item)
{
    // Se ho cliccato una macchina diversa dalla precedente
    if(ui->listWidget_ECU->currentIndex().row()!=IndiceECU){
        IndiceECU=ui->listWidget_ECU->currentIndex().row();
        for(int i=0; i<ListaECU.count(); i++){
            ListaECU.at(i)->Status=OFF_LINE;
        }
        // Se non c'è nessun collegamento attivo
        if(TCPconnected==OFF_LINE){
            emit on_pushButton_Connect_clicked();
        }
        // Se c'è un collegamento attivo va prima chiuso
        else {
            tcpClient.abort();
            emit on_pushButton_Connect_clicked();
        }
    }
    // Se ho cliccato lo stesso getto commuto lo stato
    else {
        // Se il getto non era collegato
        if(TCPconnected==OFF_LINE){
            emit on_pushButton_Connect_clicked();
        }
        // Se il getto era collegato chiudo la connessione
        else {
            tcpClient.abort();
            ListaECU.at(IndiceECU)->Status=OFF_LINE;
            Refresh_Lista_ECU();
        }
    }


}


void MainWindow::on_pushButton_Connect_clicked()
{    
    if(MQTTprotocol){
        // Server MQTT
        ui->lineEdit_IP->setText("servermqtt.com");
        ui->lineEdit_Port->setText("1883");
    }
    else {
        // WIFI
        ui->lineEdit_IP->setText("192.168.4.1");
        ui->lineEdit_Port->setText("333");
    }
    ShowTerminalMessage(ui->textEdit,QString("Connecting to %1:%2 Server").arg(ui->lineEdit_IP->text()).arg(ui->lineEdit_Port->text().toInt()));
    tcpClient.connectToHost(ui->lineEdit_IP->text(), ui->lineEdit_Port->text().toInt());

    if(MQTTprotocol){
        ListaECU.at(IndiceECU)->Status=CONNECTING;
        Refresh_Lista_ECU();
        timerInitTCP->start();
    }
    else {
        timerInitWifi->start();
    }

}

void MainWindow::on_pushButton_Back_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// Finestra di debug
void MainWindow::on_pushButton_Logo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_Elenco_clicked()
{
    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Elenco));
}

void MainWindow::on_pushButton_Stato_clicked()
{
//    timerKeepAlive->stop();
    timerDatoTCP->start();
    ui->W_Stato->DatiAgg=false;      // pulizia campi precedenti
    ui->W_Stato->RefreshStato();
    ComandoTCP = "$ST";
    if(MQTTprotocol){
        Publish_MQTT();
    }
    else {
        tcpClient.write(ComandoTCP.toLatin1());
    }
    ShowTerminalMessage(ui->textEdit, ComandoTCP);
    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Stato));
    ui->pushButton_Stato->setEnabled(false);
    ui->pushButton_Startec->setEnabled(false);
    ui->pushButton_Mappa->setEnabled(false);
    ui->W_Stato->RefreshStato();

}

void MainWindow::on_pushButton_Startec_clicked()
{

}

void MainWindow::on_pushButton_Mappa_clicked()
{
    // ignorato se sono già sulla mappa
    if(ui->stackedWidget_App->currentIndex()==ui->stackedWidget_App->indexOf(ui->page_Mappa)) return;

//    timerKeepAlive->stop();
    timerDatoTCP->start();
    ComandoTCP = "$GP";
    Publish_MQTT();
    ShowTerminalMessage(ui->textEdit, ComandoTCP);
    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Mappa));
    ui->pushButton_Stato->setEnabled(false);
    ui->pushButton_Startec->setEnabled(false);
    ui->pushButton_Mappa->setEnabled(false);

}

void MainWindow::on_pushButton_Wifi_clicked()
{
    MQTTprotocol = false;
    emit on_pushButton_Connect_clicked();
    ui->W_Stato->DatiAgg=false;      // pulizia campi precedenti
    ui->W_Stato->RefreshStato();
    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Stato));

}

void MainWindow::ComandoStop()
{
    if(MQTTprotocol){
        if(ui->W_Stato->StatoStop==0){
    //        timerKeepAlive->stop();
            timerDatoTCP->start();
            ui->W_Stato->DatiAgg=0;      // pulizia campi precedenti
            ui->W_Stato->RefreshStato();
            ComandoTCP = "$CS";
            Publish_MQTT();
            ShowTerminalMessage(ui->textEdit, ComandoTCP);
        }
    }
    // WIFI
    else {
        if(ui->W_Stato->StatoStop==0) Pulsanti[4]=1;
    }
}

void MainWindow::ComandoAuto()
{
    if(MQTTprotocol){
    //    timerKeepAlive->stop();
        timerDatoTCP->start();
        ui->W_Stato->DatiAgg=0;      // pulizia campi precedenti
        ui->W_Stato->RefreshStato();
        if(ui->W_Stato->StatoAuto==0) ComandoTCP = "$CA,1";
        else ComandoTCP = "$CA,0";
        Publish_MQTT();
        ShowTerminalMessage(ui->textEdit, ComandoTCP);
    }
    // WIFI
    else {
        if(ui->W_Stato->StatoAuto==0) Pulsanti[3]=1; else Pulsanti[3]=0;
    }
}

void MainWindow::ComandoEscl()
{
    if(MQTTprotocol){
    //    timerKeepAlive->stop();
        timerDatoTCP->start();
        ui->W_Stato->DatiAgg=0;      // pulizia campi precedenti
        ui->W_Stato->RefreshStato();
        if(ui->W_Stato->StatoEscl==0) ComandoTCP = "$CE,1";
        else ComandoTCP = "$CE,0";
        Publish_MQTT();
        ShowTerminalMessage(ui->textEdit, ComandoTCP);
    }
    // WIFI
    else {
        if(ui->W_Stato->StatoEscl==0) Pulsanti[2]=1; else Pulsanti[2]=0;
    }
}

void MainWindow::ComandoPiu()
{
    if(MQTTprotocol){
    //    timerKeepAlive->stop();
        timerDatoTCP->start();
        ui->W_Stato->DatiAgg=0;      // pulizia campi precedenti
        ui->W_Stato->RefreshStato();
        ComandoTCP = "$UP";
        Publish_MQTT();
        ShowTerminalMessage(ui->textEdit, ComandoTCP);
    }
}

void MainWindow::ComandoMeno()
{
    if(MQTTprotocol){
    //    timerKeepAlive->stop();
        timerDatoTCP->start();
        ui->W_Stato->DatiAgg=0;      // pulizia campi precedenti
        ui->W_Stato->RefreshStato();
        ComandoTCP = "$DW";
        Publish_MQTT();
        ShowTerminalMessage(ui->textEdit, ComandoTCP);
    }
}

void MainWindow::ComandoTimer()
{
    if(MQTTprotocol){
    //    timerKeepAlive->stop();
        timerDatoTCP->start();
        ui->W_Stato->DatiAgg=0;      // pulizia campi precedenti
        ui->W_Stato->RefreshStato();
        if(ui->W_Stato->StatoTimer==0) ComandoTCP = "$CT,1," + QString("%1").arg(ui->W_Stato->TimerOre, 2, 10, QChar('0')) + "," + QString("%1").arg(ui->W_Stato->TimerMin, 2, 10, QChar('0'));
        else ComandoTCP = "$CT,0,00,00";
        Publish_MQTT();
        ShowTerminalMessage(ui->textEdit, ComandoTCP);
    }
    // WIFI
    else {
        if(ui->W_Stato->StatoTimer==0) Pulsanti[5]=1;
        else {
            Pulsanti[5]=0;
            ui->W_Stato->TimerOre=0;
            ui->W_Stato->TimerMin=0;
        }
    }
}


void MainWindow::Publish_MQTT()
{
    quint16 RL=0, i=0;

    RL = 20 + ComandoTCP.length();		// Remaining Lenght = 2 TPLEN + 18 TOPIC ("ECU18/" + numero seriale + "rx") + Payload

    BufferTX_MQTT[0]  = 0x30;		// PUBLISH
    if(RL<128){
        BufferTX_MQTT[1] = RL;			// Remaining Lenght
        BufferTX_MQTT[2] = 0x00;		// TPLEN MSB
        BufferTX_MQTT[3] = 0x12;		// TPLEN LSB
        BufferTX_MQTT[4] = 0x45;		// 'E'
        BufferTX_MQTT[5] = 0x43;		// 'C'
        BufferTX_MQTT[6] = 0x55;		// 'U'
        BufferTX_MQTT[7] = 0x31;		// '1'
        BufferTX_MQTT[8] = 0x38;		// '8'
        BufferTX_MQTT[9] = 0x2F;		// '/'
        BufferTX_MQTT.insert(10,ListaECU.at(IndiceECU)->NumeroSerialeSW);
        BufferTX_MQTT[20] = 0x72;		// 'r'
        BufferTX_MQTT[21] = 0x78;		// 'x'
        BufferTX_MQTT.insert(22,ComandoTCP);		// payload
        tcpClient.write(BufferTX_MQTT,RL+2);
    }
    else {
        BufferTX_MQTT[1] = (RL%128)+128;		// Remaining Lenght LSB
        BufferTX_MQTT[2] = (RL/128);				// Remaining Lenght MSB
        BufferTX_MQTT[3] = 0x00;		// TPLEN MSB
        BufferTX_MQTT[4] = 0x12;		// TPLEN LSB
        BufferTX_MQTT[5] = 0x45;		// 'E'
        BufferTX_MQTT[6] = 0x43;		// 'C'
        BufferTX_MQTT[7] = 0x55;		// 'U'
        BufferTX_MQTT[8] = 0x31;		// '1'
        BufferTX_MQTT[9] = 0x38;		// '8'
        BufferTX_MQTT[10] = 0x2F;		// '/'
        BufferTX_MQTT.insert(11,ListaECU.at(IndiceECU)->NumeroSerialeSW);
        BufferTX_MQTT[21] = 0x72;		// 'r'
        BufferTX_MQTT[22] = 0x78;		// 'x'
        BufferTX_MQTT.insert(23,ComandoTCP);		// payload
        tcpClient.write(BufferTX_MQTT,RL+3);
    }

}

void MainWindow::IncomingData(void)
{        
    timerDatoTCP->stop();

    if(MQTTprotocol){
        BufferRX_MQTT.clear();
        BufferRX_MQTT = tcpClient.readAll();

        // CONNACK corretta
        if(BufferRX_MQTT.toHex()=="20020000"){
            ShowTerminalMessage(ui->textEdit, "CONNACK OK!");

            // SUBSCRIBE PACKET
            BufferTX_MQTT.clear();
            BufferTX_MQTT[0]  = 0x82;		// SUBSCRIBE
            BufferTX_MQTT[1]  = 0x17;		// Remaining Lenght (lunghezza fissa) = 23
                                            // 2 PKTID + 2 TPLEN + 18 TOPIC ("ECU18/" + numero seriale + "tx") + 1 QS
            BufferTX_MQTT[2]  = 0x00;		// PKTID MSB
            BufferTX_MQTT[3]  = 0x01;		// PKTID LSB
            BufferTX_MQTT[4]  = 0x00;		// TPLEN MSB
            BufferTX_MQTT[5]  = 0x12;		// TPLEN LSB
            BufferTX_MQTT[6]  = 0x45;		// 'E'
            BufferTX_MQTT[7]  = 0x43;		// 'C'
            BufferTX_MQTT[8]  = 0x55;		// 'U'
            BufferTX_MQTT[9]  = 0x31;		// '1'
            BufferTX_MQTT[10] = 0x38;		// '8'
            BufferTX_MQTT[11] = 0x2F;		// '/'
            BufferTX_MQTT.insert(12,ListaECU.at(IndiceECU)->NumeroSerialeSW);
            BufferTX_MQTT[22] = 0x74;		// 't'
            BufferTX_MQTT[23] = 0x78;		// 'x'
            BufferTX_MQTT[24] = 0x00;		// QS
            tcpClient.write(BufferTX_MQTT,25);
            timerDatoTCP->start();
            ShowTerminalMessage(ui->textEdit, "MQTT SUBSCRIBE PACKET ...");

            return;
        }
        // SUBSCRIBE ACK
        else if(BufferRX_MQTT.toHex()=="9003000100"){
            ShowTerminalMessage(ui->textEdit, "SUBSCRIBE OK!");

            timerDatoTCP->start();
            ComandoTCP = "$CK," + ListaECU.at(IndiceECU)->NumeroSerialeSW;
            Publish_MQTT();

            return;
        }
        // PUBLISH PACKET received
        else if(BufferRX_MQTT.toHex().left(2)=="30"){
            quint16 RL=0, TPLEN=0;
            if(static_cast<quint8>(BufferRX_MQTT[1])<128) {
                RL = static_cast<quint8>(BufferRX_MQTT[1]);
                TPLEN = ((static_cast<quint16>(BufferRX_MQTT[2]))<<8) + static_cast<quint8>(BufferRX_MQTT[3]);
                RispostaTCP = BufferRX_MQTT.right(RL-TPLEN-2);
            }
            else {
                RL = (BufferRX_MQTT[1]&0x7F) + ((static_cast<quint16>(BufferRX_MQTT[2]))*128);
                TPLEN = ((static_cast<quint16>(BufferRX_MQTT[3]))<<8) + static_cast<quint8>(BufferRX_MQTT[4]);
                RispostaTCP = BufferRX_MQTT.right(RL-TPLEN-2);
            }
        }

        ShowTerminalAssertMessage(ui->textEdit, RispostaTCP);

        QString Intestazione=RispostaTCP.section(',', 0, 0, QString::SectionSkipEmpty);

        if(Intestazione=="$SM"){
            ui->W_Stato->RPM = RispostaTCP.section(',', 1, 1, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->Pressione = RispostaTCP.section(',', 2, 2, QString::SectionSkipEmpty).toFloat();
            ui->W_Stato->Gasolio = RispostaTCP.section(',', 3, 3, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->TempH2O = RispostaTCP.section(',', 4, 4, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->Vbat = RispostaTCP.section(',', 5, 5, QString::SectionSkipEmpty).toFloat();
            ui->W_Stato->StatoM = RispostaTCP.section(',', 6, 6, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->StatoStop = RispostaTCP.section(',', 7, 7, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->StatoAuto = RispostaTCP.section(',', 8, 8, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->StatoEscl = RispostaTCP.section(',', 9, 9, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->StatoTimer = RispostaTCP.section(',', 10, 10, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->TimerOre = RispostaTCP.section(',', 11, 11, QString::SectionSkipEmpty).toInt();
            ui->W_Stato->TimerMin = RispostaTCP.section(',', 12, 12, QString::SectionSkipEmpty).toInt();

    //        ui->W_Stato->SelTM = RispostaTCP.section(',', 7, 7, QString::SectionSkipEmpty).toInt();
    //        ui->W_Stato->TempoRes = RispostaTCP.section(',', 8, 8, QString::SectionSkipEmpty).toInt();
    //        ui->W_Stato->MetriRes = RispostaTCP.section(',', 9, 9, QString::SectionSkipEmpty).toInt();
    //        ui->W_Stato->VelRot = RispostaTCP.section(',', 10, 10, QString::SectionSkipEmpty).toInt();

            ui->W_Stato->DatiAgg=true;
            ui->W_Stato->RefreshStato();
        }
        else if(Intestazione=="$NE"){
            GPS_Nord = RispostaTCP.section(',', 1, 1, QString::SectionSkipEmpty);
            GPS_Est = RispostaTCP.section(',', 2, 2, QString::SectionSkipEmpty);
            CaricaMappa();
        }
        else if(Intestazione=="$READY"){
            ListaECU.at(IndiceECU)->Status=ON_LINE;
            Refresh_Lista_ECU();
        }
        else if(Intestazione=="$ERROR"){
            TCPDisconnesso();
        }

        ui->pushButton_Stato->setEnabled(true);
    //    ui->pushButton_Startec->setEnabled(true);
        if(MQTTprotocol) ui->pushButton_Mappa->setEnabled(true);

    //    timerKeepAlive->start();
    }
    // WIFI
    else {
        DataRX=tcpClient.readAll();

        if((DataRX[0]=='$')&&(DataRX[1]=='S')&&(DataRX[2]=='T')){
            qint16 i=0;
            unsigned char CheckSum=0;
            for(i=0; i<15; i++) CheckSum=CheckSum^DataRX[i];
            if(CheckSum==((unsigned char)DataRX[15])){
                ui->W_Stato->RPM = ((quint16)DataRX[3])*256+DataRX[4];
                ui->W_Stato->Pressione = (((float)((unsigned char)DataRX[5])))/10;
                ui->W_Stato->Gasolio = DataRX[6];
                ui->W_Stato->TempH2O = DataRX[7];
                ui->W_Stato->Vbat = (((float)((unsigned char)DataRX[8])))/5;
                ui->W_Stato->StatoEscl = DataRX[9];
                ui->W_Stato->StatoAuto = DataRX[10];
                ui->W_Stato->StatoStop = DataRX[11];
                ui->W_Stato->StatoTimer = DataRX[12];
                if(ui->W_Stato->StatoTimer){
                    ui->W_Stato->TimerOre = DataRX[13];
                    ui->W_Stato->TimerMin = DataRX[14];
                }

                if((ui->W_Stato->StatoAuto==0)&&(Pulsanti[3]!=0)){
                    if(SaltoRit==0) SaltoRit++; else {SaltoRit=0; Pulsanti[3]=0;}
                }
                else SaltoRit=0;

                ui->W_Stato->RefreshStato();
            }
        }
        else if((DataRX[0]=='E')&&(DataRX[1]=='E')) ShowTerminalErrorMessage(ui->textEdit, "ERROR");

        QTimer::singleShot(500,this,SLOT(StatoMacchinaWifi()));

    }
}

//----------- MAPPA --------------------------------------------------------------------------
void MainWindow::on_stackedWidget_App_currentChanged(int arg1)
{
//    if (arg1!=3){
        //distruggi il widget mappa
        if (Mappa!=nullptr){
            delete Mappa;
            Mappa=nullptr;
        }
//    }
//    else {
//        //crea e visualizza il widget
////        Mappa= new MapWidget(GPS_Nord, GPS_Est, GPS_Nord_1, GPS_Est_1);
//        Mappa= new WidgetMappa();
//        //set layout programatically
//        ui->page_Mappa->layout()->addWidget(Mappa);
//    }
}

void MainWindow::CaricaMappa()
{
    //crea e visualizza il widget
    Mappa= new MapWidget(GPS_Nord, GPS_Est, this);
    //set layout programatically
    ui->page_Mappa->layout()->addWidget(Mappa);

}

void MainWindow::TCPConnesso()
{
    timerInitTCP->stop();
//    timerKeepAlive->start();
    TCPconnected=ON_LINE;
    ShowTerminalAssertMessage(ui->textEdit,"Connected!");
//    ListaGetti.at(IndiceECU)->Status=ON_LINE;
//    Refresh_Lista_Getti();
//    ui->pushButton_Stato->setEnabled(true);
//    ui->pushButton_Startec->setEnabled(true);
//    ui->pushButton_Mappa->setEnabled(true);
//    ui->pushButton_Settings->setEnabled(false);

    if(MQTTprotocol){
        // CONNECT PACKET
        BufferTX_MQTT.clear();
        BufferTX_MQTT[0]  = 0x10;		// CONNECT
        BufferTX_MQTT[1]  = 0x2E;		// Remaining Lenght (lunghezza fissa) = 46
                                        // 2 PLEN + 4 "MQTT" + 1 LVL + 1 FL + 2 KA + 2 CIDLEN + 17 ID ("ecu"+numero seriale+"_app") + 2 ULEN + 7 USER + 2 PWLEN + 6 PASSWORD
        BufferTX_MQTT[2]  = 0x00;		// PLEN MSB
        BufferTX_MQTT[3]  = 0x04;		// PLEN LSB
        BufferTX_MQTT[4]  = 0x4D;		// 'M'
        BufferTX_MQTT[5]  = 0x51;		// 'Q'
        BufferTX_MQTT[6]  = 0x54;		// 'T'
        BufferTX_MQTT[7]  = 0x54;		// 'T'
        BufferTX_MQTT[8]  = 0x04;		// LVL
        BufferTX_MQTT[9]  = 0xC2;		// FL
        BufferTX_MQTT[10] = 0x00;		// KA MSB
        BufferTX_MQTT[11] = 0xB4;		// KA LSB (180 sec)
        BufferTX_MQTT[12] = 0x00;		// CIDLEN MSB
        BufferTX_MQTT[13] = 0x11;		// CIDLEN LSB
        BufferTX_MQTT.insert(14,"ecu" + ListaECU.at(IndiceECU)->NumeroSerialeSW + "_app");
        BufferTX_MQTT[31] = 0x00;		// ULEN MSB
        BufferTX_MQTT[32] = 0x07;		// ULEN LSB
        BufferTX_MQTT[33] = 0x00;
        BufferTX_MQTT[34] = 0x00;
        BufferTX_MQTT[35] = 0x00;
        BufferTX_MQTT[36] = 0x00;
        BufferTX_MQTT[37] = 0x00;
        BufferTX_MQTT[38] = 0x00;
        BufferTX_MQTT[39] = 0x00;
        BufferTX_MQTT[40] = 0x00;		// PWLEN MSB
        BufferTX_MQTT[41] = 0x06;		// PWLEN LSB
        BufferTX_MQTT[42] = 0x00;
        BufferTX_MQTT[43] = 0x00;
        BufferTX_MQTT[44] = 0x00;
        BufferTX_MQTT[45] = 0x00;
        BufferTX_MQTT[46] = 0x00;
        BufferTX_MQTT[47] = 0x00;
        tcpClient.write(BufferTX_MQTT,48);
        timerDatoTCP->start();
        ShowTerminalMessage(ui->textEdit, "MQTT CONNECT PACKET ...");
        qDebug() << "Pacchetto MQTT:" << BufferTX_MQTT;
    }
    else {
        timerInitWifi->stop();
        ui->W_Stato->DatiAgg=true;
        ui->W_Stato->RefreshStato();
        StatoMacchinaWifi();
    }
}

void MainWindow::TCPDisconnesso()
{
    tcpClient.abort();
    timerInitTCP->stop();
    timerKeepAlive->stop();
    TCPconnected=OFF_LINE;
    ShowTerminalErrorMessage(ui->textEdit,"Not Connected!");
    if(MQTTprotocol){
        if(ListaECU.at(IndiceECU)->Status!=OFF_LINE) ListaECU.at(IndiceECU)->Status=ERROR_TCP;
        Refresh_Lista_ECU();
        ui->pushButton_Stato->setEnabled(false);
        ui->pushButton_Startec->setEnabled(false);
        ui->pushButton_Mappa->setEnabled(false);
        ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Elenco));
        ui->pushButton_Elenco->setChecked(true);
    }
}

//void MainWindow::InvioKeepAlive()
//{
//    ComandoTCP = "&";
//    tcpClient.write(ComandoTCP.toLatin1());
//    ShowTerminalMessage(ui->textEdit, ComandoTCP);
//}

//-------------------------------------------------------------------------------------------------------------------------------
//--------------------- WIFI ----------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------

void MainWindow::StatoMacchinaWifi()
{
    qint16 i=0;
    unsigned char CheckSum=0;
    QByteArray DataTX;

    DataTX[0]='$'; DataTX[1]='C'; DataTX[2]='M';
    DataTX[3]=Pulsanti[0];  // +
    DataTX[4]=Pulsanti[1];  // -
    DataTX[5]=Pulsanti[2];  // Escl. Pompa
    DataTX[6]=Pulsanti[3];  // Auto
    DataTX[7]=Pulsanti[4];  // Spegnimento
    DataTX[8]=Pulsanti[5];  // Timer
    DataTX[9]=ui->W_Stato->TimerOre;
    DataTX[10]=ui->W_Stato->TimerMin;
    DataTX[11]=0;
    for(i=0; i<12; i++) CheckSum=CheckSum^DataTX[i];
    DataTX[12]=CheckSum;
    tcpClient.write(DataTX,13);
}

void MainWindow::WifiTimeout()
{
    tcpClient.abort();
    timerInitWifi->stop();

    QSize IconSizes;
    IconSizes.setHeight(200);
    IconSizes.setWidth(200);
    ui->pushButton_Error->setIconSize(IconSizes);
    ui->stackedWidget_App->setCurrentIndex(ui->stackedWidget_App->indexOf(ui->page_Error));
}

void MainWindow::ComandoPiuPressed()
{
    Pulsanti[0]=1;
}

void MainWindow::ComandoPiuReleased()
{
    Pulsanti[0]=0;
}

void MainWindow::ComandoMenoPressed()
{
    Pulsanti[1]=1;
}

void MainWindow::ComandoMenoReleased()
{
    Pulsanti[1]=0;
}

//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event){
    tcpClient.abort();
}

void MainWindow::ShowTerminalMessage(QTextEdit* Edit, QString Message){
    //Set Log Window color
    Edit->setStyleSheet("QTextEdit {background-color: black;}");
    Edit->setTextColor(Qt::white);
    //Output to log window
    Edit->append(Message);
}

void MainWindow::ShowTerminalErrorMessage(QTextEdit* Edit, QString Message){
    //Set Log Window color
    Edit->setStyleSheet("QTextEdit {background-color: black;}");
    Edit->setTextColor(Qt::red);
    //Output to log window
    Edit->append(Message);
}

void MainWindow::ShowTerminalAssertMessage(QTextEdit* Edit, QString Message){
    //Set Log Window color
    Edit->setStyleSheet("QTextEdit {background-color: black;}");
    Edit->setTextColor(Qt::green);
    //Output to log window
    Edit->append(Message);
}

void MainWindow::stateChanged(Qt::ApplicationState newState) {
    if ((newState==Qt::ApplicationSuspended)||(newState==Qt::ApplicationInactive)){
        qApp->exit();
    }
    QApplication::processEvents();
}

#ifdef Q_OS_ANDROID
void MainWindow::keep_screen_on(bool on) {
  QtAndroid::runOnAndroidThread([on]{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
      QAndroidJniObject window =
          activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

      if (window.isValid()) {
        const int FLAG_KEEP_SCREEN_ON = 128;
        if (on) {
          window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        } else {
          window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        }
      }
    }
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
  });
}
#endif

