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

#include "ObdInfo.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <math.h>
ObdInfo::ObdInfo()
{
	/*QString filename = "";
	if (QFile::exists("obd2data.dat"))
	{
		filename = "obd2data.dat";
	}
	else if (QFile::exists("/usr/share/libobd/obd2data.dat"))
	{
		filename = "/usr/share/libobd/obd2data.dat";
	}
	else
	{
		//qDebug() << "Error! No obd2data.dat file found! Expect a segfault";
		//return;
	}*/
	/*
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open obd2data.dat file! Expect a segfault";
		return;
	}

	QXmlStreamReader reader(file.readAll());
	file.close();
	int mode;
	while (!reader.atEnd())
	{
		if (reader.name() == "mode" && reader.isStartElement())
		{
			mode = reader.attributes().value("value").toString().toInt();
			reader.readNext();
			while (reader.name() != "mode")
			{
				if (reader.name() == "pids" && reader.isStartElement())
				{
					reader.readNext();
					while (reader.name() != "pids")
					{
						if (reader.name() == "pid" && reader.isStartElement())
						{
							Pid *pid = new Pid();
							pid->name = reader.attributes().value("name").toString();
							pid->description = reader.attributes().value("desc").toString();
							pid->unit = reader.attributes().value("unit").toString();
							pid->mode = mode;
							pid->pid = intFromHex(reader.attributes().value("value").toString());
							pid->function = reader.attributes().value("function").toString();
							pid->min = reader.attributes().value("min").toString().toFloat();
							pid->max = reader.attributes().value("max").toString().toFloat();
							pid->labels = reader.attributes().value("labels").toString().toInt();
							pid->step = reader.attributes().value("step").toString().toInt();
							pid->stringRep = QString((mode < 16) ? "0" : "").append(QString::number(mode,16).toUpper()).append((pid->pid < 16) ? "0" : "").append(QString::number(pid->pid,16).toUpper());
							qDebug() << pid->stringRep;
							pidList.append(pid);
						}
						reader.readNext();
					}
				}
				reader.readNext();
			}
		}
		reader.readNext();
	}

	*/

	//Hardcoded OBD2 pids:
	/*Pid *pid0104 = new Pid();
	pid0104->name = "Load";
	pid0104->description = "Calculated Engine Load";
	pid0104->unit = "%";
	pid0104->min = 00;
	pid0104->max = 100;
	pid0104->pid = 0x04;
	pid0104->mode = 0x01;
	pid0104->function = "A*(100/255)";
	pid0104->stringRep = "0104";
	pidList.append(pid0104);

	Pid *pid0105 = new Pid();
	pid0105->name = "Temp";
	pid0105->description = "Coolant Temperature";
	pid0105->unit = "C";
	pid0105->min = -40;
	pid0105->max = 215;
	pid0105->pid = 0x05;
	pid0105->mode = 0x01;
	pid0105->function = "A-40";
	pid0105->stringRep = "0105";
	pidList.append(pid0105);
	//Pid("Temp","Coolant Temperature","C",-40,215,0x01,0x05,"A-40",0105)
	Pid *pid0106 = new Pid();
*/
	pidList.append(new Pid(0x01,0x00,"SupportedPids","Supported Pids 0x00-0x1F","","","0100",0,0,true));
	pidList.append(new Pid(0x01,0x01,"MonitorStatus","Monitor Status since DTC Cleared","","","0101",0,0,true));
	pidList.append(new Pid(0x01,0x02,"FreezeDTC","Freeze Frames DTC Codes","","","0102",0,0,true));
	pidList.append(new Pid(0x01,0x03,"Fuel Status","Fuel System Status","","","0103",0,0,true));
	pidList.append(new Pid(0x01,0x04,"Load","Calculated Engine Load","%","A*(100/255)","0104",0,100,false));
	pidList.append(new Pid(0x01,0x05,"Temp","Coolant Temperature","C","A-40","0105",-40,215,false));
	pidList.append(new Pid(0x01,0x06,"STFT-B1","Short Term Fuel Trim - Bank 1","%","(A-128)*100/128","0106",-100,100,false));
	pidList.append(new Pid(0x01,0x07,"LTFT-B1","Long Term Fuel Trim - Bank 1","%","(A-128)*100/128","0107",-100,100,false));
	pidList.append(new Pid(0x01,0x08,"STFT-B2","Short Term Fuel Trim - Bank 2","%","(A-128)*100/128","0108",-100,100,false));
	pidList.append(new Pid(0x01,0x09,"LTFT-B2","Long Term Fuel Trim - Bank 2","%","(A-128)*100/128","0109",-100,100,false));
	pidList.append(new Pid(0x01,0x0A,"Fuel Pressure","Fuel Pressure","kPa","A*3","010A",0,765,false));
	pidList.append(new Pid(0x01,0x0B,"MAP","Manifold Absolute Pressure","kPa","A","010B",0,255,false));
	pidList.append(new Pid(0x01,0x0C,"RPM","Engine Speed","rpm","((A*256) + B) / 4","010C",0,17000,false));
	pidList.append(new Pid(0x01,0x0D,"Speed","Vehicle Speed","kph","A","010D",0,255,false));
	pidList.append(new Pid(0x01,0x0E,"Timing","Timing Advance","degrees","(A/2)-64","010E",-64,64,false));
	pidList.append(new Pid(0x01,0x0F,"Intake Air","Intake Air Temp","C","A-40","010F",-40,215,false));
	pidList.append(new Pid(0x01,0x10,"MAF","Mass Air Flow","g/s","((A*256) + B) / 100","0110",0,656,false));
	pidList.append(new Pid(0x01,0x11,"Throttle","Throttle Position","%","A*(100/255)","0111",0,100,false));
	pidList.append(new Pid(0x01,0x12,"Secondary Air","Commanded Secondary Air Status","b","","0112",0,0,true));
	pidList.append(new Pid(0x01,0x13,"O2 Sensors Present","Oxygen Sensors Present","","","0113",0,0,true));
	pidList.append(new Pid(0x01,0x14,"O2B1S1Voltage","Bank 1 Sensor 1 O2 Voltage","","","0114",0,0,true));
	pidList.append(new Pid(0x01,0x15,"O2B1S2Voltage","Bank 1 Sensor 2 O2 Voltage","","","0115",0,0,true));
	pidList.append(new Pid(0x01,0x16,"O2B1S3Voltage","Bank 1 Sensor 3 O2 Voltage","","","0116",0,0,true));
	pidList.append(new Pid(0x01,0x17,"O2B1S4Voltage","Bank 1 Sensor 4 O2 Voltage","","","0117",0,0,true));
	pidList.append(new Pid(0x01,0x18,"O2B2S1Voltage","Bank 2 Sensor 1 O2 Voltage","","","0118",0,0,true));
	pidList.append(new Pid(0x01,0x19,"O2B2S2Voltage","Bank 2 Sensor 2 O2 Voltage","","","0119",0,0,true));
	pidList.append(new Pid(0x01,0x1A,"O2B2S3Voltage","Bank 2 Sensor 3 O2 Voltage","","","011A",0,0,true));
	pidList.append(new Pid(0x01,0x1B,"O2B2S4Voltage","Bank 2 Sensor 4 O2 Voltage","","","011B",0,0,true));
	pidList.append(new Pid(0x01,0x1C,"OBDStandards","OBD Standards this vehicle conforms to","","","011C",0,0,true));
	pidList.append(new Pid(0x01,0x1D,"O2Sensors","Oxygen Sensors Present","","","011D",0,0,true));
	pidList.append(new Pid(0x01,0x1E,"AuxInput","Auxillary Input Status","","","011E",0,0,true));
	pidList.append(new Pid(0x01,0x1F,"RunTime","Run Time Since Engine Start","seconds","(A*256)+B","011F",0,65535,false));
	pidList.append(new Pid(0x01,0x21,"DistanceWithMIL","Distance traveled with MIL on","km","(A*256)+B","0121",0,65535,false));
	pidList.append(new Pid(0x01,0x22,"FuelPressure","Fuel Rail Pressure (relative to manifold vacume)","kPa","((A*256)+B)*0.079","0122",0,5177.256,false));
	pidList.append(new Pid(0x01,0x23,"FuelPressure","Fuel Rail Pressure (diesel, or direct inject gas)","kPa","((A*256)+B)*10","0123",0,655350,false));


	pidList.append(new Pid(0x01,0x2F,"FuelLevel","Fuel Level Input","%","A*(100/255)","012F",0,100,false));
	pidList.append(new Pid(0x01,0x46,"AmbientAir","Ambient Air Temp","C","A-40","0146",-40,215,false));



	pidList.append(new Pid(0x01,0x1F,"Engine Runtime","Time since engine start","s","(256*A)+B","011F",0,65535,false));


	pidList.append(new Pid(0x01,0x31,"DistanceSinceMILCleared","Distance since MIL Cleared","km","(A*256)+B","0131",0,65535,false));




	ModeSixInfo mid01;
	mid01.description = "Oxygen Sensor Monitor Bank 1 - Sensor 1";
	mid01.id = 0x01;
	modeSixInfoList.append(mid01);
	ModeSixInfo mid02;
	mid02.description = "Oxygen Sensor Monitor Bank 1 - Sensor 2";
	mid02.id = 0x02;
	modeSixInfoList.append(mid02);
	ModeSixInfo mid03;
	mid03.description = "Oxygen Sensor Monitor Bank 1 - Sensor 3";
	mid03.id = 0x03;
	modeSixInfoList.append(mid03);
	ModeSixInfo mid04;
	mid04.description = "Oxygen Sensor Monitor Bank 1 - Sensor 4";
	mid04.id = 0x04;
	modeSixInfoList.append(mid04);
	ModeSixInfo mid05;
	mid05.description = "Oxygen Sensor Monitor Bank 2 - Sensor 1";
	mid05.id = 0x05;
	modeSixInfoList.append(mid05);
	ModeSixInfo mid06;
	mid06.description = "Oxygen Sensor Monitor Bank 2 - Sensor 2";
	mid06.id = 0x06;
	modeSixInfoList.append(mid06);
	ModeSixInfo mid07;
	mid07.description = "Oxygen Sensor Monitor Bank 2 - Sensor 3";
	mid07.id = 0x07;
	modeSixInfoList.append(mid07);
	ModeSixInfo mid08;
	mid08.description = "Oxygen Sensor Monitor Bank 2 - Sensor 4";
	mid08.id = 0x08;
	modeSixInfoList.append(mid08);
	ModeSixInfo mid09;
	mid09.description = "Oxygen Sensor Monitor Bank 3 - Sensor 1";
	mid09.id = 0x09;
	modeSixInfoList.append(mid09);
	ModeSixInfo mid0A;
	mid0A.description = "Oxygen Sensor Monitor Bank 3 - Sensor 2";
	mid0A.id = 0x0A;
	modeSixInfoList.append(mid0A);
	ModeSixInfo mid0B;
	mid0B.description = "Oxygen Sensor Monitor Bank 3 - Sensor 3";
	mid0B.id = 0x0B;
	modeSixInfoList.append(mid0B);
	ModeSixInfo mid0C;
	mid0C.description = "Oxygen Sensor Monitor Bank 3 - Sensor 4";
	mid0C.id = 0x0C;
	modeSixInfoList.append(mid0C);
	ModeSixInfo mid0D;
	mid0D.description = "Oxygen Sensor Monitor Bank 4 - Sensor 1";
	mid0D.id = 0x0D;
	modeSixInfoList.append(mid0D);
	ModeSixInfo mid0E;
	mid0E.description = "Oxygen Sensor Monitor Bank 4 - Sensor 2";
	mid0E.id = 0x0E;
	modeSixInfoList.append(mid0E);
	ModeSixInfo mid0F;
	mid0F.description = "Oxygen Sensor Monitor Bank 4 - Sensor 3";
	mid0F.id = 0x0F;
	modeSixInfoList.append(mid0F);
	ModeSixInfo mid10;
	mid10.description = "Oxygen Sensor Monitor Bank 4 - Sensor 4";
	mid10.id = 0x10;
	modeSixInfoList.append(mid10);
	ModeSixInfo mid21;
	mid21.description = "Catalyst Monitor Bank 1";
	mid21.id = 0x21;
	modeSixInfoList.append(mid21);
	ModeSixInfo mid22;
	mid22.description = "Catalyst Monitor Bank 2";
	mid22.id = 0x22;
	modeSixInfoList.append(mid22);
	ModeSixInfo mid23;
	mid23.description = "Catalyst Monitor Bank 3";
	mid23.id = 0x23;
	modeSixInfoList.append(mid23);
	ModeSixInfo mid24;
	mid24.description = "Catalyst Monitor Bank 4";
	mid24.id = 0x24;
	modeSixInfoList.append(mid24);
	ModeSixInfo mid31;
	mid31.description = "EGR Monitor Bank 1";
	mid31.id = 0x31;
	modeSixInfoList.append(mid31);
	ModeSixInfo mid32;
	mid32.description = "EGR Monitor Bank 2";
	mid32.id = 0x32;
	modeSixInfoList.append(mid32);
	ModeSixInfo mid33;
	mid33.description = "EGR Monitor Bank 3";
	mid33.id = 0x33;
	modeSixInfoList.append(mid33);
	ModeSixInfo mid34;
	mid34.description = "EGR Monitor Bank 4";
	mid34.id = 0x34;
	modeSixInfoList.append(mid34);
	ModeSixInfo mid39;
	mid39.description = "EVAP Monitor (Cap Off)";
	mid39.id = 0x39;
	modeSixInfoList.append(mid39);
	ModeSixInfo mid3A;
	mid3A.description = "EVAP Monitor (0.090\")";
	mid3A.id = 0x3A;
	modeSixInfoList.append(mid3A);
	ModeSixInfo mid3B;
	mid3B.description = "EVAP Monitor (0.040\")";
	mid3B.id = 0x3B;
	modeSixInfoList.append(mid3B);
	ModeSixInfo mid3C;
	mid3C.description = "EVAP Monitor (0.020\")";
	mid3C.id = 0x3C;
	modeSixInfoList.append(mid3C);
	ModeSixInfo mid3D;
	mid3D.description = "Purge Flow Monitor";
	mid3D.id = 0x3D;
	modeSixInfoList.append(mid3D);
	ModeSixInfo mid41;
	mid41.description = "Oxygen Sensor Heater Monitor Bank 1 - Sensor 1";
	mid41.id = 0x41;
	modeSixInfoList.append(mid41);
	ModeSixInfo mid42;
	mid42.description = "Oxygen Sensor Heater Monitor Bank 1 - Sensor 2";
	mid42.id = 0x42;
	modeSixInfoList.append(mid42);
	ModeSixInfo mid43;
	mid43.description = "Oxygen Sensor Heater Monitor Bank 1 - Sensor 3";
	mid43.id = 0x43;
	modeSixInfoList.append(mid43);
	ModeSixInfo mid44;
	mid44.description = "Oxygen Sensor Heater Monitor Bank 1 - Sensor 4";
	mid44.id = 0x44;
	modeSixInfoList.append(mid44);

	ModeSixInfo mid45;
	mid45.description = "Oxygen Sensor Heater Monitor Bank 2 - Sensor 1";
	mid45.id = 0x45;
	modeSixInfoList.append(mid45);
	ModeSixInfo mid46;
	mid46.description = "Oxygen Sensor Heater Monitor Bank 2 - Sensor 2";
	mid46.id = 0x46;
	modeSixInfoList.append(mid46);
	ModeSixInfo mid47;
	mid47.description = "Oxygen Sensor Heater Monitor Bank 2 - Sensor 3";
	mid47.id = 0x47;
	modeSixInfoList.append(mid47);
	ModeSixInfo mid48;
	mid48.description = "Oxygen Sensor Heater Monitor Bank 2 - Sensor 4";
	mid48.id = 0x48;
	modeSixInfoList.append(mid48);


	ModeSixInfo mid49;
	mid49.description = "Oxygen Sensor Heater Monitor Bank 3 - Sensor 1";
	mid49.id = 0x49;
	modeSixInfoList.append(mid49);
	ModeSixInfo mid4A;
	mid4A.description = "Oxygen Sensor Heater Monitor Bank 3 - Sensor 2";
	mid4A.id = 0x4A;
	modeSixInfoList.append(mid4A);
	ModeSixInfo mid4B;
	mid4B.description = "Oxygen Sensor Heater Monitor Bank 3 - Sensor 3";
	mid4B.id = 0x4B;
	modeSixInfoList.append(mid4B);
	ModeSixInfo mid4C;
	mid4C.description = "Oxygen Sensor Heater Monitor Bank 3 - Sensor 4";
	mid4C.id = 0x4C;
	modeSixInfoList.append(mid4C);


	ModeSixInfo mid4D;
	mid4D.description = "Oxygen Sensor Heater Monitor Bank 4 - Sensor 1";
	mid4D.id = 0x4D;
	modeSixInfoList.append(mid4D);
	ModeSixInfo mid4E;
	mid4E.description = "Oxygen Sensor Heater Monitor Bank 4 - Sensor 2";
	mid4E.id = 0x4E;
	modeSixInfoList.append(mid4E);
	ModeSixInfo mid4F;
	mid4F.description = "Oxygen Sensor Heater Monitor Bank 4 - Sensor 3";
	mid4F.id = 0x4F;
	modeSixInfoList.append(mid4F);
	ModeSixInfo mid50;
	mid50.description = "Oxygen Sensor Heater Monitor Bank 4 - Sensor 4";
	mid50.id = 0x50;
	modeSixInfoList.append(mid50);

	ModeSixInfo mid61;
	mid61.description = "Heated Catalyst Monitor Bank 1";
	mid61.id = 0x61;
	modeSixInfoList.append(mid61);
	ModeSixInfo mid62;
	mid62.description = "Heated Catalyst Monitor Bank 2";
	mid62.id = 0x62;
	modeSixInfoList.append(mid62);
	ModeSixInfo mid63;
	mid63.description = "Heated Catalyst Monitor Bank 3";
	mid63.id = 0x63;
	modeSixInfoList.append(mid63);
	ModeSixInfo mid64;
	mid64.description = "Heated Catalyst Monitor Bank 4";
	mid64.id = 0x64;
	modeSixInfoList.append(mid64);


	ModeSixInfo mid71;
	mid71.description = "Secondary Air Monitor 1";
	mid71.id = 0x71;
	modeSixInfoList.append(mid71);
	ModeSixInfo mid72;
	mid72.description = "Secondary Air Monitor 2";
	mid72.id = 0x72;
	modeSixInfoList.append(mid72);
	ModeSixInfo mid73;
	mid73.description = "Secondary Air Monitor 3";
	mid73.id = 0x73;
	modeSixInfoList.append(mid73);
	ModeSixInfo mid74;
	mid74.description = "Secondary Air Monitor 4";
	mid74.id = 0x74;
	modeSixInfoList.append(mid74);

	ModeSixInfo mid81;
	mid81.description = "Fuel System Monitor Bank 1";
	mid81.id = 0x81;
	modeSixInfoList.append(mid81);
	ModeSixInfo mid82;
	mid82.description = "Fuel System Monitor Bank 2";
	mid82.id = 0x82;
	modeSixInfoList.append(mid82);
	ModeSixInfo mid83;
	mid83.description = "Fuel System Monitor Bank 3";
	mid83.id = 0x83;
	modeSixInfoList.append(mid83);
	ModeSixInfo mid84;
	mid84.description = "Fuel System Monitor Bank 4";
	mid84.id = 0x84;
	modeSixInfoList.append(mid84);

	ModeSixInfo midA1;
	midA1.description = "Mis-Fire Monitor General Data";
	midA1.id = 0xA1;
	modeSixInfoList.append(midA1);
	ModeSixInfo midA2;
	midA2.description = "Mis-Fire Cylinder 1 Data";
	midA2.id = 0xA2;
	modeSixInfoList.append(midA2);
	ModeSixInfo midA3;
	midA3.description = "Mis-Fire Cylinder 2 Data";
	midA3.id = 0xA3;
	modeSixInfoList.append(midA3);
	ModeSixInfo midA4;
	midA4.description = "Mis-Fire Cylinder 3 Data";
	midA4.id = 0xA4;
	modeSixInfoList.append(midA4);
	ModeSixInfo midA5;
	midA5.description = "Mis-Fire Cylinder 4 Data";
	midA5.id = 0xA5;
	modeSixInfoList.append(midA5);
	ModeSixInfo midA6;
	midA6.description = "Mis-Fire Cylinder 5 Data";
	midA6.id = 0xA6;
	modeSixInfoList.append(midA6);
	ModeSixInfo midA7;
	midA7.description = "Mis-Fire Cylinder 6 Data";
	midA7.id = 0xA7;
	modeSixInfoList.append(midA7);
	ModeSixInfo midA8;
	midA8.description = "Mis-Fire Cylinder 7 Data";
	midA8.id = 0xA8;
	modeSixInfoList.append(midA8);
	ModeSixInfo midA9;
	midA9.description = "Mis-Fire Cylinder 8 Data";
	midA9.id = 0xA9;
	modeSixInfoList.append(midA9);
	ModeSixInfo midAA;
	midAA.description = "Mis-Fire Cylinder 9 Data";
	midAA.id = 0xAA;
	modeSixInfoList.append(midAA);
	ModeSixInfo midAB;
	midAB.description = "Mis-Fire Cylinder 10 Data";
	midAB.id = 0xAB;
	modeSixInfoList.append(midAB);
	ModeSixInfo midAC;
	midAC.description = "Mis-Fire Cylinder 11 Data";
	midAC.id = 0xAC;
	modeSixInfoList.append(midAC);
	ModeSixInfo midAD;
	midAD.description = "Mis-Fire Cylinder 12 Data";
	midAD.id = 0xAD;
	modeSixInfoList.append(midAD);

	ModeSixInfo test01;
	test01.description = "Rich to lean sensor threshold voltage (constant)";
	test01.id = 0x01;
	modeSixTestList.append(test01);
	ModeSixInfo test02;
	test02.description = "Lean to Rich sensor threshold voltage (constant)";
	test02.id = 0x02;
	modeSixTestList.append(test02);
	ModeSixInfo test03;
	test03.description = "Low sensor voltage for switch time calculation (constant)";
	test03.id = 0x03;
	modeSixTestList.append(test03);
	ModeSixInfo test04;
	test04.description = "High sensor voltage for switch time calculation (constant)";
	test04.id = 0x04;
	modeSixTestList.append(test04);
	ModeSixInfo test05;
	test05.description = "Rich to lean sensor switch time (calculated)";
	test05.id = 0x05;
	modeSixTestList.append(test05);
	ModeSixInfo test06;
	test06.description = "Lean to rich sensor switch time (calculated)";
	test06.id = 0x06;
	modeSixTestList.append(test06);
	ModeSixInfo test07;
	test07.description = "Minimum sensor voltage for test cycle (calculated)";
	test07.id = 0x07;
	modeSixTestList.append(test07);
	ModeSixInfo test08;
	test08.description = "Maximum sensor voltage for test cycle (calculated)";
	test08.id = 0x08;
	modeSixTestList.append(test08);
	ModeSixInfo test09;
	test09.description = "Time between sensor transitions (calculated)";
	test09.id = 0x09;
	modeSixTestList.append(test09);
	ModeSixInfo test0A;
	test0A.description = "Sensor period (calculated)";
	test0A.id = 0x0A;
	modeSixTestList.append(test0A);
	ModeSixInfo test0B;
	test0B.description = "Misfire counts for last 10 driving cycles (calculated)";
	test0B.id = 0x0B;
	modeSixTestList.append(test0B);
	ModeSixInfo test0C;
	test0C.description = "Misfire counts for last/current driving cycle (calculated)";
	test0C.id = 0x0C;
	modeSixTestList.append(test0C);

	ModeSixScalers scale01;
	scale01.usaid = 0x01;
	scale01.offset = 0;
	scale01.description = "Raw Value";
	scale01.multiplier = 1.0;
	scale01.min = 0;
	scale01.max = 0xFFFF;
	modeSixScalerList.append(scale01);
	ModeSixScalers scale02;
	scale02.usaid = 0x02;
	scale02.offset = 0;
	scale02.description = "Raw Value";
	scale02.multiplier = 0.1;
	scale02.min = 0;
	scale02.max = 0xFFFF;
	modeSixScalerList.append(scale02);
	ModeSixScalers scale03;
	scale03.usaid = 0x03;
	scale03.offset = 0;
	scale03.description = "Raw Value";
	scale03.multiplier = 0.01;
	scale03.min = 0;
	scale03.max = 0xFFFF;
	modeSixScalerList.append(scale03);
	ModeSixScalers scale05;
	scale05.usaid = 0x05;
	scale05.offset = 0;
	scale05.description = "Raw Value";
	scale05.multiplier = 0.0000305;
	scale05.min = 0;
	scale05.max = 0xFFFF;
	modeSixScalerList.append(scale05);

	ModeSixScalers scale0A;
	scale0A.usaid = 0x0A;
	scale0A.offset = 0;
	scale0A.description = "Voltage";
	scale0A.units = "mV";
	scale0A.multiplier = 0.122;
	scale0A.min = 0;
	scale0A.max = 0xFFFF;
	modeSixScalerList.append(scale0A);
	ModeSixScalers scale0E;
	scale0E.offset = 0;
	scale0E.usaid = 0x0E;
	scale0E.description = "Amps";
	scale0E.units = "A";
	scale0E.multiplier = 0.001;
	scale0E.min = 0;
	scale0E.max = 0xFFFF;
	modeSixScalerList.append(scale0E);
	ModeSixScalers scale10;
	scale10.usaid = 0x10;
	scale10.offset = 0;
	scale10.description = "Time";
	scale10.units = "ms";
	scale10.multiplier = 1.0;
	scale10.min = 0;
	scale10.max = 0xFFFF;
	modeSixScalerList.append(scale10);
	ModeSixScalers scale16;
	scale16.usaid = 0x16;
	scale16.description = "Temperature";
	scale16.units = "C";
	scale16.offset = -40;
	scale16.multiplier = 0.1;
	scale16.min = 0;
	scale16.max = 0xFFFF;
	modeSixScalerList.append(scale16);
	ModeSixScalers scale1C;
	scale1C.usaid = 0x1C;
	scale1C.description = "Angle";
	scale1C.units = "degrees";
	scale1C.offset = 0;
	scale1C.multiplier = 0.01;
	scale1C.min = 0;
	scale1C.max = 0xFFFF;
	modeSixScalerList.append(scale1C);
	ModeSixScalers scale20;
	scale20.usaid = 0x20;
	scale20.description = "Ratio";
	scale20.units = "R";
	scale20.offset = 0;
	scale20.multiplier = 0.0039062;
	scale20.min = 0;
	scale20.max = 0xFFFF;
	modeSixScalerList.append(scale20);
	ModeSixScalers scale24;
	scale24.usaid = 0x24;
	scale24.offset = 0;
	scale24.description = "Counts";
	scale24.units = "Count";
	scale24.multiplier = 1;
	scale24.min = 0;
	scale24.max = 0xFFFF;
	modeSixScalerList.append(scale24);
	ModeSixScalers scale30;
	scale30.usaid = 0x30;
	scale30.offset = 0;
	scale30.description = "Percentage";
	scale30.units = "%";
	scale30.offset = 0;
	scale30.multiplier = 0.001526;
	scale30.min = 0;
	scale30.max = 0xFFFF;
	modeSixScalerList.append(scale30);
	ModeSixScalers scaleA9;
	scaleA9.usaid = 0xA9;
	scaleA9.offset = 0;
	scaleA9.description = "Pressure/Second";
	scaleA9.units = "Pa/s";
	scaleA9.offset = 0;
	scaleA9.multiplier = 0.25;
	scaleA9.min = 0;
	scaleA9.max = 0xFFFF;
	modeSixScalerList.append(scaleA9);
	ModeSixScalers scaleFE;
	scaleFE.usaid = 0xFE;
	scaleFE.offset = 0;
	scaleFE.description = "Pressure";
	scaleFE.units = "Pa";
	scaleFE.offset = -8192;
	scaleFE.multiplier = 0.25;
	scaleFE.min = 0;
	scaleFE.max = 0xFFFF;
	modeSixScalerList.append(scaleFE);


//NEED SCALARS 0xB1



}
ObdInfo::ModeSixInfo ObdInfo::getInfoFromByte(int obdmid)
{
	for (int i=0;i<modeSixInfoList.size();i++)
	{
		if (modeSixInfoList[i].id == obdmid)
		{
			return modeSixInfoList[i];
		}
	}
	ModeSixInfo info;
	info.id = 0;
	return info;
}

ObdInfo::ModeSixInfo ObdInfo::getTestFromByte(int tid)
{
	for (int i=0;i<modeSixTestList.size();i++)
	{
		if (modeSixTestList[i].id == tid)
		{
			return modeSixTestList[i];
		}
	}
	ModeSixInfo info;
	info.id=0;
	return info;
}
ObdInfo::ModeSixScalers ObdInfo::getScalerFromByte(int UASID)
{
	for (int i=0;i<modeSixScalerList.size();i++)
	{
		if (modeSixScalerList[i].usaid == UASID)
		{
			return modeSixScalerList[i];
		}
	}
	qDebug() << "Unable to find scaler" << QString::number(UASID,16);
	ModeSixScalers scale;
	scale.usaid = 0;
	return scale;
}

ObdInfo::Pid* ObdInfo::getPidFromString(QString str)
{
	for (int i=0;i<pidList.count();i++)
	{
		if (pidList[i]->stringRep == str)
		{
			return pidList[i];
		}
	}
	return 0;
}
ObdInfo::Pid *ObdInfo::getPidFromBytes(int mode,int pid)
{
	for (int i=0;i<pidList.count();i++)
	{
		if (pidList[i]->mode == mode && pidList[i]->pid == pid)
		{
			return pidList[i];
		}
	}
	return 0;
}
int ObdInfo::intFromHex(QString num)
{
	int final = 0;
	for (int i=0;i<num.length();i++)
	{
		if (num.at(i).toAscii() >=48 && num.at(i).toAscii() <=57)
		{
			if ((16 << (int)(4*((num.length()-2)-i))) == 0)
			{
				final += (num.at(i).toAscii() - 48);
			}
			else
			{
				final += (num.at(i).toAscii() - 48) * ((16 << (int)(4 * ((num.length()-2) - i))));
			}
		}
		else if (num.at(i).toUpper().toAscii() >= 65 && num.at(i).toUpper().toAscii() <= 70)
		{
			final += 10 + (num.at(i).toUpper().toAscii() - 65) * pow(16.0,(num.length()-1) - i);
		}
	}
	return final;
}
ObdInfo::Pid::Pid(unsigned char amode,unsigned char apid,QString aname,QString adescription,QString aunit,QString afunction,QString astringrep,float amin, float amax,bool abitencoded)
{
	mode = amode;
	pid = apid;
	name = aname;
	description = adescription;
	unit = aunit;
	function = afunction;
	stringRep = astringrep;
	min = amin;
	max = amax;
	bitencoded = abitencoded;
}

ObdInfo::Pid::Pid()
{

}
