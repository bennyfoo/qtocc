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
\class	QoccCircleCentreDlg
\brief	This class provides the keyboard entry for the circle-centre command
\author	Peter C. Dolbey
*/
#include <QAction>

#include <qnamespace.h>
#include "qoccinternal.h"
#include "qoccviewwidget.h"
#include "qoccmakecirclecentre.h"

QoccCircleCentreDlg::QoccCircleCentreDlg( )
{

}

QoccCircleCentreDlg::~QoccCircleCentreDlg( )
{
}

/*!
\class	QoccCircleCentre
\brief	This class provides the interactive entry for the circle-centre command
\author	Peter C. Dolbey
*/

QoccMakeCircleCentre::QoccMakeCircleCentre( QAction* startingAction, QWidget* parent )
:QoccCommand( startingAction, parent )
{
	parameter = Centre;
	if ( myAction )
	{
		myAction->setDisabled(true);
	}
}

QoccMakeCircleCentre::~QoccMakeCircleCentre( )
{
	if ( myAction )
	{
		myAction->setDisabled(false);
	}
}

TopoDS_Shape QoccMakeCircleCentre::buildCircle( QoccViewWidget* widget )
{
	gp_Pnt p(widget->x(), widget->y(), widget->z());
	dRadius = p.Distance(ptCentre);
	gp_Ax2 aix(ptCentre, dirNormal);
	gp_Circ circle(aix, dRadius);
	BRepBuilderAPI_MakeEdge builder(circle);
	TopoDS_Shape shape = builder.Shape();
	return shape;
}

void QoccMakeCircleCentre::moveEvent( QoccViewWidget* widget )
{
	switch (parameter)
	{
	case Centre:
		break;
	case Radius:
		{
			if (!aisCircle.IsNull())
			{
				widget->getContext()->Remove(aisCircle,Standard_True);
				aisCircle.Nullify();
			}
			aisCircle = new AIS_Shape(buildCircle( widget ));
			aisCircle->SetColor(Quantity_NOC_YELLOW);
			widget->getContext()->Display(aisCircle,Standard_True); 
			break;
		}
	}
}

void QoccMakeCircleCentre::clickEvent( QoccViewWidget* widget )
{
	switch (parameter)
	{
	case Centre:
		{
			dirNormal = widget->getDirection();

			gp_Pnt p(widget->x(), widget->y(), widget->z());
			ptCentre = p;

			TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex( ptCentre );
			aisCentre = new AIS_Shape(aVertex);
			widget->getContext()->Display(aisCentre,Standard_True); 

			myIsDrawing = true;
			parameter = Radius;
			break;
		}
	case Radius:
		{		
			if (!aisCircle.IsNull())
			{
				widget->getContext()->Remove(aisCircle,Standard_True);
				aisCircle.Nullify();
			}
			aisCircle = new AIS_Shape(buildCircle( widget ));
			aisCircle->SetColor(Quantity_NOC_WHITE);
			widget->getContext()->Display(aisCircle,Standard_True);
			aisCircle.Nullify();
			myIsDrawing = false;
			parameter = Centre;
			break;
		}
	}
}

