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

#ifndef ECU_H
#define ECU_H
#include <QString>
#include <QDate>
#include <QTime>

typedef enum{
ON_LINE=0,
OFF_LINE=1,
CONNECTING=2,
CONNECTED=3,
ERROR_TCP=4
}ECU_Status;

//lista tipi di errori della macchina
typedef enum{
ALLARME_NO,
ALLARME_RADIATORE,
ALLARME_PRESSIONE,
ALLARME_ACQUA_MOTORE,
ALLARME_ALTERNATORE,
ALLARME_OLIO,
ALLARME_RISERVA,
FINE_LAVORO,
ALLARME_FILTRO_GASOLIO,
ALLARME_GIRI_MOTORE,
GUASTO_SENSORE_PRESSIONE,
RISPARMIO_ENERGIA
}ECU_Motor_status;

typedef struct{
//Valori Generali Centralina
QString                 NumeroSerialeSW;
QString                 NumeroSerialeHW;
QString                 NumeroSIM;
QString                 Nome;
QString                 Descrizione="";
ECU_Status              Status;
QDateTime               UltimaLettura;
//Valori Stato Centralina
quint16                 GiriMotore;
float                   Pressione;
float                   Depressione;
quint8                  PressRec;
quint8                  PressRecImp;
quint8                  Gasolio;
quint8                  TempH2O;
float                   VBat;
ECU_Motor_status        StatoMotore;
//Sezione gestione comandi remoti
QString                 ComandoPending;
QString                 StatusCommand;
//Sezione coordinate
double                  Latitudine;
double                  Longitudine;
//Sezione startec
quint8                  StatoStartec;
quint8                  EventoStartec;
quint16                 LunghTuboStartec;
quint16                 VelMisStartec;
int                     Vel_Imp_startec;
int                     Pausa_Start_startec;
int                     Pausa_End_startec;
}ECU_t;

#endif // ECU_H
