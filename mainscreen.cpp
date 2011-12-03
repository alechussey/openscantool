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

#include "mainscreen.h"

#define BUTTON_WIDTH  250
#define BUTTON_HEIGHT 100

MainScreen::MainScreen(QWidget *parent) :
    QWidget(parent)
{
	// create the four big buttons for each task you can perform
	dashButton = new QPushButton(tr("Dashboard"));
	dashButton->setMinimumSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	
	tripButton = new QPushButton(tr("Trip Computer"));
	tripButton->setMinimumSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	
	diagButton = new QPushButton(tr("Diagnostics"));
	diagButton->setMinimumSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	
	logButton = new QPushButton(tr("Logging"));
	logButton->setMinimumSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	
	// create grid layout for buttons
	mainLayout = new QGridLayout();
	mainLayout->setAlignment(Qt::AlignCenter);
	mainLayout->setSpacing(15);
	mainLayout->addWidget(dashButton, 0, 0);
	mainLayout->addWidget(tripButton, 0, 1);
	mainLayout->addWidget(diagButton, 1, 0);
	mainLayout->addWidget(logButton, 1, 1);
	
	// set this widgets layout as our button grid layout
	setLayout(mainLayout);
}
