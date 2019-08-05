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
\class	QoccMakePointDlg
\brief	This class provides the keyboard entry for the circle-centre command
\author	Peter C. Dolbey
*/
#include <QAction>

#include <qnamespace.h>
#include "qoccinternal.h"
#include "qoccviewwidget.h"
#include "qoccmakepoint.h"

QoccPointDlg::QoccPointDlg( )
{

}

QoccPointDlg::~QoccPointDlg( )
{
}

/*!
\class	QoccMakePoint
\brief	This class provides the interactive entry for the circle-centre command
\author	Peter C. Dolbey
*/

QoccMakePoint::QoccMakePoint( QAction* startingAction, QWidget* parent )
:QoccCommand( startingAction, parent )
{
	if ( myAction )
	{
		myAction->setDisabled(true);
	}
}

QoccMakePoint::~QoccMakePoint( )
{
	if ( myAction )
	{
		myAction->setDisabled(false);
	}
}

void QoccMakePoint::moveEvent( QoccViewWidget* )
{

}

void QoccMakePoint::clickEvent( QoccViewWidget* widget )
{
	gp_Pnt p(widget->x(), widget->y(), widget->z());
	TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex( p );
	Handle(AIS_Shape) aisVertex = new AIS_Shape(aVertex);
	// context is the handle to an AIS_InteractiveContext object.
	widget->getContext()->Display(aisVertex,Standard_True); 
}

