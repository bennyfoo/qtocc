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
#ifndef QOCCPOINTONEDGE_H
#define QOCCPOINTONEDGE_H

#include <QtCore/QObject>
#include <QtGui/QCursor>

#include "qocc.h"
#include "qocccommand.h"

class QOCC_DECLSPEC QoccPointOnEdgeDlg : public QObject
{
    Q_OBJECT

public:

    QoccPointOnEdgeDlg( );
    ~QoccPointOnEdgeDlg( );

private: 

    
};

class QOCC_DECLSPEC QoccMakePointOnEdge : public QoccCommand
{
    Q_OBJECT

public: //Enumerations
/*	enum CircleParameter
    {
        Centre, Radius, Done
    };
    */

    enum PointOnEdgeParameter
    {
        Selection, Point, Done
    };

    public: //Methods

    QoccMakePointOnEdge( QAction* startingAction = NULL, QWidget* parent = NULL );
    ~QoccMakePointOnEdge( );

    virtual void clickEvent(QoccViewWidget* widget);
    virtual void moveEvent(QoccViewWidget*  widget);

protected: //
// 	TopoDS_Shape buildCircle( QoccViewWidget* widget );
    TopoDS_Shape buildPoint( QoccViewWidget* widget );
    gp_Pnt ConvertMouseToPoint(int x, int y, QoccViewWidget* widget);

protected: //Attributes

    gp_Pnt			 ptCentre;
    gp_Dir			 dirNormal;
    Handle_AIS_Shape aisCentre;
    Handle_AIS_Shape aisCircle;
    Standard_Real	 dRadius;
    QCursor 		 myInvisibleCursor;
    // 	CircleParameter  parameter;

    PointOnEdgeParameter	parameter;
    TopoDS_Shape	 edge;
    gp_XYZ 			 edgeBnd1, edgeBnd2;
    Standard_Real	 edgeLength;
    gp_Pnt			 ptOnEdge;
    Handle_AIS_Shape aisPoint;
};


#endif // QOCCCIRCLECENTRE_H
