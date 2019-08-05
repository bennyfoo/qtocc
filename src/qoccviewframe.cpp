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
\class	QoccViewFrame
\brief	This class provides a QFrame derived object containing a toolbar and an
        OpenCascade viewer widget (\see QoccViewWidget). The toolbar wraps several 
        utility functions that manipulate the view.
\author	Peter C. Dolbey
*/
#include <QVBoxLayout>
#include <QAction>

#include "qoccviewframe.h"
#include "qoccviewwidget.h"
#include "qoccmainwindow.h"

/*!
\brief	Constructor for QoccViewFrame.
        This constructor creates an uninitialised window for rendering a
        Handle_V3d_View based OpenCASCADE 3D viewer.
\param	aContext The widget's AIS context
\param	parent The widget's parent widget
\param	wflags The widget's window configuration flags
\see	initialize
*/
QoccViewFrame::QoccViewFrame(	QoccViewerContext* aContext, 
                                QWidget *parent, 
                                Qt::WindowFlags wflags )
  : QFrame( parent, wflags )
{
    myContext = aContext;
    myParent = static_cast<QoccMainWindow*> (parent);
    myLayout = new QVBoxLayout( this );
    myLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    myLayout->setMargin(0);
    myLayout->setSpacing(0);

    myToolBar = new QToolBar( this );
    myToolBar->setAutoFillBackground(true);   
    
    myOCCWidget = new QoccViewWidget( myContext, parent, wflags );

    createActions();
    setToolbarActions();
    connectActions();
    connectParent();

    myLayout->addWidget( myToolBar );
    myLayout->addWidget( myOCCWidget );

}
/*! 
\brief	The class destructor
*/
QoccViewFrame::~QoccViewFrame()
{
    disconnectActions();
    disconnectParent();

    delete actionFitExtents;
    delete actionFitArea;
    delete actionFitAll;
    delete actionZoomGroup;

    delete myToolBar;
    delete myLayout;
    delete myOCCWidget;
}

/*!
\brief	createActions sets up all the singleton toolbar actions, and calls 
        the grouped action functions.
*/
void QoccViewFrame::createActions( void)
{
    actionIdle = new QAction( tr("Idle"), this );
    actionIdle->setStatusTip(tr("Selection Mode"));
    actionIdle->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/arrow.png")));

    createZoomGroup();
    createViewGroup();
}

void QoccViewFrame::createZoomGroup( void )
{
    actionZoomGroup = new QAction( this );
    {
        actionFitExtents = new QAction(tr("&Fit to Extents"), this);
        actionFitExtents->setStatusTip(tr("Show entire drawing"));
        actionFitExtents->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/viewmagall.png")));

        actionFitArea = new QAction(tr("&Zoom Selected Area"), this);
        actionFitArea->setStatusTip(tr("Zoom to selected region"));
        actionFitArea->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/viewmagfit.png")));

        actionFitAll = new QAction(tr("Zoom &All"), this);
        actionFitAll->setStatusTip(tr("Zoom entire object"));
        actionFitAll->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/viewmag.png")));

        popupZoomGroup = new QMenu ( this );
        popupZoomGroup->addAction (actionFitExtents);
        popupZoomGroup->addAction (actionFitArea);
        popupZoomGroup->addAction (actionFitAll);
    }
    actionZoomGroup->setText(actionFitExtents->text());
    actionZoomGroup->setStatusTip(actionFitExtents->statusTip());
    actionZoomGroup->setIcon(actionFitExtents->icon());
    actionZoomGroup->setMenu(popupZoomGroup);
}

void QoccViewFrame::createViewGroup( void )
{
    actionViewGroup = new QAction ( this );
    {
        actionViewTop = new QAction(tr("View from &Top"), this);
        actionViewTop->setStatusTip(tr("Top View"));
        actionViewTop->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/planview.png")));

        actionViewFront = new QAction(tr("View from &Front"), this);
        actionViewFront->setStatusTip(tr("Front View"));
        actionViewFront->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/frontview.png")));

        actionViewLeft = new QAction(tr("View from &Left"), this);
        actionViewLeft->setStatusTip(tr("Left View"));
        actionViewLeft->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/leftview.png")));

        actionViewTopFront = new QAction(tr("View from Top Front"), this);
        actionViewTopFront->setStatusTip(tr("Top Front View"));
        actionViewTopFront->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/topfrontview.png")));

        actionViewAxo = new QAction(tr("&Axonometric View"), this);
        actionViewAxo->setStatusTip(tr("Axo View"));
        actionViewAxo->setIcon(QIcon(QString::fromUtf8(":/icons/actions/22x22/axoview.png")));

        popupViewGroup = new QMenu ( this );
        popupViewGroup->addAction (actionViewTop);
        popupViewGroup->addAction (actionViewFront);
        popupViewGroup->addAction (actionViewLeft);
        popupViewGroup->addSeparator ();
        popupViewGroup->addAction (actionViewTopFront);
        popupViewGroup->addAction (actionViewAxo);
    }
    actionViewGroup->setText(actionViewAxo->text());
    actionViewGroup->setStatusTip(actionViewAxo->statusTip());
    actionViewGroup->setIcon(actionViewAxo->icon());
    actionViewGroup->setMenu(popupViewGroup);
}

void QoccViewFrame::setToolbarActions()
{
    myToolBar->addAction(actionIdle);
    myToolBar->addSeparator();
    myToolBar->addAction(actionZoomGroup);
    myToolBar->addSeparator();
    myToolBar->addAction(actionViewGroup);
}

void QoccViewFrame::connectActions()
{
    connect( actionIdle, SIGNAL(triggered()), myOCCWidget, SLOT(idle()));

    connect( actionZoomGroup,  SIGNAL(triggered()), myOCCWidget, SLOT(fitExtents()) );
    {
        connect( actionFitExtents, SIGNAL(triggered()), this, SLOT(fitExtents()) );
        connect( actionFitArea,    SIGNAL(triggered()), this, SLOT(fitArea()) );
        connect( actionFitAll,     SIGNAL(triggered()), this, SLOT(fitAll()) );
    }
    connect( actionViewGroup,  SIGNAL(triggered()), myOCCWidget, SLOT(viewAxo()) );
    {
        connect( actionViewTop,      SIGNAL(triggered()), this, SLOT(viewTop()) );
        connect( actionViewLeft,     SIGNAL(triggered()), this, SLOT(viewLeft()) );
        connect( actionViewFront,    SIGNAL(triggered()), this, SLOT(viewFront()) );
        connect( actionViewTopFront, SIGNAL(triggered()), this, SLOT(viewTopFront()) );
        connect( actionViewAxo,      SIGNAL(triggered()), this, SLOT(viewAxo()) );
    }
}

void QoccViewFrame::disconnectActions()
{
    actionIdle->disconnect();
    
    actionZoomGroup->disconnect();
    actionFitExtents->disconnect();
    actionFitArea->disconnect();
    actionFitAll->disconnect();
}

void QoccViewFrame::connectParent()
{
    // Connect the messages from this widget to the status bar
    // display on the main window.
    connect( myOCCWidget, SIGNAL(sendStatus(const QString)), 
             myParent,    SLOT  (statusMessage(const QString)) );

    // Connect application event signals to the widget
    connect( myParent,    SIGNAL(fileOpening()), 
             myOCCWidget, SLOT  (refresh()) );
    connect( myParent,    SIGNAL(fileOpened()),  
             myOCCWidget, SLOT  (newShape()) );

    // connect widget events to the context wrapper
    connect( myOCCWidget, SIGNAL(mouseMoved(QoccViewWidget*)), 
             myContext,   SLOT  (moveEvent(QoccViewWidget*)) );
    // connect widget events to the context wrapper
    connect( myOCCWidget, SIGNAL(mouseClicked(QoccViewWidget*)), 
             myContext,   SLOT  (clickEvent(QoccViewWidget*)) );

}

void QoccViewFrame::disconnectParent()
{
    myOCCWidget->disconnect();
}

void QoccViewFrame::invokeAction( QAction* group, QAction* action, const char* signal )
{
    group->setText     ( action->text() );
    group->setStatusTip( action->statusTip() );
    group->setIcon     ( action->icon() );
    if ( myOCCWidget )
    {
        group->disconnect( );
        connect ( group, SIGNAL(triggered()), myOCCWidget, signal );
        group->trigger();
    }
}

void QoccViewFrame::fitExtents( void )
{
    invokeAction ( actionZoomGroup,  actionFitExtents, SLOT(fitExtents()) );
}

void QoccViewFrame::fitArea( void )
{
    invokeAction ( actionZoomGroup,  actionFitArea, SLOT( fitArea()) );
}

void QoccViewFrame::fitAll( void )
{
    invokeAction ( actionZoomGroup,  actionFitAll, SLOT(fitAll()) );
}

void QoccViewFrame::viewAxo( void )
{
    invokeAction (actionViewGroup, actionViewAxo, SLOT(viewAxo()) );
}

void QoccViewFrame::viewTop( void )
{
    invokeAction ( actionViewGroup, actionViewTop, SLOT(viewTop()) );
}

void QoccViewFrame::viewFront( void )
{
    invokeAction ( actionViewGroup,  actionViewFront, SLOT(viewFront()) );
}

void QoccViewFrame::viewTopFront( void )
{
    invokeAction ( actionViewGroup,  actionViewTopFront, SLOT(viewTopFront()) );
}

void QoccViewFrame::viewLeft( void )
{
    invokeAction ( actionViewGroup, actionViewLeft, SLOT(viewLeft()) );
}

