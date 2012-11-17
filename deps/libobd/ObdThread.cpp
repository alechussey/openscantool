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

#include "ObdThread.h"
#include <QVector>
#include <QDateTime>
#include <QMetaType>
#include <QStringList>
#define CR "\015"
#define LF "\012"




void debugCallback(const char *text,void *ptr,obdLib::DebugLevel lvl)
{
	ObdThread *obd = qobject_cast<ObdThread*>(static_cast<QObject*>(ptr));
	if (obd && ptr)
	{
		obd->debug(QString(text),lvl);
	}
	else
	{
		qDebug() << "Error casting debugcallback pointer to ObdThread. This should not happen!!!";
		return;
	}
}

void commsCallback(const char *text,void *ptr)
{
	ObdThread *obd = qobject_cast<ObdThread*>(static_cast<QObject*>(ptr));
	if (obd && ptr)
	{
		obd->commsDebug(QString(text));
	}
	else
	{
		qDebug() << "Error casting commscallback pointer to ObdThread. This should not happen!!!";
		return;
	}
}


ObdThread::ObdThread(QObject *parent) : QThread(parent)
{
	qRegisterMetaType<QList<QString> >("QList<QString>");
	qRegisterMetaType<ObdThread::ObdError>("ObdThread::ObdError");
	qRegisterMetaType<QList<unsigned char> >("QList<unsigned char>");
	qRegisterMetaType<QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS> >("QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS>");
	m_obd = new obdLib();
	m_threadRunning = false;
	m_requestLoopRunning = false;
	loopType = 0;
	//m_set = 0;
	//threadLockMutex.lock();
	m_obdConnected = false;
	m_baud = 0;
	m_port = "";
	m_obdInfo = new ObdInfo();
	m_dbgLevel = obdLib::DEBUG_VERY_VERBOSE;
	m_obd->setDebugCallback(&debugCallback,this);
	m_obd->setCommsCallback(&commsCallback,this);
	start();
}
void ObdThread::debug(QString msg,obdLib::DebugLevel level)
{
	if (level >= m_dbgLevel)
	{
		qDebug() << "obdlib debug callback:" << msg;
	}
	emit debugMessage(msg,level);
}
void ObdThread::commsDebug(QString msg)
{
	emit rawCommsMessage(msg);
	//qDebug() << "Comms:" << msg;
}
void ObdThread::startMonitorMode()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = START_MONITOR;
	m_monitorMode = true;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::stopMonitorMode()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = STOP_MONITOR;
	m_monitorMode = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::setDebugLevel(obdLib::DebugLevel level)
{
	m_dbgLevel = level;
}

void ObdThread::connect(bool init)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = CONNECT;
	if (init)
	{
		req.custom.append("1");
	}
	else
	{
		req.custom.append("0");
	}
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::start()
{
	m_threadRunning = true;
	QThread::start();
}
void ObdThread::sendReqMonitorStatus()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = MONITOR_STATUS;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::disconnect()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = DISCONNECT;
	m_reqClassList.append(req);
	threadLockMutex.unlock();

}
void ObdThread::clearReqList()
{
	debug("ObdThread::clearReqList()",obdLib::DEBUG_VERBOSE);
	removePidMutex.lock();
	m_reqClassListThreaded.clear();
	removePidMutex.unlock();
}
void ObdThread::sendReqTroubleCodes()
{
	debug("ObdThread::sendReqTroubleCodes()",obdLib::DEBUG_VERBOSE);
	//qDebug() << "Trouble codes request...";
	threadLockMutex.lock();
	RequestClass req;
	req.repeat = false;
	req.type = TROUBLE_CODES;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::switchBaud()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = SWITCH_BAUD;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendReqOnBoardMonitors()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ON_BOARD_MONITORS;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::sendSingleShotRequest(QByteArray request)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	req.custom = request;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendReqVoltage()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = VOLTAGE;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendReqMfgString()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = MFG_STRING_ONE;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::setHeader(bool on)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ELM_COMMAND;
	req.custom = QByteArray("ATH") + (on ? "1" : "0") + "\r";
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendElmCommand(QString cmd)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ELM_COMMAND;
	req.custom = QByteArray(cmd.append("\r").toAscii());
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::setEcho(bool on)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ELM_COMMAND;
	req.custom = QByteArray("ATE") + (on ? "1" : "0") + "\r";
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::setLineFeed(bool on)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ELM_COMMAND;
	req.custom = QByteArray("ATL") + (on ? "1" : "0") + "\r";
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendReqSupportedModes()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = REQ_SUPPORTED_MODES;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::sendSingleShotBlindRequest(QByteArray request)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_BLIND_REQUEST;
	req.custom = request;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::findObdPort()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = FIND_PORT;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::removeRequest(int mode, int pid, int priority)
{
	threadLockMutex.lock();
	RequestClass req;
	req.pid = pid;
	req.mode = mode;
	req.priority = priority;
	req.type = MODE_PID;
	m_reqClassRemoveList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::addRequest(RequestClass req)
{
	threadLockMutex.lock();
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::removeRequest(RequestClass req)
{
	threadLockMutex.lock();
	m_reqClassRemoveList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::MX_setProtocol(int num)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STP" + QString::number(num) + "\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::MX_setSWCanMode(int num)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STCSWM" + QString::number(num) + "\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::ST_addPassFilter(QString filter)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STFAP" + filter + "\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::ST_addBlockFilter(QString filter)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STFAB" + filter + "\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::ST_startMonitorMode()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ST_START_MONITOR;
	m_monitorMode = true;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::ST_stopMonitorMode()
{
	m_monitorMode = false;
}

void ObdThread::ST_startFilterMonitorMode()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = ST_START_FILTER_MONITOR;
	m_monitorMode = true;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::ST_stopFilterMonitorMode()
{
	m_monitorMode = false;
}

void ObdThread::ST_addFlowControlFilter(QString filter)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STFAFC" + filter + "\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::ST_clearPassFilters()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STFCP\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::ST_clearBlockFilters()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STFCB\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::ST_clearFlowFilters()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STFCFC\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendCanMessage(QString msg,bool is29Bit)
{
	msg = msg.replace(" ","");
	threadLockMutex.lock();
	if (is29Bit)
	{
		RequestClass priority;
		priority.type = RAW_REQUEST;
		QByteArray reqbytes;
		reqbytes.append("ATCP" + msg.left(2) +"\r");
		priority.custom = reqbytes;
		priority.repeat = false;

		RequestClass header;
		header.type = RAW_REQUEST;
		reqbytes.clear();
		reqbytes.append("ATSH" + msg.mid(2,4) + "\r");
		header.custom = reqbytes;
		header.repeat = false;

		RequestClass message;
		message.type = RAW_REQUEST;
		reqbytes.clear();
		reqbytes.append(msg.mid(6) + "\r");
		message.custom = reqbytes;
		message.repeat = false;

		m_reqClassList.append(priority);
		m_reqClassList.append(header);
		m_reqClassList.append(message);
	}
	else
	{
		QByteArray reqbytes;
		RequestClass header;
		header.type = RAW_REQUEST;
		reqbytes.clear();
		reqbytes.append("ATSH " + msg.left(3) + "\r");
		header.custom = reqbytes;
		header.repeat = false;

		RequestClass message;
		message.type = RAW_REQUEST;

		reqbytes.append(msg.left(3) +"\r");
		message.custom = reqbytes;
		message.repeat = false;
		//reqbytes.append("ATSH" + QString::number(baud) + "\r");
		m_reqClassList.append(header);
		m_reqClassList.append(message);
	}
	threadLockMutex.unlock();
}

void ObdThread::MX_setBaudRate(int baud)
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STPBR " + QString::number(baud) + "\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::MX_checkBaudRate()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = RAW_REQUEST;
	QByteArray reqbytes;
	reqbytes.append("STPBRR\r");
	req.custom = reqbytes;
	req.repeat = false;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

QStringList ObdThread::parseCode(QString codetoparse,QString type)
{
	QStringList codes;
	bool inCode = false;
	int count =0;
	QString codetype ="";
	for (int j=0;j<codetoparse.size();j++)
	{
		if (codetoparse[j] == '4' && codetoparse[j+1] == '3' && inCode == false)
		{
			inCode = true;
			//Funny little thing. A8 (And A7) have size indicators after the FIRST 43, but not after the second.
			if (((type == "A8") || /*(type == "8") ||*/ (type == "7") || (type == "6") || (type == "A7") || type == "A6") && codes.size() == 0)
			{
				j+=4;
			}
			else
			{
				j+=2;
			}
		}
		if (inCode)
		{
			count++;
			unsigned char onefirst = m_obd->byteArrayToByte(codetoparse.toAscii().at(j),codetoparse.toAscii().at(j+1));
			unsigned char onesecond = m_obd->byteArrayToByte(codetoparse.toAscii().at(j+2),codetoparse.toAscii().at(j+3));
			if (!(onefirst >> 7) & 1)
			{
				if (!((onefirst >> 6) & 1))
				{
					//qDebug() << "Powertrain code";
					codetype = "P";
				}
				else if ((onefirst >> 6) & 1)
				{
					//qDebug() << "Chassis code";
					codetype = "C";
				}
			}
			else
			{
				if (!((onefirst >> 6) & 1))
				{
					//qDebug() << "Body code";
					codetype = "B";
				}
				else
				{
					//qDebug() << "Network code";
					codetype = "U";
				}
			}
			unsigned char second = (onefirst >> 4) & 0x3;
			unsigned char third = (onefirst) & 0xF;
			unsigned char fourth = ((onesecond >> 4) & 0xF);
			unsigned char fifth = (onesecond & 0xF);
			//We don't want to return an empty code. All 0's is just padding
			if (!((second == 0 ) && (third == 0) && (fourth == 0) && (fifth == 0)))
			{
				qDebug() << QString::number(second) << QString::number(third) << QString::number(fourth) << QString::number(fifth);
				codes.append(codetype + QString::number(second) + QString::number(third) + QString::number(fourth) + QString::number(fifth));
			}

			j += 3;
			if (codes.size() == 3 || codes.size() == 6)
			{
				//Protocol type A8 does not have another 43 seperating code chunks.
				//All other protocols do I think.
				if (type != "A8" && type != "A7" && type != "A6")
				{
					inCode = false;
				}
			}
		}

	}
	return codes;
}

void ObdThread::run()
{
	int cycleCounter =0;
	bool first = true;
	m_requestLoopRunning = true;
	std::vector<unsigned char> replyVectorString;
	std::vector<unsigned char> replyVector;
	while (m_threadRunning)
	{
		removePidMutex.lock();
		threadLockMutex.lock();
		for (int i=0;i<m_reqClassListThreaded.size();i++)
		{
			if (!m_reqClassListThreaded[i].repeat)
			{
				m_reqClassFailureMap.remove(&m_reqClassListThreaded[i]);
				m_reqClassListThreaded.removeAt(i);
				i--;
			}
		}
		for (int i=0;i<m_reqClassRemoveList.count();i++)
		{
			//qDebug() << "Removing pid:" << m_reqClassRemoveList[i].mode << m_reqClassRemoveList[i].pid;
			for (int j=0;j<m_reqClassListThreaded.count();j++)
			{
				if ((m_reqClassRemoveList[i].mode == m_reqClassListThreaded[j].mode) && (m_reqClassRemoveList[i].pid == m_reqClassListThreaded[j].pid) && (m_reqClassRemoveList[i].type == m_reqClassListThreaded[j].type))
				{
					m_reqClassFailureMap.remove(&m_reqClassListThreaded[j]);
					m_reqClassListThreaded.removeAt(j);
					m_reqClassRemoveList.removeAt(i);
					j--;
					i--;
				}
			}
		}
		m_reqClassRemoveList.clear();
		for (int i=0;i<m_reqClassList.count();i++)
		{
			//qDebug() << "Adding req" << m_reqClassList[i].type;
			//m_reqClassListThreaded.prepend(m_reqClassList[i]);
			m_reqClassListThreaded.append(m_reqClassList[i]);
			m_reqClassFailureMap[&m_reqClassList[i]] = 0;
			m_whiteList[&m_reqClassList[i]]=false;
		}
		m_reqClassList.clear();
		threadLockMutex.unlock();
		first = true;
		//qDebug() << "Size:" << m_reqClassListThreaded.size();
		//debug("Size: " + QString::number(m_reqClassListThreaded.size()),DEBUG_VERY_VERBOSE);
		for (int i=0;i<m_reqClassListThreaded.count();i++)
		{
			if (!m_obdConnected && m_reqClassListThreaded[i].type != CONNECT)
			{
				//Not connected, no connect.
				continue;
			}
			if (!m_threadRunning)
			{
				break;
			}
			//Handle request here
			if (m_reqClassListThreaded[i].type == CONNECT)
			{
				bool init = true;
				if (m_reqClassListThreaded[i].custom.size() > 0)
				{
					if (m_reqClassListThreaded[i].custom[0] == '0')
					{
						init = false;
					}
					else if (m_reqClassListThreaded[i].custom[0] == '1')
					{
						init = true;
					}
				}
				if (!m_connect(init))
				{
					debug("Unable to connect",obdLib::DEBUG_ERROR);
					//qDebug() << "Unable to connect";
					//emit liberror(UNABLE_TO_OPEN_COM_PORT);
					continue;
				}
				m_obdConnected = true;
			}
			else if (m_reqClassListThreaded[i].type == DISCONNECT)
			{
				qDebug() << "Disconnected from ELM";
				m_obd->closePort();
				threadLockMutex.unlock();

				for (int j=0;j<m_reqClassListThreaded.count();j++)
				{
					m_reqClassListThreaded.clear();
				}
				threadLockMutex.unlock();
				m_obdConnected = false;
				emit disconnected();

			}
			else if (m_reqClassListThreaded[i].type == ELM_COMMAND)
			{
				std::vector<unsigned char> replyVector;
				QString reply="";
				m_obd->sendObdRequestString(m_reqClassListThreaded[i].custom,m_reqClassListThreaded[i].custom.length(),&replyVector,20,3);
				reply = "";
				for (unsigned int j=0;j<replyVector.size();j++)
				{
					reply += replyVector[j];
				}
				if (reply.contains("OK"))
				{
					//Command successful
				}
				else
				{
					//Command failed here
					emit elmCommandFailed(m_reqClassListThreaded[i].custom);
				}
				//return false;
			}
			else if (m_reqClassListThreaded[i].type == START_MONITOR)
			{
				m_obd->sendObdRequest("ATD0\r",5,-1);
				m_obd->sendObdRequest("ATMA\r",5,-1);

				qDebug() << "START MONITOR MODE";
				while (m_monitorMode && m_threadRunning)
				{
					QByteArray str = QByteArray(m_obd->monitorModeReadLine().c_str());
					emit monitorModeLine(str);
				}
				qDebug() << "END MONITOR MODE";
				m_obd->sendObdRequest("\r",1,-1);
			}
			else if (m_reqClassListThreaded[i].type == ST_START_FILTER_MONITOR)
			{
				m_obd->sendObdRequest("STM\r",5,-1);
				while (m_monitorMode && m_threadRunning)
				{
					QByteArray str = QByteArray(m_obd->monitorModeReadLine().c_str());
					emit monitorModeLine(str);
				}
				m_obd->sendObdRequest("\r",1,-1);
			}
			else if (m_reqClassListThreaded[i].type == ST_START_MONITOR)
			{
				m_obd->sendObdRequest("STMA\r",5,-1);
				while (m_monitorMode && m_threadRunning)
				{
					QByteArray str = QByteArray(m_obd->monitorModeReadLine().c_str());
					emit monitorModeLine(str);
				}
				m_obd->sendObdRequest("\r",1,-1);
			}
			else if (m_reqClassListThreaded[i].type == STOP_MONITOR)
			{

			}
			else if (m_reqClassListThreaded[i].type == VOLTAGE)
			{
				std::vector<unsigned char> reply;
				QByteArray replyArray;
				if (!m_obd->sendObdRequestString("ATRV\r",5,&reply,100,5))
				{
					emit consoleMessage("Unable to send voltage request");
					qDebug() << "Error with voltage request";
				}
				else
				{
					for (unsigned int j=0;j<reply.size();j++)
					{
						if (reply[j] != '\r' && reply[j] != '\n' && reply[j] != ' ' && reply[j] != '>')
						{
							replyArray.append(reply[j]);
						}
					}
					//emit singleShotResponse(m_reqClassListThreaded[i].custom,replyArray);
					//emit voltage()
					//qDebug() << replyArray;
					emit voltageReply(QString(replyArray).toDouble());
				}
			}
			else if (m_reqClassListThreaded[i].type == REQ_SUPPORTED_MODES)
			{
				std::vector<unsigned char> reply;
				QByteArray replyArray;
				QList<QString> modelist;
				if (m_obd->sendObdRequestString("0500\r",5,&reply,100,5))
				{
					modelist.append("05");
				}
				if (m_obd->sendObdRequestString("0600\r",5,&reply,100,5))
				{
					modelist.append("06");
				}
				if (m_obd->sendObdRequestString("0700\r",5,&reply,100,5))
				{
					modelist.append("07");
				}
				if (m_obd->sendObdRequestString("0800\r",5,&reply,100,5))
				{
					modelist.append("08");
				}
				if (m_obd->sendObdRequestString("0900\r",5,&reply,100,5))
				{
					modelist.append("09");
				}
				for (unsigned int j=0;j<reply.size();j++)
				{
					replyArray.append(reply[j]);
				}
				//emit singleShotResponse(m_reqClassListThreaded[i].custom,replyArray);
				emit supportedModesReply(modelist);
			}
			else if (m_reqClassListThreaded[i].type == ON_BOARD_MONITORS)
			{
				QString vect = "";
				if (!m_obd->sendObdRequestString("ATDPN\r",6,&replyVector))
				{
					qDebug() << "Error retreiving protocol";
				}
				else
				{

					for (unsigned int j=0;j<replyVector.size();j++)
					{
						if (replyVector[j] != '\n' && replyVector[j] != '\r' && replyVector[j] != ' ')
						{
							vect += replyVector[j];
						}
					}
					qDebug() << "Protocol Number:" << vect;
				}
				QString protNum = vect;
				//setHeaders(true); //Neccesary for proper reading
				//headersOn();
				m_setHeader(true);
				std::vector<unsigned char> reply;
				if (vect == "A3")
				{

					QList<unsigned char> pids;
					unsigned int var = 0;

					for (int k=0;k<0xC0;k+=0x20)
					{
						if (!m_threadRunning)
						{
							break;
						}
						QString newreq = QString(QString("06") + ((k == 0) ? "00" : QString::number(k,16)) + "\r");
						qDebug() << "Requesting:" << newreq;
						if (m_obd->sendObdRequest(newreq.toStdString().c_str(),5,&reply))
						{
							var = (((unsigned int)reply[6] << 24) + ((unsigned int)reply[7] << 16) + ((unsigned int)reply[8] << 8) + (unsigned int)reply[9]);
							qDebug() << QString::number(reply[2],16) <<QString::number(reply[3],16) << QString::number(reply[4],16) << QString::number(reply[5],16);
							for (int j=1;j<0x20;j++)
							{
								if (((var >> (0x20-j)) & 1))
								{
									qDebug() << "Pid " << j << "enabled";
									pids.append(j + k);
								}
								else
								{
									qDebug() << "pid" << j << "disabled";
								}
							}

						}
					}
					for (int j=0;j<pids.size();j++)
					{
						if (!m_threadRunning)
						{
							break;
						}
						QString reqstr = (QString("06") + ((pids[j] < 16) ? "0" : "") + QString::number(pids[j],16).toUpper() + "\r");
						qDebug() << "Request:" << reqstr << QString::number(pids[j]);
						m_obd->sendObdRequestString(reqstr.toUpper().toStdString().c_str(),5,&reply,100,5);
						QString response="";
						for (unsigned int k=0;k<reply.size();k++)
						{
							response += reply[k];
						}
						//qDebug() << "Response:" << response;
						response = response.mid(9);
						QString responsesplit = response.replace("\r","").replace("\n","").replace(" ","");
						qDebug() << "Done" << responsesplit;
						if (responsesplit.size() >= 15)
						{
							unsigned char limittype = m_obd->byteArrayToByte(responsesplit[4].toAscii(),responsesplit[5].toAscii());
							//unsigned char testhigh = m_obd->byteArrayToByte(responsesplit[6].toAscii(),responsesplit[7].toAscii());
							//unsigned char testlow = m_obd->byteArrayToByte(responsesplit[8].toAscii(),responsesplit[9].toAscii());
							//unsigned char limithigh = m_obd->byteArrayToByte(responsesplit[10].toAscii(),responsesplit[11].toAscii());
							//unsigned char limitlow = m_obd->byteArrayToByte(responsesplit[12].toAscii(),responsesplit[13].toAscii());
							qDebug() << "Test:" << QString::number(limittype);
							if (((limittype >> 7) & 1) == 1)
							{
								qDebug() << "Minimum Value";
							}
							else
							{
								qDebug() << "Maximum Value";
							}
							unsigned char testid = (((limittype << 1) & 0xFF) >> 1);
							qDebug() << "TestID:" << QString::number(testid);

						}
						else
						{
							qDebug() << "REsponse too short!!!";
						}
						//5 and 6 limit type
						//7 and 8 test value high
						//9 and 10 test value low
						//11 and 12 limit value high
						//13 and 14 limit value low

					}

				}
				else
				{
					std::vector<unsigned char> reply;
					QByteArray reply00;
					QByteArray reply20;
					QByteArray reply40;
					QByteArray reply60;
					QByteArray reply80;
					QByteArray replyA0;
					QList<unsigned char> pids;
					//QByteArray pids;
					unsigned int var = 0;
					for (int k=0;k<0xC0;k+=0x20)
					{
						if (!m_threadRunning)
						{
							break;
						}
						QString newreq = QString(QString("06") + ((k == 0) ? "00" : QString::number(k,16)) + "\r");
						qDebug() << "Requesting:" << newreq;
						if (m_obd->sendObdRequest(newreq.toStdString().c_str(),5,&reply))
						{
							var = (((unsigned int)reply[2] << 24) + ((unsigned int)reply[3] << 16) + ((unsigned int)reply[4] << 8) + (unsigned int)reply[5]);
							qDebug() << QString::number(reply[2],16) <<QString::number(reply[3],16) << QString::number(reply[4],16) << QString::number(reply[5],16);
							for (int j=1;j<0x20;j++)
							{
								if (((var >> (0x20-j)) & 1))
								{
									qDebug() << "Pid " << j << "enabled";
									pids.append(j + k);
								}
								else
								{
									qDebug() << "pid" << j << "disabled";
								}
							}

						}
					}
					if (!m_obd->sendObdRequest("ATH1\r",5,&reply))
					{
						qDebug() << "Unable to enable headers";
					}
					qDebug() << "ON_BOARD MONITORS";
					qDebug() << pids.size() << "monitors enabled";
					QList<QString> minlist;
					QList<QString> maxlist;
					QList<unsigned char> tidlist;
					QList<unsigned char> midlist;
					QList<QString> vallist;
					QList<QString> passlist;
					for (int j=0;j<pids.size();j++)
					{
						if (!m_threadRunning)
						{
							break;
						}
						QString reqstr = (QString("06") + ((pids[j] < 16) ? "0" : "") + QString::number(pids[j],16).toUpper() + "\r");
						qDebug() << "Request:" << reqstr << QString::number(pids[j]);
						m_obd->sendObdRequestString(reqstr.toStdString().c_str(),5,&reply,100,5);
						QString response="";
						for (unsigned int k=0;k<reply.size();k++)
						{
							response += reply[k];
						}
						//qDebug() << "Response:" << response;
						response = response.mid(10);
						QStringList responsesplit = response.replace("\r","").replace("\n","").split("7E8");
						QString total = "";
						for (int k=0;k<responsesplit.size();k++)
						{
							if (responsesplit[k].length() > 0)
							{
								QString line = responsesplit[k];
								line = line.replace(" ","");
								line = line.mid(2);
								total += line.replace("\r","").replace("\n","");
							}
						}

						for (int k=0;k<total.length();k++)
						{
							qDebug() << total.mid(k);
							if (total.length() > k+18)
							{
								QString name = total.mid(k,4);
								k+= 4;
								QString type = total.mid(k,2);
								unsigned char obdmidchar = obdLib::byteArrayToByte(name[0].toAscii(),name[1].toAscii());
								unsigned char obdtidchar = obdLib::byteArrayToByte(name[2].toAscii(),name[3].toAscii());
								unsigned char typechar = obdLib::byteArrayToByte(type[0].toAscii(),type[1].toAscii());
								midlist.append(obdmidchar);
								tidlist.append(obdtidchar);
								k+=2;
								QString val = total.mid(k,4);
								unsigned int valb=0;
								valb += obdLib::byteArrayToByte(val[0].toAscii(),val[1].toAscii()) << 8;
								valb += obdLib::byteArrayToByte(val[2].toAscii(),val[3].toAscii());
								qDebug() << name;
								qDebug() << type;
								qDebug() << "MID:" << QString::number(obdmidchar,16);
								qDebug() << "TID:" << QString::number(obdtidchar,16);

								ObdInfo::ModeSixScalers scaler = getInfo()->getScalerFromByte(typechar);
								ObdInfo::ModeSixInfo info = getInfo()->getInfoFromByte(obdmidchar);
								ObdInfo::ModeSixInfo test = getInfo()->getTestFromByte(obdtidchar);
								k+=4;
								QString min = total.mid(k,4);
								k+=4;
								QString max = total.mid(k,4);
								k += 3;
								qDebug() << "Min:" << min;
								qDebug() << "Max:" << max;
								unsigned char maxtop = obdLib::byteArrayToByte(max[0].toAscii(),max[1].toAscii());
								unsigned char maxbot = obdLib::byteArrayToByte(max[2].toAscii(),max[3].toAscii());
								double totalmax = (maxtop << 8) + maxbot;
								unsigned char mintop = obdLib::byteArrayToByte(min[0].toAscii(),min[1].toAscii());
								unsigned char minbot = obdLib::byteArrayToByte(min[2].toAscii(),min[3].toAscii());
								double totalmin = (mintop << 8) + minbot;
								if (test.id == 0)
								{
									//MFG Test
								//	qDebug() << info.description << "MFG Test";
								}
								else
								{
								//	qDebug() << info.description << test.description;
								}
								double newval = valb;
								if (typechar >= 0x80)
								{
									//
									newval = (short)valb;
									totalmax = (short)totalmax;
									totalmin = (short)totalmin;
									newval = ((newval * scaler.multiplier) + scaler.offset);
									totalmin = (totalmin * scaler.multiplier + scaler.offset);
									totalmax = (totalmax * scaler.multiplier + scaler.offset);
								}
								else
								{
									newval = ((valb * scaler.multiplier) + scaler.offset);
									totalmax = totalmax * scaler.multiplier + scaler.offset;
									totalmin = totalmin * scaler.multiplier + scaler.offset;
								}
								qDebug() << "Scalar:" << QString::number(scaler.multiplier);
								qDebug() << "Valb" << QString::number(valb);
								qDebug() << "Newval:" << QString::number(newval);
								vallist.append(QString::number(newval));

								//qDebug() << valb << scaler.multiplier << newval << scaler.units;

								minlist.append(QString::number(totalmin));
								maxlist.append(QString::number(totalmax));
								if (newval >= totalmin && newval <= totalmax)
								{
									passlist.append("PASS");
								}
								else
								{
									passlist.append("FAIL");
								}
							}
						}
					}
					emit onBoardMonitoringReply(midlist,tidlist,vallist,minlist,maxlist,passlist);
				}
				//void onBoardMonitoringReply(QList<QString> midlist,QList<QString> tidlist,QList<QString> vallist,QList<QString> minlist,QList<QString> maxlist);
				if (!m_obd->sendObdRequest("ATH0\r",5,&reply))
				{
					qDebug() << "Unable to disable headers";
				}
			}
			else if (m_reqClassListThreaded[i].type == SWITCH_BAUD)
			{
				//Switch baud to 38400;
				qDebug() << "Switching baud...";
				m_obd->sendObdRequest("ATBRD 68\r",9);
				qDebug() << "Sent request. Closing and reopening port";
				//usleep(10000);

				m_obd->closePort();
				msleep(100);
				m_obd->openPort(m_port.toStdString().c_str(),38400);
				std::vector<unsigned char> reply;
				qDebug() << "Sending confirmation";
				m_obd->sendObdRequestString("\r",1,&reply,1,1);
				QString vect = "";
				for (unsigned int i=0;i<reply.size();i++)
				{
					vect.append(reply.at(i));
				}
				qDebug() << "Vect:" << vect;
			}
			else if (m_reqClassListThreaded[i].type == CLEAR_TROUBLE_CODES)
			{
				consoleMessage("Trouble Code Clear Requested...");
				m_obd->sendObdRequest("04\r",3);
			}
			else if (m_reqClassListThreaded[i].type == MONITOR_STATUS)
			{
				qDebug() << "MONITOR_STATUS";
				std::vector<unsigned char> replyVector;
				m_obd->sendObdRequestString("0101\r",5,&replyVector,100,5);
				QString vect2 = "";

				for (unsigned int j=0;j<replyVector.size();j++)
				{
					if (replyVector[j] != '\n' && replyVector[j] != '\r' && replyVector[j] != ' ')
					{
						vect2.append(replyVector[j]);
					}
					//vect2 += replyVector[j];
				}
				if (vect2.size() < 8)
				{
					qDebug() << "MONITOR_STATUS returned a result too short!" << vect2;
				}
				else
				{
					qDebug() << "MONITOR_STATUS size: " << vect2.size();
					//unsigned char one = m_obd->byteArrayToByte(vect2[4].toAscii(),vect2[5].toAscii());
					unsigned char two = m_obd->byteArrayToByte(vect2[6].toAscii(),vect2[7].toAscii());
					unsigned char three = m_obd->byteArrayToByte(vect2[8].toAscii(),vect2[9].toAscii());
					unsigned char four = m_obd->byteArrayToByte(vect2[10].toAscii(),vect2[11].toAscii());
					QString onestr;
					QString twostr;
					QString threestr;
					for (int j=0;j<8;j++)
					{
						if ((two >> j) & 1)
						{
							onestr += "1";
						}
						else
						{
							onestr += "0";
						}
						if ((three >> j) & 1)
						{
							twostr += "1";
						}
						else
						{
							twostr += "0";
						}
						if ((four >> j) & 1)
						{
							threestr += "1";
						}
						else
						{
							threestr += "0";
						}
					}
					qDebug() << "Monitor:" << onestr << twostr << threestr;
					qDebug() << vect2;
					qDebug() << QString::number(two) << QString::number(three) << QString::number(four);
					//QList<QString> resultlist;
					QMap<CONTINUOUS_MONITOR,MONITOR_COMPLETE_STATUS> result;
					result[MISFIRE] = (((two >> 0) & 1) ? (((two >> 4) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[FUEL_SYSTEM] = (((two >> 1) & 1) ? (((two >> 5) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[COMPONENTS] = (((two >> 2) & 1) ? (((two >> 6) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[CATALYST] = (((three >> 0) & 1) ? (((four >> 0) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[HEATED_CATALYST] = (((three >> 1) & 1) ? (((four >> 1) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[EVAPORATIVE_SYSTEM] = (((three >> 2) & 1) ? (((four >> 2) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[SECONDARY_AIR_SYSTEM] = (((three >> 3) & 1) ? (((four >> 3) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[AC_REFRIGERANT] = (((three >> 4) & 1) ? (((four >> 4) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[OXYGEN_SENSOR] = (((three >> 5) & 1) ? (((four >> 5) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[OXYGEN_SENSOR_HEATER] = (((three >> 6) & 1) ? (((four >> 6) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);
					result[EGR_SYSTEM] = (((three >> 7) & 1) ? (((four >> 7) & 1) ? INCOMPLETE : COMPLETE) : UNAVAILABLE);

							//
					//QString misfire = QString(((two >> 0) & 1) ? "1" : "0") + ":" + (((two >> 4) & 1) ? "1" : "0");
					//QString fuelsystem = QString(((two >> 1) & 1) ? "1" : "0") + ":" + (((two >> 5) & 1) ? "1" : "0");
					//QString component = QString(((two >> 2) & 1) ? "1" : "0") + ":" + (((two >> 6) & 1) ? "1" : "0");
					//	QString reserved = QString(((two >> 3) & 1) ? "1" : "0") + ":" + (((two >> 7) & 1) ? "1" : "0");
					//QString catalyst = QString(((three >> 0) & 1) ? "1" : "0") + ":" + (((four >> 0) & 1) ? "1" : "0");
					//QString heatedcat =QString (((three >> 1) & 1) ? "1" : "0") + ":" + (((four >> 1) & 1) ? "1" : "0");
					//QString evapsys = QString(((three >> 2) & 1) ? "1" : "0") + ":" + (((four >> 2) & 1) ? "1" : "0");
					//QString secondair = QString(((three >> 3) & 1) ? "1" : "0") + ":" + (((four >> 3) & 1) ? "1" : "0");
					//QString acrefrig = QString(((three >> 4) & 1) ? "1" : "0") + ":" + (((four >> 4) & 1) ? "1" : "0");
					//QString oxygensensor = QString(((three >> 5) & 1) ? "1" : "0") + ":" + (((four >> 5) & 1) ? "1" : "0");
					//QString oxygenheater = QString(((three >> 6) & 1) ? "1" : "0") + ":" + (((four >> 6) & 1) ? "1" : "0");
					//QString egrsystem = QString(((three >> 7) & 1) ? "1" : "0") + ":" + (((four >> 7) & 1) ? "1" : "0");

					/*resultlist.append(misfire);
					resultlist.append(fuelsystem);
					resultlist.append(component);
					//resultlist.append(reserved);
					resultlist.append(catalyst);
					resultlist.append(heatedcat);
					resultlist.append(evapsys);
					resultlist.append(secondair);
					resultlist.append(acrefrig);
					resultlist.append(oxygensensor);
					resultlist.append(oxygenheater);
					resultlist.append(egrsystem);*/
					emit monitorTestReply(result);

				}
			}
			else if (m_reqClassListThreaded[i].type == MFG_STRING_ONE)
			{
				std::vector<unsigned char> replyVector;
				m_obd->sendObdRequestString("AT@1\r",5,&replyVector,100,5);

				QString vect2 = "";

				for (unsigned int j=0;j<replyVector.size();j++)
				{
					if (replyVector[j] != '\n' && replyVector[j] != '\r' && replyVector[j] != ' ')
					{
						vect2.append(replyVector[j]);
					}
					//vect2 += replyVector[j];
				}
				emit mfgStringReply(vect2);
			}
			else if (m_reqClassListThreaded[i].type == TROUBLE_CODES)
			{
				consoleMessage("Trouble Codes Requested...");
				//setHeaders(true);
				//headersOn();
				m_setHeader(true);
				std::vector<unsigned char> replyVector;
				//troubleCodes(QList<QString> codes)
				QString vect = "";
				if (!m_obd->sendObdRequestString("ATDPN\r",6,&replyVector))
				{
					qDebug() << "Error retreiving protocol";
				}
				else
				{

					for (unsigned int j=0;j<replyVector.size();j++)
					{
						if (replyVector[j] != '\n' && replyVector[j] != '\r' && replyVector[j] != ' ')
						{
							vect += replyVector[j];
						}
					}
					qDebug() << "Protocol Number:" << vect;
				}
				QString protNum = vect;
				//setHeaders(true); //Neccesary for proper reading

				if (!m_setHeader(true))
				{
					qDebug() << "Error turning headers on!";
				}
				if (!m_obd->sendObdRequestString("03\r",3,&replyVector))
				{
					//qDebug() << "Error retreiving trouble codes";
					if (m_obd->lastError() == obdLib::NODATA)
					{
						//Nodata on trouble codes means there are none available to read.
						emit consoleMessage("No trouble codes");
						emit troubleCodesReply(QString(),QList<QString>());
						m_reqClassFailureMap.remove(&m_reqClassListThreaded[i]);
						m_reqClassListThreaded.removeAt(i);
						//continue;
						//qDebug() << "NODATA";
					}
					//Error
				}
				//vect.clear();
				QString vect2 = "";

				for (unsigned int j=0;j<replyVector.size();j++)
				{
					if (replyVector[j] != '\n' && replyVector[j] != '\r' && replyVector[j] != ' ')
					{
						vect.append(replyVector[j]);
					}
					if (replyVector[j] != ' ')
					{
					//qDebug() << replyVector[i];
					vect2 += replyVector[j];
					}
				}
				//qDebug() << vect2;
				//qDebug() << vect;
				QByteArray bytevect;
				for (int j=0;j<vect.size();j++)
				{
					bytevect.append(m_obd->byteArrayToByte(vect.toAscii().at(j),vect.toAscii().at(j+1)));
					//qDebug() << "Origional" << vect.toAscii().at(i) << vect.toAscii().at(i+1);
					//qDebug() << "Converted:" << m_obd->byteArrayToByte(vect.toAscii().at(i),vect.toAscii().at(i+1));
					j++;
				}
				QStringList vectsplitline = vect2.split("\r");
				QStringList ecuname;
				QStringList ecuresponse;
				for (int k=0;k<vectsplitline.size();k++)
				{
					QString full = "";
					QString name = "";
					//We want to pick up on the string as the beginning,
					//after all the header stuff. On A6 A7 and A8, this is different.
					if (protNum == "A7" || protNum == "7")
					{
						full = vectsplitline[k].mid(10);
						name = vectsplitline[k].mid(6,2);
					}
					else if (protNum == "A8" || protNum == "A6" || protNum == "8" || protNum == "6")
					{
						full = vectsplitline[k].mid(5);
						name = vectsplitline[k].mid(0,3);
					}
					else
					{
						full = vectsplitline[k].mid(6,vectsplitline[k].length()-8); //This used to be -8, why? To handle the checksum!
						full = full.replace("\n","");
						full = full.replace("\r","");
						name = vectsplitline[k].mid(4,2);
					}
					bool found = false;
					for (int l=0;l<ecuname.size();l++)
					{
						if (ecuname[l] == name)
						{
							ecuresponse[l] += full;
							found = true;
						}
					}
					if (!found)
					{
						ecuname.append(name);
						ecuresponse.append(full);
					}

				}
				for (int k=0;k<ecuresponse.length();k++)
				{
					//ecuResponseMap[ecuname[k]].append(parseCode(ecuresponse[k],protNum));
					//codes.append(parseCode(ecuresponse[k],protNum));
					QStringList reply = parseCode(ecuresponse[k],protNum);
					emit troubleCodesReply(ecuname[k],reply);
				}
				/*QString rett = "";
				for (int i=0;i<replyVector.size();i++)
				{
					rett += (char)replyVector[i] + " ";
				}
				qDebug() << rett;
				*/
				//continue;
				//qDebug() << rett;

				//qDebug() << "Done with trouble codes";
				//troubleCodesReply(codes);
			}
			else if (m_reqClassListThreaded[i].type == RAW_REQUEST)
			{
				emit consoleMessage("Raw request");
				qDebug() << "Raw Request";
				std::vector<unsigned char> reply;
				QByteArray replyArray;

				if (!m_obd->sendObdRequestString(m_reqClassListThreaded[i].custom,m_reqClassListThreaded[i].custom.length(),&reply,100,5))
				{
					emit consoleMessage("Unable to send custom request");
					qDebug() << "Error with custom request";
				}
				for (unsigned int j=0;j<reply.size();j++)
				{
					replyArray.append(reply[j]);
				}
				emit singleShotReply(m_reqClassListThreaded[i].custom,replyArray);
			}
			else if (m_reqClassListThreaded[i].type == RAW_BLIND_REQUEST)
			{
				emit consoleMessage("Raw blind request");
				qDebug() << "Raw blind Request";
				std::vector<unsigned char> reply;
				QByteArray replyArray;

				if (!m_obd->sendObdRequestString(m_reqClassListThreaded[i].custom,m_reqClassListThreaded[i].custom.length(),&reply,0,0))
				{
					emit consoleMessage("Unable to send custom blind request");
					qDebug() << "Error with custom blind request";
				}
			}
			else if (m_reqClassListThreaded[i].type == START_REQ_LOOP)
			{

			}
			else if (m_reqClassListThreaded[i].type == STOP_REQ_LOOP)
			{

			}
			else if (m_reqClassListThreaded[i].type == SCAN_ALL)
			{
				//Full scan for supported pids
				qDebug() << "Beginning full pid scan";
				QString pid = "";
				std::vector<unsigned char> reply;
				QList<QString> pidList;
				for (int j=1;j<=9;j++)
				{
					//pidList.append(QList<int>());
					if (j != 3 && j != 4 && j != 2)
					{
						for (int k=0;k<0x20;k++)
						{
							pid = QString("0").append(QString::number(j)).append(((j < 16) ? "0" : "")).append(QString::number(k,16));
							if (!m_obd->sendObdRequest(QString(pid + "\r").toStdString().c_str(),pid.length()+1,&reply))
							{
								if (m_obd->lastError() == obdLib::NODATA)
								{
									//pidList[i-1].append(0);
								}
								else
								{
									//pidList[i-1].append(0);

								}
							}
							else
							{
								//pidList[i-1].append(1);
								pidList.append(pid);
							}
							msleep(100);
						}
					}
				}
				supportedPidsReply(pidList);
			}
			else if (m_reqClassListThreaded[i].type == FIND_PORT)
			{
				qDebug() << "Finding OBD port...";
				QStringList portList;
				portList << "/dev/ttyS0";
				portList << "/dev/ttyS1";
				portList << "/dev/ttyUSB0";
				portList << "/dev/ttyUSB1";
				portList << "/dev/ttyUSB2";
				portList << "/dev/ttyUSB3";
				portList << "/dev/ttyUSB4";
				//portList << "/dev/pts/0";
				//portList << "/dev/pts/1";
				//portList << "/dev/pts/2";
				//portList << "/dev/pts/3";
				QList<int> baudList;
				baudList << -1;
				baudList << 9600;
				baudList << 38400;
				if (!m_obdConnected)
				{
					bool found = false;
					QString foundport = "";
					for (int j=0;j<baudList.count();j++)
					{
						for (int k=0;k<portList.count();k++)
						{
							if (baudList[j] != -1)
							{
								m_obd->openPort(portList[k].toStdString().c_str(),baudList[j]);
							}
							else
							{
								m_obd->openPort(portList[k].toStdString().c_str());
							}
							if (!m_initElm())
							{
								qDebug() << "Error";
								m_obd->closePort();
							}
							else
							{
								qDebug() << "Port Found!" << portList[k];
								m_obd->closePort();
								foundport = portList[k];
								k = portList.count();
								found = true;
							}
						}
					}
					if (found)
					{
						emit obdPortFound(foundport);
					}
				}

			}
			else if (m_reqClassListThreaded[i].type == REQ_SUPPORTED_PIDS)
			{
				emit consoleMessage("Requesting supported pids");

				std::vector<unsigned char> replyVector;


				if (!m_obd->sendObdRequest("0100\r",5,&replyVector))
				{
					qDebug() << "Error in requesting supported pids 0-20";
					continue;
				}
				QString rep = "Reply: ";
				for (unsigned int j=0;j<replyVector.size();j++)
				{
					rep += QString::number(replyVector[j],16).toUpper() + " ";
				}
				//qDebug() << "String reply:" << rep;
				unsigned var = (((unsigned int)replyVector[2] << 24) + ((unsigned int)replyVector[3] << 16) + ((unsigned int)replyVector[4] << 8) + (unsigned int)replyVector[5]);
				//qDebug() << "00 PID Reply:" << var;
				for (int j=2;j<6;j++)
				{
					//qDebug() << replyVector[i];
				}
				QList<QString> pids;
				for (int j=1;j<0x20;j++)
				{
					if (((var >> (0x20-j)) & 1))
					{
						//qDebug() << "Pid " << i << "Supported";
						pids.append(QString("01").append((j < 16) ? "0" : "").append(QString::number(j,16).toUpper()));
						//qDebug() << "Pid" << pids[pids.count()-1] << "supported";
						//pids[0].append(1);
					}
					else
					{
						//pids[0].append(0);
					}
				}


				if (!m_obd->sendObdRequest("0120\r",5,&replyVector))
				{
					qDebug() << "Error in requesting supported pids 20-24";
					//continue;
				}
				else
				{
					rep = "Reply: ";
					for (unsigned int j=0;j<replyVector.size();j++)
					{
						rep += QString::number(replyVector[j],16).toUpper() + " ";
					}
					//qDebug() << "String reply:" << rep;
					var = (((unsigned int)replyVector[2] << 24) + ((unsigned int)replyVector[3] << 16) + ((unsigned int)replyVector[4] << 8) + (unsigned int)replyVector[5]);
					//qDebug() << "00 PID Reply:" << var;
					for (int j=2;j<6;j++)
					{
						//qDebug() << replyVector[i];
					}
					//QList<QString> pids;
					for (int j=1;j<0x20;j++)
					{
						if (((var >> (0x20-j)) & 1))
						{
							//qDebug() << "Pid " << i << "Supported";
							pids.append(QString("01").append(QString::number(j+(0x20),16).toUpper()));
							//qDebug() << "Pid" << pids[pids.count()-1] << "supported";
							//pids[0].append(1);
						}
						else
						{
							//pids[0].append(0);
						}
					}
				}
				if (!m_obd->sendObdRequest("0140\r",5,&replyVector))
				{
					qDebug() << "Error in requesting supported pids 40-60";

				}
				else
				{
					rep = "Reply: ";
					for (unsigned int j=0;j<replyVector.size();j++)
					{
						rep += QString::number(replyVector[j],16).toUpper() + " ";
					}
					//qDebug() << "String reply:" << rep;
					var = (((unsigned int)replyVector[2] << 24) + ((unsigned int)replyVector[3] << 16) + ((unsigned int)replyVector[4] << 8) + (unsigned int)replyVector[5]);
					//qDebug() << "00 PID Reply:" << var;
					for (int j=2;j<6;j++)
					{
						//qDebug() << replyVector[i];
					}
					for (int j=1;j<0x20;j++)
					{
						if (((var >> (0x20-j)) & 1))
						{
							//qDebug() << "Pid " << i << "Supported";
							pids.append(QString("01").append(QString::number(j+0x40,16).toUpper()));
							//qDebug() << "Pid" << pids[pids.count()-1] << "supported";
							//pids[0].append(1);
						}
						else
						{
							//pids[0].append(0);
						}
					}
				}


				emit supportedPidsReply(pids);
			}
			else if (m_reqClassListThreaded[i].type == MODE_PID)
			{
				//qDebug() << "MODE_PID" << m_obdConnected;
				if (m_obdConnected)
				{
					if (first)
					{
						first = false;
						cycleCounter++;
					}
					if (((cycleCounter % m_reqClassListThreaded[i].priority) == 0) || cycleCounter == 0)
					{
						//qDebug() << "Making req" << m_reqList[currentReqNum].mid(0,m_reqList[currentReqNum].length()-1) << m_set << m_reqPriority[currentReqNum] << currentReqNum;

						//if (!m_obd->sendObdRequest(m_reqList[currentReqNum].toStdString().c_str(),m_reqList[currentReqNum].length(),&replyVector))
						QString req = (m_reqClassListThreaded[i].mode < 16) ? "0" : "";
						req += QString::number(m_reqClassListThreaded[i].mode,16).toUpper();
						req += (m_reqClassListThreaded[i].pid < 16) ? "0" : "";
						req += QString::number(m_reqClassListThreaded[i].pid,16).toUpper();
						//qDebug() << "Current request:" << req;
						QString pidreq = req;
						//qDebug() << "sending req" << req;
						if (m_reqClassListThreaded[i].wait != 0)
						{
							req += " " + QString::number(m_reqClassListThreaded[i].wait);
						}
						req += "\r";
						if (!m_obd->sendObdRequest(req.toStdString().c_str(),req.length(),&replyVectorString))
						{
							if (m_obd->lastError() == obdLib::NODATA)
							{
								qDebug() << "OBD Request failed: NODATA for request" << req.mid(0,req.length()-1) << m_reqClassFailureMap[&m_reqClassListThreaded[i]];

								m_reqClassFailureMap[&m_reqClassListThreaded[i]]++;
								bool allislost=true;
								foreach(RequestClass* req,m_reqClassFailureMap.keys())
								{
									allislost &= m_reqClassFailureMap[req] > 0;
								}


								if (!allislost && !m_whiteList[&m_reqClassListThreaded[i]] && m_reqClassFailureMap[&m_reqClassListThreaded[i]] > 20)
								{
									//qDebug() << "Reached Maximum errors for pid. Disabling: " << req.mid(0,req.length()-1);
									debug("Reached maximum errors for pid. Disabling: " + req.mid(0,req.length()-1),obdLib::DEBUG_WARN);
									emit consoleMessage(QString("Reached maximum errors for pid ") + req.mid(0,req.length()-1) + QString(". Removing from request list."));
									m_reqClassFailureMap.remove(&m_reqClassListThreaded[i]);
									m_reqClassListThreaded.removeAt(i);
								}
								//m_errorCount[currentReqNum]++;
								//if (m_errorCount[currentReqNum] > 20)
								//{
								//	qDebug() << "Reached Maximum errors for pid" << m_reqList[currentReqNum].mid(0,m_reqList[currentReqNum].length()-1) << "shutting down requests for that pid";
								//	m_reqList.removeAt(currentReqNum);
								//	m_reqPriority.removeAt(currentReqNum);
								//	m_errorCount.removeAt(currentReqNum);
								//	emit consoleMessage(QString("Readed maximum errors for pid") + m_reqList[currentReqNum].mid(0,m_reqList[currentReqNum].length()-1) + "shutting down requests for that pid");
								//}

							}
							else if (m_obd->lastError() == obdLib::SERIALREADERROR || m_obd->lastError() == obdLib::SERIALWRITEERROR)
							{
								//errorCount++;
								//qDebug() << "Serial read/write error on request" << req.mid(0,req.length()-1);
								debug("Serial read/write error on request " + req.mid(0,req.length()-1),obdLib::DEBUG_FATAL);
								emit consoleMessage("Serial read/write error on request" + req.mid(0,req.length()-1));


								//m_requestLoopRunning = false;

							}
							//Error here
						}
						else
						{
							//emit consoleMessage("Error in recieve!" + QString::number(m_obd->lastError()));
							m_reqClassFailureMap[&m_reqClassListThreaded[i]] = 0;
							m_whiteList[&m_reqClassListThreaded[i]] = true;
							//errorCount = 0;
							/*replyVector.clear();
							for (unsigned int i=0;i<replyVectorString.size()-1;i++)
							{
								replyVector.push_back(m_obd->byteArrayToByte(replyVectorString[i],replyVectorString[i+1]));
								i++;
							}
							*/
							ObdInfo::Pid *pid = m_obdInfo->getPidFromString(pidreq);
							if (pid)
							{
								/*QString rrtest="";
								for (int i=0;i<replyVectorString.size();i++)
								{
									rrtest += QString::number(replyVectorString.at(i),16).toUpper() + " ";
								}
								qDebug() << rrtest;
								*/
								QString func = pid->function;
								if (func.contains("A"))
								{
									func = func.replace(QString("A"),QString::number(replyVectorString[2]));
								}
								if (func.contains("B"))
								{
									func = func.replace(QString("B"),QString::number(replyVectorString[3]));
								}
								QString done = parse(func);
								//qDebug() <<"firing pid: "<<pidreq<<" val: "<<done;
								pidReply(pidreq,done,cycleCounter,(((float)QDateTime::currentDateTime().toTime_t()) + ((float)QDateTime::currentDateTime().time().msec() / 1000.0)));
							}
							else
							{
								//qDebug() << "Invalid Pid returned:" << req.mid(0,req.length()-1);
								debug("Invalid PID returned from ObdInfo::getPidFromString()!!!",obdLib::DEBUG_ERROR);
								emit consoleMessage(QString("Invalid PID returned from ObdInfo::getPidFromString(): ") + req.mid(0,req.length()-1));
							}
						}

					}
				}
			}
			if (!m_requestLoopRunning)
			{
				m_requestLoopRunning = true;
				i = m_reqClassListThreaded.count();
				continue;
			}
		}
		removePidMutex.unlock();
		if (m_reqClassListThreaded.size() == 0 || !m_obdConnected)
		{
			/*debug("Loop complete, but nothing to do!",obdLib::DEBUG_VERY_VERBOSE);
			debug("List size: " + QString::number(m_reqClassListThreaded.size()),obdLib::DEBUG_VERY_VERBOSE);
			debug("m_obdConnected: " + (m_obdConnected ? "true" : "false"),obdLib::DEBUG_VERY_VERBOSE);*/
			msleep(200);
		}
	}
	if (m_obdConnected)
	{
		m_obd->closePort();
	}
}


void ObdThread::setPort(QString port)
{
	m_port = port;
}
void ObdThread::setBaud(int baud)
{
	m_baud = baud;
}
void ObdThread::addRequest(int mode, int pid, int priority,int wait)
{
	debug("Adding: " + QString::number(mode,16) + ":" + QString::number(pid,16),obdLib::DEBUG_VERBOSE);
	threadLockMutex.lock();
	RequestClass req;
	req.mode = mode;
	req.pid = pid;
	req.priority = priority;
	req.repeat = true;
	req.type = MODE_PID;
	req.wait = wait;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

void ObdThread::sendReqSupportedPids()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = REQ_SUPPORTED_PIDS;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendReqFullPidScan()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = SCAN_ALL;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}
void ObdThread::sendClearTroubleCodes()
{
	threadLockMutex.lock();
	RequestClass req;
	req.type = CLEAR_TROUBLE_CODES;
	m_reqClassList.append(req);
	threadLockMutex.unlock();
}

QString ObdThread::parse(QString str)
{
	if ((str[0] != '(' && str[str.length()-1] != ')'))
	{
		str = QString("(") + str + ")";
	}
	QString done = calc(str);
	//qDebug() << "Final Answer:" << done;
	return done;
}
bool ObdThread::m_setHeader(bool on)
{
	std::vector<unsigned char> replyVector;
	QString reply="";
	if (on)
	{
		if (!m_obd->sendObdRequestString("ath1\r",5,&replyVector,20,3))
		{
			return false;
		}
	}
	else
	{
		if (!m_obd->sendObdRequestString("ath0\r",5,&replyVector,20,3))
		{
			return false;
		}
	}
	reply = "";
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	if (reply.contains("OK"))
	{
		return true;
	}
	return false;
}

bool ObdThread::m_setEcho(bool on)
{
	std::vector<unsigned char> replyVector;
	QString reply="";
	if (on)
	{
		if (!m_obd->sendObdRequestString("ate1\r",5,&replyVector,20,3))
		{
			return false;
		}
	}
	else
	{
		if (!m_obd->sendObdRequestString("ate0\r",5,&replyVector,20,3))
		{
			return false;
		}
	}
	reply = "";
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	if (reply.contains("OK"))
	{
		return true;
	}
	debug("Bad return when turning echo off :" + reply,obdLib::DEBUG_WARN);
	//qDebug() << "Bad Echo:" << reply;
	return false;

}

bool ObdThread::m_setLineFeed(bool on)
{
	std::vector<unsigned char> replyVector;
	QString reply="";
	if (on)
	{
		if (!m_obd->sendObdRequestString("atl1\r",5,&replyVector,20,3))
		{
			return false;
		}
	}
	else
	{
		if (!m_obd->sendObdRequestString("atl0\r",5,&replyVector,20,3))
		{
			return false;
		}
	}
	reply = "";
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	if (reply.contains("OK"))
	{
		return true;
	}
	return false;

}

bool ObdThread::m_initElm()
{
	//This tries to connect twice. The first time without a reset, the second time with a reset before all inits.
	std::vector<unsigned char> replyVector;
	for (int i=0;i<2;i++)
	{
		//These are to fix issues with certain devices like obdpro's tool.
		if (!m_obd->sendObdRequestString(CR,1,&replyVector,100,5)) {}
		if (!m_obd->sendObdRequestString(CR,1,&replyVector,100,5)) {}

		if (i == 1)
		{
			if (!m_resetElm())
			{
				emit consoleMessage("Error resetting ELM Device");
				//qDebug() << "Error resetting ELM device";
				debug("Error resetting ELM Device",obdLib::DEBUG_ERROR);
				if (i == 1) return false;
			}
			m_setProtocol(0,false);
		}
		//Ensure echo, headers, and linefeeds are off.
		if (!m_setEcho(false))
		{
			emit consoleMessage("Error turning echo off");
			//qDebug() << "Error turning echo off";
			debug("Error turning off echo",obdLib::DEBUG_ERROR);
			if (i == 1) return false;
			continue;
		}
		if (!m_setHeader(false))
		{
			emit consoleMessage("Error turning headers off");
			//qDebug() << "Error turning headers off";
			debug("Error turning off headers",obdLib::DEBUG_ERROR);
			if (i == 1) return false;
			continue;
		}
		if (!m_setLineFeed(false))
		{
			emit consoleMessage("Error turning linefeeds off");
			//qDebug() << "Error turning linefeeds off";
			debug("Error turning linefeeds off",obdLib::DEBUG_ERROR);
			if (i == 1) return false;
			continue;
		}

		if (!m_obd->sendObdRequestString("0100\r",5,&replyVector,100,10))
		{
			emit consoleMessage("Error in detecting protocol");
			//qDebug() << "Error in finding protocol";
			debug("Error finding protocol with 0100",obdLib::DEBUG_ERROR);
			if (i == 1) return false;
			continue;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool ObdThread::m_resetElm()
{
	std::vector<unsigned char> replyVector;
	QString reply="";
	reply = "";
	replyVector.clear();
	if (!m_obd->sendObdRequestString("atz" CR,4,&replyVector,100,5))
	{
		//qDebug() << "Error in reset sent";
		debug("Error when sending reset command",obdLib::DEBUG_WARN);
	}
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	//qDebug() << "Reset Reply:" << reply;
	if (reply.contains("ELM"))
	{
		return true;
	}
	else if (reply.contains("atz"))
	{
		//Echoing here?
		//qDebug() << "Reset echoed:" << reply;
		debug("Reset echoed: " + reply,obdLib::DEBUG_VERBOSE);
		m_obd->flush();
		usleep(10000);
		return true;
	}
	else
	{
		//qDebug() << "Reset:" << reply;
		debug("Bad return when resetting :" + reply,obdLib::DEBUG_WARN);
		return false;
	}
}
QString ObdThread::m_getElmVersion()
{
	std::vector<unsigned char> replyVector;
	QString reply="";
	m_obd->sendObdRequestString("ati\r",4,&replyVector,20,3);
	reply = "";
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	//qDebug() << "Reply String:" << reply.replace("ati","");
	}
	return reply;
}
QString ObdThread::calc(QString str)
{
	//This is where the magic happens...
	//qDebug() << str;
	int start=-1;
	int stop=-1;
	for (int i=0;i<str.length();i++)
	{
		if (str[i] == '(')
		{
			if (start == -1)
				start = i+1;
		}
		if (str[i] == ')')
		{
				stop = i;
			//i = str.length();
		}
	}
	if (start != -1 && stop != -1)
	{
		QString tmp = calc(str.mid(start,stop-start));
		//qDebug() << "Old:" << str;
		str.replace(start-1,(stop-start)+2,tmp);
		//qDebug() << "New:" << str;
	}
	else
	{
		//qDebug() << "Inner function:" << str;
	}
	start = 0;
	stop = -1;
	//int op = 0;
	//int current;
	//"5+3*2+2-5*4/8;
	//5 3*2 2-5*4/8   + split
	//2 5*4/8     - split
	//5*4 8    / split
	//5 4   * split

	//"5+3*2+2-5*4/8;
	//5+3*2+2 5*4/8  - split
	//5 3*2 2-5*4/8   + split
	//5+3 2+2-5 4/8  * split

	for (int k=0;k<2;k++)
	{
	for (int i=0;i<str.length();i++)
	{
		if (k == 1)
		{
		if (str[i] == '+')
		{
			stop = -1;
			start = i;
			for (int j=i-1;j>=0;j--)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j=0;
				}
			}
			if (stop == -1)
			{
				stop = 0;
			}
			int replacer = stop;
			QString tmp = str.mid(stop,start-stop);
			//qDebug() << "Found:" <<i<<stop<< tmp;
			stop = -1;
			for (int j=i+1;j<str.length();j++)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j = str.length();
				}
			}
			if (stop == -1)
			{
				stop = str.length();
			}
			QString tmp2 = str.mid(start+1,stop-(start+1));
			//qDebug() << "Found2:" <<start<<stop<< tmp2;
			float first = tmp.toFloat();
			float second = tmp2.toFloat();
			str = str.replace(replacer,tmp.length()+tmp2.length()+1,QString::number(first+second));
			//qDebug() << str;
			i = 0;
		}
		if (str[i] == '-')
		{
			stop = -1;
			start = i;
			for (int j=i-1;j>=0;j--)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j=0;
				}
			}
			if (stop == -1)
			{
				stop = 0;
			}
			int replacer = stop;
			QString tmp = str.mid(stop,start-stop);
			//qDebug() << "Found:" <<i<<stop<< tmp;
			stop = -1;
			for (int j=i+1;j<str.length();j++)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j = str.length();
				}
			}
			if (stop == -1)
			{
				stop = str.length();
			}
			QString tmp2 = str.mid(start+1,stop-(start+1));
			//qDebug() << "Found2:" << tmp2;
			float first = tmp.toFloat();
			float second = tmp2.toFloat();
			str = str.replace(replacer,tmp.length()+tmp2.length()+1,QString::number(first-second));
			//qDebug() << str;
			i = 0;
		}
		}
		if (k == 0)
		{
		if (str[i] == '*')
		{
			stop = -1;
			start = i;
			for (int j=i-1;j>=0;j--)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j=0;
				}
			}
			if (stop == -1)
			{
				stop = 0;
			}
			int replacer = stop;
			QString tmp = str.mid(stop,start-stop);
			//qDebug() << "Found:" << tmp;
			stop = -1;
			for (int j=i+1;j<str.length();j++)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j = str.length();
				}
			}
			if (stop == -1)
			{

				stop = str.length();
			}
			QString tmp2 = str.mid(start+1,stop-(start+1));
			//qDebug() << "Found2:" << tmp2;
			float first = tmp.toFloat();
			float second = tmp2.toFloat();
			str = str.replace(replacer,tmp.length()+tmp2.length()+1,QString::number(first*second));
			//qDebug() << str;
			i = 0;
		}
		if (str[i] == '/')
		{
			stop = -1;
			start = i;
			for (int j=i-1;j>=0;j--)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j=0;
				}
			}
			if (stop == -1)
			{
				stop = 0;
			}
			int replacer = stop;
			QString tmp = str.mid(stop,start-stop);
			//qDebug() << "Found:" << tmp;
			stop = -1;
			for (int j=i+1;j<str.length();j++)
			{
				if (str[j] == '+' || str[j] == '-' || str[j] == '/' || str[j] == '*')
				{
					stop = j;
					j = str.length();
				}
			}
			if (stop == -1)
			{
				stop = str.length();
			}
			QString tmp2 = str.mid(start+1,stop-(start+1));
			//qDebug() << "Found2:" << tmp2;
			float first = tmp.toFloat();
			float second = tmp2.toFloat();
			str = str.replace(replacer,tmp.length()+tmp2.length()+1,QString::number(first/second));
			//qDebug() << str;
			i = 0;
		}
		}
		//usleep(100000);
	}
	}
	return str;
}
bool ObdThread::m_connect(bool init)
{
	//qDebug() << "Connecting...";
	debug("ObdThread::m_connect()",obdLib::DEBUG_VERBOSE);
	if (m_obd->openPort(m_port.toStdString().c_str(),m_baud) < 0)
	{
		//qDebug() << "Error opening OBD Port";
		debug("Error opening OBD port",obdLib::DEBUG_ERROR);
		emit liberror(ObdThread::UNABLE_TO_OPEN_COM_PORT);
		return false;
	}
	if (init)
	{
		if (!m_initElm())
		{
			debug("Error in ELM init",obdLib::DEBUG_ERROR);
			emit consoleMessage("Error in ELM init, port not opened");
			m_obd->closePort();
			//emit disconnected();
			//continue;
			return false;
		}
	}
	//m_obdConnected = true;
	QString version = m_getElmVersion().replace("\r","").replace("\n","");
	emit consoleMessage(QString("Elm found. Version: ").append(version));
	//qDebug() << "Connected to ELM version" << version;
	debug("Connected to ELM " + version,obdLib::DEBUG_INFO);
	//setProtocol(0,false);
	QString protocol = m_getProtocolName().replace("\r","").replace("\n","");
	//qDebug() << "Connected protocol:" << protocol;
	debug("Protocol " + protocol,obdLib::DEBUG_INFO);
	emit connected(version);
	emit protocolReply(protocol);
	return true;
}
QString ObdThread::m_getProtocolName()
{
	std::vector<unsigned char> replyVector;
	QString reply="";
	//m_obd->sendObdRequestString("0100\r",5,&replyVector,20,5);
	m_obd->sendObdRequestString("ATDP\r",5,&replyVector,100);
	reply = "";
	for (unsigned int i=0;i<replyVector.size();i++)
	{
		reply += replyVector[i];
	}
	//qDebug() << "Reply String:" << reply.replace("ati","");
	return reply;

}
void ObdThread::m_setProtocol(int num, bool autosearch)
{
	Q_UNUSED(num);
	Q_UNUSED(autosearch);
	QString req = QString("ATSP") + (autosearch ? "A" : "") + QString::number(num) + "\r";
	if (!m_obd->sendObdRequest(req.toStdString().c_str(),req.length(),20))
	{
		qDebug() << "Error setting protocol";
	}
}
