/**************************************************************************
*   Copyright (C) 2010                                                    *
*   Michael Carpenter (malcom2073) <malcom2073@gmail.com>                 *
*   Kevron Rees (tripzero) tripzero.kev@gmail.com                         *
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


#ifndef OBDTHREAD_H
#define OBDTHREAD_H
#include <QThread>
#include <QString>
#include <QList>
#include <QDebug>
#include <QMutex>
#include "ObdInfo.h"
#include "obdlib.h"
#include <QByteArray>
#include <QVector>
#include <qglobal.h>
#ifndef WINHACK
#define WINHACK Q_OS_WIN32
#endif


#define LIBOBD_VERSION_MAJOR 1
#define LIBOBD_VERSION_MINOR 0
#define LIBOBD_VERSION_PATCH 0


void debugCallback(char *text,void *ptr);


//! Main class for interfacing with ELM based scan tools.
/*! This is the primary class for using libobd. Upon instantiation, you should call ObdThread::start() to start the event loop, 
then connect your slots to ObdThread signals. Then you can start sending requests via ObdThread::addRequest, or functions such 
as ObdThread::sendReqSupportedModes. \n\n Example usage:\n\n
ObdThread *thread = new Obdthread();\n
connect(thread,SIGNAL(pidReply(QString,QString,int,double)),this,SLOT(obdPidReply(QString,QString,int,double)));\n
thread->start();\n
thread->addRequest(0x01,0x0C,1,0);\n

void MyClass::obdPidReply(QString pid,QString val, int set, double time)\n
{\n
	qDebug() << "pid is:" << pid << "val is:" << val << "set is:" << set << "time is:" << time;\n
}\n
\n\n\n
This will give an output of something along the lines of\n
"pid is: 010C val is: 35 set is: 1 time is: 123350304"\n\n

A few useful notes:\n
It is considered safe to call val.toDouble().\n
A way to convert from pid to two unsigned char values is included with obdLib:\n
unsigned char mode = obdLib::byteArrayToByte(pid[0].toAscii(),pid[1].toAscii());\n
unsigned char pid = obdLib::byteArrayToByte(pid[2].toAscii(),pid[3].toAscii());\n
*/
class ObdThread : public QThread
{
	Q_OBJECT
public:
	//! OBD Library Errors
	/*! Thie enum is used to describe obd errors */
	enum ObdError
	{
		UNABLE_TO_OPEN_COM_PORT,
		READ_ERROR,
		WRITE_ERROR
	};

	//! Types of requests in the ObdThread event loop
	/*! These are generally only used internally, however they are available for use
for custom requests. */
	enum RequestType
	{
		TROUBLE_CODES,
		CLEAR_TROUBLE_CODES,
		MODE_PID,
		RAW_REQUEST,
		RAW_BLIND_REQUEST,
		CUSTOM,
		FREEZE_FRAME,
		NONE,
		REQ_SUPPORTED_PIDS,
		SCAN_ALL,
		CONNECT,
		DISCONNECT,
		START_REQ_LOOP,
		STOP_REQ_LOOP,
		START_MONITOR,
		ST_START_MONITOR,
		ELM_COMMAND,
		ST_START_FILTER_MONITOR,
		STOP_MONITOR,
		FIND_PORT,
		VOLTAGE,
		SWITCH_BAUD,
		MONITOR_STATUS,
		MFG_STRING_ONE,
		ON_BOARD_MONITORS,
		REQ_SUPPORTED_MODES
	};
	enum CONTINUOUS_MONITOR
	{
		MISFIRE,
		FUEL_SYSTEM,
		COMPONENTS,
		CATALYST,
		HEATED_CATALYST,
		EVAPORATIVE_SYSTEM,
		SECONDARY_AIR_SYSTEM,
		AC_REFRIGERANT,
		OXYGEN_SENSOR,
		OXYGEN_SENSOR_HEATER,
		EGR_SYSTEM,
		NMHC_CAT,
		NOX_SCR_MONITOR,
		BOOST_PRESSURE,
		EXHAUST_GAS_SENSOR,
		PM_FILTER_MONITORING,
		EGR_VVT_SYSTEM
	};
	enum MONITOR_COMPLETE_STATUS
	{
		COMPLETE,
		INCOMPLETE,
		UNAVAILABLE
	};


	//! RequestClass
	/*! Req */
	class RequestClass
	{
	public:
		RequestClass()
		{
			mode = 0;
			pid = 0;
			type = NONE;
			time = 0;
			priority=0;
			repeat = false;
			wait=0;
		}
		RequestClass(int pmode, int ppid, int ppriority, int pwait)
		{
			mode = pmode;
			pid = ppid;
			type = MODE_PID;
			wait = pwait;
			time = 0;
			priority = ppriority;
			repeat = true;
		}

		RequestType type;
		int mode;
		int wait;
		int pid;
		unsigned long time;
		int priority;
		bool repeat;
		QByteArray custom;
	};

	//! Constructor
	/*! Setting a parent is optional, since ObdThread generally lives for the
	entire life of the application */
	ObdThread(QObject *parent=0);

public slots:
	void debug(QString msg,obdLib::DebugLevel level);
	void commsDebug(QString msg);
	void setDebugLevel(obdLib::DebugLevel level);

	//! Set the port on which ObdThread operatoes
	/*! This should be something like COM1, COM5, or //./COM20 on windows. /dev/ttyS0 or /dev/ttyUSB0 on *nix systems */
	void setPort(QString port);

	//! Set the baud rate which ObdThread opens the serial port with
	/*! Valid baud rates include 9600, 38400, 115200, and a few others. These three are the primary ones
	that most tools use. */
	void setBaud(int baud);

	//! Adds a repeat mode/pid request to the queue
	/*! This will add a request to the queue to be repeaidly requested. Example: obdThread->addRequest(0x01,0x0D,1,1);

	\param mode integer mode, 0x01 or 0x05 for instance
	\param pid integer pid, 0x0D for vehicle speed
	\param priority how often the pid gets requested. 1 is every cycle, 3 is every third cycle, etc
	\param wait This should be set to 0 unless you know what you're doing.*/
	void addRequest(int mode, int pid, int priority,int wait);

	//! Removes a repeat mode/pid request to the queue
	/*! This will remove a previously added mode/pid request that matches mode, pid, and priority.
	\param mode integer mode, 0x01 or 0x05 for instance
	\param pid integer pid, 0x0D for vehicle speed
	\param priority hot often the pid gets requested */
	void removeRequest(int mode, int pid, int priority);

	//! Convenience function for adding requests to the event loop.
	/*! This will add a request defined by a RequestClass object. This can be any type of request that is accepted by ObdThread.
	\param req RequestClass object */
	void addRequest(RequestClass req);

	void removeRequest(RequestClass req);
	void sendReqSupportedPids();
	//void setInfo(ObdInfo *info) { m_obdInfo = info; }
	ObdInfo *getInfo() { return m_obdInfo; }

	//! Connect to an OBD scan tool
	/*! Connects to a scan tool previously defined by ObdThread::setPort and ObdThread::setBaud */
	void connect(bool init=true);

	//! Disconnects from an OBD scan tool
	/*! Disconnects from the currently connected scan tool */
	void disconnect();

	//! Clears request list
	/*! Clears the entire request list. This goes  in as a request, so it may not take effect immediatly,
	and some requests may still be processed before this request goes through and clears the list */
	void clearReqList();

	//! Sends a request to clear trouble codes
	/*! Adds a request to clear trouble codes. This request will have no response */
	void sendClearTroubleCodes();

	//! Sends a request for Mode 06 onboard monitors
	/*! Adds a request to retrieve Mode 06 on board monitors, if supported. */
	void sendReqOnBoardMonitors();

	//! Sends a request to the scantool to read voltage
	/*! Uses "ATRV\r" for this request */
	void sendReqVoltage();


	void sendReqSupportedModes();
	void sendReqMfgString();
	void sendReqFullPidScan();
	void switchBaud();


	//! Start monitor mode
	/*! This uses ATMA tQStringo monitor the OBD bus. Protocol must be set before this point for this to do anything*/
	void startMonitorMode();
	//! Stop monitor mode
	void stopMonitorMode();

	//! Set protocol of Obdlink MX
	/*! This uses STP XX to set the protocol*/
	void MX_setProtocol(int num);

	//! Set SW Can mode of Obdlink MX
	/*! This uses STCWM X to set the mode.*/
	void MX_setSWCanMode(int num);

	//! Set the OBD baud rate of Obdlink MX
	/*! Uses STPBR to set the baud rate*/
	void MX_setBaudRate(int baud);

	//! Check baud rate of Obdlink MX
	/*! This uses STPBRR to check the baud rate and see if it is equal to what was set with MX_setBaudRate*/
	void MX_checkBaudRate();

	//! Add Pass CAN filter
	/*! filter is in the form "pattern,mask", such as ST_addPassFilter("07E8,07FF"); */
	void ST_addPassFilter(QString filter);

	//! Add Block CAN filter
	void ST_addBlockFilter(QString filter);

	//! Add Flow Control CAN filter
	void ST_addFlowControlFilter(QString filter);

	//! Send raw can message.
	/*! This message should include the appriate header for 11 or 29bit, whichever is selected*/
	void sendCanMessage(QString msg,bool is29Bit);

	//! Clear Pass CAN filters
	void ST_clearPassFilters();
	//! Clear Block CAN filters
	void ST_clearBlockFilters();
	//! Clear Flow Control CAN filters
	void ST_clearFlowFilters();
	//! Start STN1100 specific monitor mode
	void ST_startMonitorMode();
	//! Stop STN1100 specific monitor mode
	void ST_stopMonitorMode();

	//! Start STN1100 specific filtered monitor mode
	void ST_startFilterMonitorMode();
	//! Stop STN1100 specific filtered monitor omde
	void ST_stopFilterMonitorMode();


	void sendSingleShotRequest(QByteArray request);
	void sendSingleShotBlindRequest(QByteArray request);
	void stopThread() { m_threadRunning = false; }
	void sendReqTroubleCodes();

	//! Starts the ObdThread event loop
	/*! This function starts the thread associated with libobd. This function must be called before any other functions, however it is 
	acceptable to connect signals before calling this function. */
	void start();

	void sendReqMonitorStatus();
	QString port() { return m_port; }

	//! Convenience function for finding which port the ELM device is connected to
	/*! This function cycles through serial ports on the computer, sending ATI commands in an attempt to find the ELM device */
	void findObdPort();

	void sendElmCommand(QString cmd);
	void setHeader(bool on);
	void setEcho(bool on);
	void setLineFeed(bool on);
	QString version() { return QString::number(LIBOBD_VERSION_MAJOR) + "." + QString::number(LIBOBD_VERSION_MINOR) + "." + QString::number(LIBOBD_VERSION_PATCH); }

private:
	void run();
	QStringList parseCode(QString code,QString type);
	obdLib::DebugLevel m_dbgLevel;
	QMutex threadLockMutex;
	QMutex loopTypeMutex;
	QMutex removePidMutex;
	bool m_obdConnected;
	ObdInfo *m_obdInfo;
	volatile bool m_monitorMode;
	int loopType;
	QList<RequestClass> m_reqClassList;
	QList<RequestClass> m_reqClassListThreaded;
	QList<RequestClass> m_reqClassRemoveList;
	QMap<RequestClass*,int> m_reqClassFailureMap;
	QMap<RequestClass*,bool> m_whiteList;
	QList<QString> m_reqList;
	QList<int> m_reqPriority;
	QList<int> m_errorCount;
	QString m_port;
	int m_baud;
	volatile bool m_threadRunning;
	bool m_requestLoopRunning;
	obdLib *m_obd;
	QString parse(QString str);
	QString calc(QString str);
	bool m_connect(bool init=true);
	bool m_setHeader(bool on);
	bool m_setEcho(bool on);
	bool m_setLineFeed(bool on);
	bool m_initElm();
	bool m_resetElm();
	QString m_getElmVersion();
	QString m_getProtocolName();
	void m_setProtocol(int num, bool autosearch);

signals:
	//void monitorTestReply(QList<QString> list);

	//! Monitor mode reply signal.
	/*! This function is called for any of the three monitor mode functions, startMonitorMode, ST_startMonitorMode, and ST_startFilterMonitorMode
		line is the newline terminated result from the vehicle. Each message should be its own line, and each line should be a complete
		message	*/
	void monitorModeLine(QByteArray line);

	//! Continuous onboard monitoring test reply
	void monitorTestReply(QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS> monitorlist);
	//! Mode $06 reply
	void onBoardMonitoringReply(QList<unsigned char> midlist,QList<unsigned char> tidlist,QList<QString> vallist,QList<QString> minlist,QList<QString> maxlist,QList<QString> passlist);

	//! OBD tool manufacturer
	void mfgStringReply(QString string);
	void liberror(ObdThread::ObdError err);
	void voltageReply(double volts);
	void supportedModesReply(QList<QString> list);
	void connected(QString version);
	void disconnected();
	void reqLoopStarted();
	void reqLoopStopped();
	void elmCommandFailed(QString command);
	//! Main reply function for standard mode 1 pid requests.
	void pidReply(QString pid,QString val,int set,double time);
	void singleShotReply(QByteArray request, QByteArray list);
	void supportedPidsReply(QList<QString> list);
	void troubleCodesReply(QString ecu, QList<QString> codes);
	void consoleMessage(QString message);
	void obdPortFound(QString portname);
	void protocolReply(QString protocol);

	//! Signal for raw serial communications logging
	void rawCommsMessage(QString msg);

	//! Debugging signal
	void debugMessage(QString msg,obdLib::DebugLevel lvl);
};
#endif //OBDTHREAD_H
