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

#ifndef QOCCVIEWFRAME_H
#define QOCCVIEWFRAME_H

#include <QToolBar>
#include <QMenu>
#include <QFrame>

#include "qocc.h"

class QoccMainWindow;
class QoccViewWidget;
class QoccViewerContext;
class QVBoxLayout;
class Handle_AIS_InteractiveContext;

class QOCC_DECLSPEC QoccViewFrame : public QFrame
{
    Q_OBJECT

public:

    QoccViewFrame ( QoccViewerContext* aContext = NULL, 
                    QWidget *parent = NULL, 
                    Qt::WindowFlags wflags = 0 );

    ~QoccViewFrame();

    QoccViewWidget* getOCCWidget() { return myOCCWidget; }
    QToolBar*       getToolBar()   { return myToolBar; }

signals:

public slots:
    void fitArea      ();
    void fitAll       ();
    void fitExtents   ();
    void viewAxo      ();
    void viewTop      ();
    void viewLeft     ();
    void viewFront    ();
    void viewTopFront ();

protected: // methods

private: // members
    QoccViewerContext* myContext;
    QoccViewWidget*    myOCCWidget;
    
    QToolBar*		myToolBar;
    QVBoxLayout*	myLayout;
    QoccMainWindow* myParent;

    QAction* actionIdle;
    
    QAction* actionZoomGroup;
    QMenu*   popupZoomGroup;
    QAction* actionFitExtents;
    QAction* actionFitArea;
    QAction* actionFitAll;

    QAction* actionViewGroup;
    QMenu*   popupViewGroup;
    QAction* actionViewTop;
    QAction* actionViewFront;
    QAction* actionViewTopFront;
    QAction* actionViewLeft;
    QAction* actionViewAxo;

private: // methods
    void setToolbarActions( void );

    void createActions( void );

    void connectActions( void );
    void disconnectActions( void );

    void connectParent( void );
    void disconnectParent( void );

    void createZoomGroup( void );
    void createViewGroup( void );

    void invokeAction( QAction* group, QAction* action, const char* signal );

public:

};

#endif // QOCCVIEWFRAME_H
