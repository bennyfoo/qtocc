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

#ifndef QOCCVIEWERCONTEXT_H
#define QOCCVIEWERCONTEXT_H

#include <QtCore/QObject>
#include "qocc.h"

#include <TopAbs_ShapeEnum.hxx>

class QoccCommand;
class QoccViewWidget;

class QOCC_DECLSPEC QoccViewerContext : public QObject
{

    Q_OBJECT

public:

    QoccViewerContext();
    QoccViewerContext( Handle_AIS_InteractiveContext context, Handle_V3d_Viewer viewer );

    ~QoccViewerContext();

    Handle_V3d_Viewer&              getViewer();
    Handle_AIS_InteractiveContext&  getContext();

    static Handle_V3d_Viewer createViewer(	const Standard_CString aDisplay,
                                    const Standard_ExtString aName,
                                    const Standard_CString aDomain,
                                    const Standard_Real ViewSize );

    void setLocalContext( TopAbs_ShapeEnum lcType );
    void clearLocalContexts();

    void deleteAllObjects();

    void setGridOffset (Quantity_Length offset);

    bool isCommandActive()	{ return !(myCommand == NULL); }
    bool isDrawing();		

    void setCommand ( QoccCommand* command );
    void clearCommand ( );

public slots:

    void gridXY   ( void );
    void gridXZ   ( void );
    void gridYZ   ( void );
    void gridOn   ( void );
    void gridOff  ( void );
    void gridRect ( void );
    void gridCirc ( void );

    void clickEvent(QoccViewWidget* widget);
    void moveEvent (QoccViewWidget* widget);


signals:

    void error (int errorCode, QString& errorDescription);

private:

    Handle_V3d_Viewer				myViewer;
    Handle_AIS_InteractiveContext	myContext;
    Aspect_GridType					myGridType;
    Aspect_GridDrawMode				myGridMode;
    Quantity_NameOfColor			myGridColor;
    Quantity_NameOfColor			myGridTenthColor;
    QoccCommand*					myCommand;
    bool							myIsDrawing;

};

#endif // QOCCVIEWERCONTEXT_H
