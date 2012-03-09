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

#include "gauges.h"

BaseGauge::BaseGauge(QWidget *parent) : QwtDial(parent)
{
	m_label = "";
	
	// setup parameters used by most gauges
	setScaleComponents(QwtAbstractScaleDraw::Ticks | QwtAbstractScaleDraw::Labels);
	setScaleTicks(0, 8, 16);
	setScaleArc(0.0, 215.0);
	scaleDraw()->setSpacing(8);
	setWrapping(false);
	setReadOnly(true);
	setOrigin(160.0);
	
	// setup basic needle
	needle = new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow, true, Qt::red, Qt::gray);
	setNeedle(needle);
}

void BaseGauge::setLabel(const QString &label)
{
	m_label = label;
	update();
}

QString BaseGauge::label() const
{
	return m_label;
}

void BaseGauge::drawScaleContents(QPainter *painter, const QPointF &center, double radius) const
{
	// create rectangle to hold the text inside of and move it to the center
	QRectF rect(0.0, 0.0, 2.0 * radius, 2.0 * radius - 10.0);
	rect.moveCenter(center);
	
	// give the painter a 'text' pen and draw the tex to the screen
	painter->setPen(palette().color(QPalette::Text));	
	painter->drawText(rect, Qt::AlignBottom | Qt::AlignHCenter, m_label);
}


Speedometer::Speedometer(QWidget *parent) : BaseGauge(parent)
{
	// setup parameters for a basic speedometer
	setRange(0, 160);
	setScale(-1, 2, 10);
	setLabel("MPH");
}


Tachometer::Tachometer(QWidget *parent) : BaseGauge(parent)
{
	// setup parameters for a basic tachometer
	setRange(0, 10000);
	setScale(-1, 2, 1000.0);
	setLabel("RPM\nx1000");
}
