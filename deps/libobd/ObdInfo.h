/**************************************************************************
*   Copyright (C) 2010 by Michael Carpenter (malcom2073)                  *
*   malcom2073@gmail.com                                                  *
*                                                                         *
*   This file is a part of libobd                                         *
*                                                                         *
*   libobd is free software: you can redistribute it and/or modify        *
*   it under the terms of the GNU Lesser General Public License as        *
*   published by the Free Software Foundation, either version 2 of        *
*   the License, or (at your option) any later version.                   *
*                                                                         *
*   libobd is distributed in the hope that it will be useful,             *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with libobd.  If not, see <http://www.gnu.org/licenses/>.       *
***************************************************************************/

#ifndef OBDINFO_H
#define OBDINFO_H
#include <QString>
#include <QList>


//! SAE1979 information
/*! This class contains definitions for SAE1979 modes, pids, and other misc information. It is localized in english  */
class ObdInfo
{
public:
	ObdInfo();
	class Pid
	{
	public:
		Pid(unsigned char amode,unsigned char apid,QString aname,QString adescription,QString aunit,QString afunction,QString astringrep,float amin, float amax,bool abitencoded);
		Pid();
		QString stringRep;
		QString name;
		QString description;
		QString unit;
		QString function;
		float min;
		float max;
		unsigned char mode;
		unsigned char pid;
		int labels;
		int step;
		bool bitencoded;
	};

	struct ModeSixInfo
	{
		QString description;
		int id;
	};
	struct ModeSixScalers
	{
		QString description;
		QString units;
		int min;
		int max;
		double multiplier;
		int usaid;
		int offset;
	};

	QList<Pid*> pidList;
	QList<ModeSixScalers> modeSixScalerList;
	QList<ModeSixInfo> modeSixInfoList;
	QList<ModeSixInfo> modeSixTestList;
	Pid *getPidFromString(QString str);
	Pid *getPidFromBytes(int mode,int pid);
	ModeSixInfo getInfoFromByte(int obdmid);
	ModeSixInfo getTestFromByte(int tid);
	ModeSixScalers getScalerFromByte(int UASID);
	static int intFromHex(QString num);
};
#endif //OBDINFO_H
