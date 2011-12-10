/*
 *  OpenScanTool
 *  Copyright (C) 2011  Alec Hussey
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

#ifndef GAUGES_H
#define GAUGES_H

#include <QtGui>
#include <qwt_dial.h>
#include <qwt_dial_needle.h>

class BaseGauge : public QwtDial
{
public:
	explicit BaseGauge(QWidget *parent = 0);
	
	void setLabel(const QString &label);
	QString label(void) const;

protected:
	virtual void drawScaleContents(QPainter *painter, const QPointF &center, double radius) const;

private:
	QString m_label;
	QwtDialSimpleNeedle *needle;
};

class Speedometer : public BaseGauge
{
public:
	explicit Speedometer(QWidget *parent = 0);
};

#endif // GAUGES_H
