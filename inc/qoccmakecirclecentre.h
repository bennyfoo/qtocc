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
#ifndef QOCCCIRCLECENTRE_H
#define QOCCCIRCLECENTRE_H

#include <QtCore/QObject>
#include "qocc.h"
#include "qocccommand.h"

class QOCC_DECLSPEC QoccCircleCentreDlg : public QObject
{
    Q_OBJECT

public:

    QoccCircleCentreDlg( );
    ~QoccCircleCentreDlg( );

private: 

    
};

class QOCC_DECLSPEC QoccMakeCircleCentre : public QoccCommand
{
    Q_OBJECT

public: //Enumerations
    enum CircleParameter
    {
        Centre, Radius, Done
    };

public: //Methods

    QoccMakeCircleCentre( QAction* startingAction = NULL, QWidget* parent = NULL );
    ~QoccMakeCircleCentre( );

    virtual void clickEvent(QoccViewWidget* widget);
    virtual void moveEvent(QoccViewWidget*  widget);

protected: //
    TopoDS_Shape buildCircle( QoccViewWidget* widget );

protected: //Attributes

    gp_Pnt			 ptCentre;
    gp_Dir			 dirNormal;
    Handle_AIS_Shape aisCentre;
    Handle_AIS_Shape aisCircle;
    Standard_Real	 dRadius;
    CircleParameter  parameter;

};


#endif // QOCCCIRCLECENTRE_H
