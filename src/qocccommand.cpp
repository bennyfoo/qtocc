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
** Copyright (C) 2006, 2007, Peter Dolbey.
**
** All rights reserved.
**
****************************************************************************/
/*!
\class	QoccCommand
\brief	This class provides a base object for commands
\author	Peter C. Dolbey
*/

#include <qnamespace.h>
#include "qoccinternal.h"
#include "qocccommand.h"

QoccCommand::QoccCommand( QAction* startingAction, QWidget* /* parent */ )
{
	myAction = startingAction;
	myIsDrawing = false;
}

QoccCommand::~QoccCommand( )
{

}




