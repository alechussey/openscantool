/*
 *  OpenScanTool
 *  Copyright (C) 2012  Alec Hussey
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dashboard.h"

Dashboard::Dashboard(QWidget *parent) :
    QWidget(parent)
{
	obd = new ObdThread();
	connect(obd, SIGNAL(pidReply(QString,QString,int,double)), 
			this, SLOT(response(QString,QString,int,double)));
	obd->start();
	obd->setPort("/dev/pts/2");
	obd->setBaud(9600);
	obd->addRequest(0x01, 0x0c, 1, 0);
	obd->addRequest(0x01, 0x0d, 1, 0);
	obd->addRequest(0x01, 0x11, 1, 0);
	
	speedo = new Speedometer();
	tacho = new Tachometer();
	
	mainLayout = new QGridLayout();
	mainLayout->addWidget(speedo, 0, 0);
	mainLayout->addWidget(tacho, 0, 1);
	setLayout(mainLayout);
}

void Dashboard::response(QString pid, QString val, int set, double time)
{
	qDebug() << "Called Dashboard::response()";
	qDebug() << pid << val << set << time;
}
