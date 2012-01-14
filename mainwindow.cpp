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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	// initialize all screens
	main = new MainScreen(this);
	connect(main, SIGNAL(dashTriggered()), this, SLOT(dashboard()));
	
	// setup main window itself
	setWindowTitle("OpenScanTool");
	setMinimumSize(800, 600);
	
	// set default screen as central widget
	setCentralWidget(main);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupActions()
{
	// file menu actions
	connectAction = new QAction(tr("Connect to vehicle"), this);
	connectAction->setToolTip(tr("Establish a connection to your vehicle."));
	connectAction->setShortcut();
	
	openAction = new QAction(tr("Open data log"), this);
	openAction->setToolTip(tr("Open a previously saved data log file."));
}

void MainWindow::setupMenus()
{
	//
}

void MainWindow::dashboard()
{
	dash = new Dashboard(this);
	setCentralWidget(dash);
}
