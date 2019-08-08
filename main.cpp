/****************************************************************************
**
** This file is part of the QtOPENCASCADE Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Copyright (C) 2006, 2007, Peter Dolbey, Marc Britten, Stephane Routelous
**               Stefan Boeykens, Pawel Dobrolowski, Walter Steffe
**               ¨¢lvaro Castro Castilla, Dirk Burski. 
**
** All rights reserved.
**
****************************************************************************/
/*!
\file	main.cpp
\brief	Top leve application
*/

#include <iostream>
#include <fstream>
using namespace std;
#include "qoccapplication.h"
#include "qoccmainwindow.h"
#include "qtdotnetstyle.h"

#include <QtGui>
//#include <QPlastiqueStyle>

int main(int argc, char *argv[])
{
    QoccApplication app( argc, argv );
#if defined(Q_WS_WIN)
    QtDotNetStyle* style = new QtDotNetStyle();
    app.setStyle(style);
#endif
    QoccMainWindow *window = new QoccMainWindow();
    window->showMaximized();

    int retval = app.exec();
    return retval;
}
