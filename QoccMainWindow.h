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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeWidget>
#include <QWidget>

#include "qoccviewercontext.h"
#include "qoccviewframe.h"
#include "qoccviewwidget.h"
#include "qoccinputoutput.h"
#include "TOcaf_Application.hxx"

class QAction;
class QLabel;
class QMenu;

class QoccMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QoccMainWindow();
    ~QoccMainWindow();
    Handle_AIS_InteractiveContext& getContext() { return myVC->getContext(); };

signals:
    void fileOpening();
    void fileOpened();

private slots:
    void newFile();
    void open();
    void openOcaf();
    void save();
    void print();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void about();
    void bottle();
    void aboutQt();
    void createBox();
    void clearDocument();
    void eraseAllFromOcaf();
    //void xyzPosition ( QoccViewWidget* widget );
    //void addPoint (V3d_Coordinate X, V3d_Coordinate Y, V3d_Coordinate Z);

    void statusMessage (const QString aMessage);

    void selectNeutralPoint();
    void selectVertices();
    void selectEdges();
    void selectWires();
    void selectFaces();
    void selectShells();
    void selectSolids();
    void selectCompSolids();
    void selectCompounds();
    void selectShapes();

    void circleCentre();
    void vertex();
    void vertexOnEdge();
    void cancelDraw();
    void displayPrs();


private:
    QAction* createAction(	const QString& caption, 
                            const QString& shortcut, 
                            const QString& tip,
                            const char* slot,
                            const QObject* target = NULL);
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *drawMenu;
    QMenu *viewMenu;
    QMenu *viewActionsMenu;
    QMenu *viewDisplayMenu;
    QMenu *formatMenu;
    QMenu *helpMenu;
    QMenu *gridMenu;
    QMenu *selectMenu;

    QAction *newAction;
    QAction *openAction;
    QAction *openOcafAction;
    QAction *saveAction;
    QAction *printAction;
    QAction *exitAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *aboutAction;

    QAction *fitAction;
    QAction *fitAllAction;
    QAction *zoomAction;
    QAction *panAction;
    QAction *rotAction;

    QAction *gridXYAction;
    QAction *gridXZAction;
    QAction *gridYZAction;
    QAction *gridOnAction;
    QAction *gridOffAction;
    QAction *gridRectAction;
    QAction *gridCircAction;

    QAction *viewFrontAction;
    QAction *viewBackAction;
    QAction *viewTopAction;
    QAction *viewBottomAction;
    QAction *viewLeftAction;
    QAction *viewRightAction;
    QAction *viewAxoAction;
    QAction *viewResetAction;
    QAction *viewGridAction;		// Grid view action 

    QAction *selectNeutralAction;
    QAction *selectCompoundAction;	// TopAbs_COMPOUND
    QAction *selectCompSolidAction;	// TopAbs_COMPSOLID
    QAction *selectSolidAction;		// TopAbs_SOLID
    QAction *selectShellAction;		// TopAbs_SHELL
    QAction *selectFaceAction;		// TopAbs_FACE
    QAction *selectWireAction;		// TopAbs_WIRE
    QAction *selectEdgeAction;		// TopAbs_EDGE
    QAction *selectVertexAction;	// TopAbs_VERTEX
    QAction *selectShapeAction;		// TopAbs_SHAPE

    QAction *pointAction;
    QAction *circleCentreAction;	// Circle drawing action Mk I
    QAction *pointOnEdgeAction;
    QAction *cancelDrawAction;

    QAction *backgroundAction;

    QAction *aboutQtAction;

    //Sample Action
    QAction *bottleAction;

    QAction *boxAction;


    // The OpenCASCADE Qt widget and context;
    // QoccViewWidget*     myOCC;
    QoccViewerContext*  myVC;

    QTabBar*			myTabBar;
    QTabWidget*			myTabWidget;
    QWidget*			myTab;

    QoccViewFrame*      mySketchFrame;
    QoccViewFrame*      myGeometryFrame;
    QoccViewFrame*      myAssemblyFrame;
    QoccViewFrame*      myMeshFrame;

    QString myLastFolder;
    
    Handle(TOcaf_Application)	m_ocafApp;
    Handle(TDocStd_Document) 	m_ocafDoc;

};

#endif

