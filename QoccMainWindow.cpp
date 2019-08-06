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

//#include <Windows.h>
#include <QtGui/QtGui>
#include <QFileDialog>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QMessageBox>
#include <QLineEdit>

#include <Aspect_RectangularGrid.hxx>
#include <V3d_View.hxx>

#include "qoccmainwindow.h"
#include "qocccommands.h"
#include "dlgbox.h"
#include "TPrsStd_AISViewer.hxx"
#include "TDocStd_Document.hxx"
#include "AIS_InteractiveContext.hxx"
#include "dlgbox.h"
#include "TOcaf_Commands.hxx"
#include "TPrsStd_AISPresentation.hxx"
#include "TNaming_NamedShape.hxx"
#include "TDataStd_Integer.hxx"
#include "AIS_Shape.hxx"
#include "TopAbs_ShapeEnum.hxx"
#include "AIS_ListOfInteractive.hxx"
#include "AIS_ListIteratorOfListOfInteractive.hxx"
#include "TDF_ChildIterator.hxx"

void LoadBottle ( Handle_AIS_InteractiveContext theContext );
void ShowOrigin ( Handle_AIS_InteractiveContext theContext );
void AddVertex  ( double x, double y, double z, Handle_AIS_InteractiveContext theContext );

QoccMainWindow::QoccMainWindow()
: myLastFolder(tr(""))
{
    m_ocafApp = new TOcaf_Application();
    m_ocafApp->NewDocument("Ocaf-Sample",m_ocafDoc);
    m_ocafDoc->SetUndoLimit(10);

    TCollection_ExtendedString a3DName( "Visual3D" );
    Handle_V3d_Viewer viewer = QoccViewerContext::createViewer( "DISPLAY", a3DName.ToExtString(), "", 1000.0 );
    //viewer->Init();
    //viewer->SetZBufferManagment( Standard_False );
    viewer->SetDefaultViewProj( V3d_Zpos );	// Top view
    viewer->SetDefaultLights();
    viewer->SetLightOn();

    TPrsStd_AISViewer::New( m_ocafDoc->Main(),viewer );
    Handle_AIS_InteractiveContext context;
    TPrsStd_AISViewer::Find(m_ocafDoc->Main(), context);
    context->SetDisplayMode(AIS_Shaded,Standard_True);
    
    myVC  = new QoccViewerContext( context, viewer );

    myTabWidget = new QTabWidget( this );
    myTabWidget->setObjectName( QString::fromUtf8( "myTabWidget" ) );
    myTabWidget->setTabShape( QTabWidget::Rounded );

    mySketchFrame = new QoccViewFrame( myVC, this );
    myTabWidget->addTab( mySketchFrame, tr( "Sketch" ) );

    setCentralWidget( myTabWidget );

    QAction* anAction = new QAction(tr("&Faces"), this);
    anAction->setStatusTip(tr("Highlight faces"));
    anAction->setIcon(QIcon(QString::fromUtf8(":/Resources/actions/22x22/viewmagall.png")));
        
    //QToolBar* toolBar = mySketchFrame->getToolBar();

    createActions();
    createMenus();

    statusBar()->showMessage(tr("A context menu is available by right-clicking"));

    setWindowTitle(tr("Qt OpenCASCADE"));
    setMinimumSize(160, 160);
}

QoccMainWindow::~QoccMainWindow()
{
    m_ocafApp->Close( m_ocafDoc );
}

void QoccMainWindow::eraseAllFromOcaf()
{
    m_ocafDoc->NewCommand();

    TDF_Label LabSat = m_ocafDoc->Main();

    for ( TDF_ChildIterator it(LabSat); it.More(); it.Next() )
    {
        TDF_Label L = it.Value();
        Handle_TNaming_NamedShape TNS;
        if ( !L.FindAttribute( TNaming_NamedShape::GetID(), TNS ) ) continue;
        Handle_TDataStd_Integer TDI;

        // To know if the object was displayed
        if ( L.FindAttribute( TDataStd_Integer::GetID(), TDI ) )
            if( !TDI->Get() )  continue;

        Handle_TPrsStd_AISPresentation  prs;
        if ( !L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs ) )
            prs = TPrsStd_AISPresentation::Set( L, TNaming_NamedShape::GetID() );
        prs->SetColor( Quantity_NOC_ORANGE );
        prs->Erase( 1 );
    }

    getContext()->UpdateCurrentViewer();

    m_ocafDoc->CommitCommand();
}

void QoccMainWindow::clearDocument() 
{
    m_ocafDoc->NewCommand();

    AIS_ListOfInteractive list;
    getContext()->DisplayedObjects( list );
    AIS_ListIteratorOfListOfInteractive it;	
    for(it.Initialize( list ); it.More(); it.Next() )
    {
        Handle(TPrsStd_AISPresentation) prs
            = Handle_TPrsStd_AISPresentation::DownCast( it.Value()->GetOwner() );
        TDataStd_Integer::Set(prs->Label(), 0);
        prs->Erase(1);
    }
 
    m_ocafDoc->CommitCommand();
    getContext()->UpdateCurrentViewer();
}

void QoccMainWindow::newFile()
{
    statusBar()->showMessage(tr("Invoked File|New"));
    eraseAllFromOcaf();
    
    myVC->clearCommand ();
    myVC->clearLocalContexts();
    myVC->deleteAllObjects();
//  	delete myVC;
// 	m_ocafApp->Close( m_ocafDoc );
    
// 	m_ocafApp->NewDocument("Ocaf-Sample",m_ocafDoc);
// 	m_ocafDoc->SetUndoLimit(10);

// 	TCollection_ExtendedString a3DName( "Visual3D" );
// 	Handle_V3d_Viewer viewer = QoccViewerContext::createViewer( "DISPLAY", a3DName.ToExtString(), "", 1000.0 );
// 	viewer->Init();
// 	viewer->SetZBufferManagment( Standard_False );
// 	viewer->SetDefaultViewProj( V3d_Zpos );	// Top view

// 	TPrsStd_AISViewer::New( m_ocafDoc->Main(),viewer );
// 	Handle_AIS_InteractiveContext context;
// 	TPrsStd_AISViewer::Find(m_ocafDoc->Main(), context);
// 	context->SetDisplayMode(AIS_Shaded);
// 	myVC = new QoccViewerContext( context, viewer );
}

void QoccMainWindow::open()
{
    QString		fileName;
    QString		fileType;
    QFileInfo	fileInfo;

    QoccInputOutput::FileFormat format;
    QoccInputOutput reader;

    statusBar()->showMessage(tr("Invoked File|Open"));
    myVC->clearCommand ();

    fileName = QFileDialog::getOpenFileName ( this,
                                              tr("Open File"),
                                              myLastFolder,
                                              tr( "All drawing types (*.brep *.rle *.igs *iges *.stp *.step *stl);;"
                                                  "BREP (*.brep *.rle);;"
                                                  "STEP (*.step *.stp);;"
                                                  "IGES (*.iges *.igs);;"
                                                  "STL (*.stl)" ));
    if (!fileName.isEmpty())
    {
        fileInfo.setFile(fileName);
        fileType = fileInfo.suffix();
        if (fileType.toLower() == tr("brep") || fileType.toLower() == tr("rle"))
        {
            format = QoccInputOutput::FormatBREP;
        }
        if (fileType.toLower() == tr("step") || fileType.toLower() == tr("stp"))
        {
            format = QoccInputOutput::FormatSTEP;
        }
        if (fileType.toLower() == tr("iges") || fileType.toLower() == tr("igs"))
        {
            format = QoccInputOutput::FormatIGES;
        }
        if (fileType.toLower() == tr("stl"))
        {
            format = QoccInputOutput::FormatSTL;
        }
        myVC->clearLocalContexts();
        myLastFolder = fileInfo.absolutePath();
        emit fileOpening();
        reader.importModel ( fileInfo.absoluteFilePath(), format, myVC->getContext() );
        emit fileOpened();
    }
}

void QoccMainWindow::openOcaf()
{
    statusBar()->showMessage(tr("Invoked File|Open Ocaf"));
    myVC->clearCommand ();

    QString fileName;
    QFileInfo fileInfo;
    fileName = QFileDialog::getOpenFileName ( this,
                                              tr("Open Ocaf File"),
                                              myLastFolder,
                                            tr( "All Ocaf types (*.xml *.cbf);;" 
                                            "XmlOcaf (*.xml);;"
                                            "BinOcaf (*.cbf);;"
                                            "Ocaf-Sample (*);;" ));
    if( fileName != "" )
    {
        fileInfo.setFile( fileName );
    
        TCollection_ExtendedString s( (char *) fileName.toStdString().c_str() );
    
        // open the document in the current application
    
        /*CDF_RetrievableStatus RS = */m_ocafApp->Open(s,m_ocafDoc);
    
        //connect the document CAF (myDoc) with the AISContext (myAISContext)
    //	TPrsStd_AISViewer::Has(myOcafDoc->Main());
        TPrsStd_AISViewer::New( m_ocafDoc->Main(),myVC->getViewer() );
        m_ocafDoc->SetUndoLimit(10);
    
        TPrsStd_AISViewer::Find(m_ocafDoc->Main(), myVC->getContext() );
    
        // Display the presentations (which was not stored in the document)
        displayPrs();
    
        myLastFolder = fileInfo.absolutePath();
    }
}
    
void QoccMainWindow::displayPrs()
{
    TDF_Label LabSat = m_ocafDoc->Main();
    
    for( TDF_ChildIterator it(LabSat); it.More(); it.Next() )
    {
        TDF_Label L = it.Value();
        Handle(TNaming_NamedShape) TNS;
        if (!L.FindAttribute(TNaming_NamedShape::GetID(), TNS)) continue;
        Handle(TDataStd_Integer) TDI;
    
        // To know if the object was displayed
        if (L.FindAttribute(TDataStd_Integer::GetID(), TDI))
            if(!TDI->Get())  continue;
    
        Handle(TPrsStd_AISPresentation) prs;
        if (!L.FindAttribute(TPrsStd_AISPresentation::GetID(),prs))
            prs = TPrsStd_AISPresentation::Set(L,TNaming_NamedShape::GetID());
        prs->SetColor(Quantity_NOC_ORANGE);
        prs->Display(1);
    }
    
    getContext()->UpdateCurrentViewer();
}


void QoccMainWindow::save()
{
    statusBar()->showMessage(tr("Invoked File|Save"));
    myVC->clearCommand ();

    QFileInfo fileInfo;
    QString fileName, fileType;

    fileName = QFileDialog::getSaveFileName (	this,
                            tr("Save File"), 
                            myLastFolder,
                            tr( "All Ocaf types (*.xml *.cbf);;" 
                            "XmlOcaf (*.xml);;"
                            "BinOcaf (*.cbf);;"
                            "Ocaf-Sample (*);;"
                             ) );

    fileInfo.setFile(fileName);
    fileType = fileInfo.suffix();


    // Choose storage format
    if( fileType == ("xml") )
    {
      // The document must be saved in XML format
        m_ocafDoc->ChangeStorageFormat("XmlOcaf");
    }
    else if( fileType == "cbf" )
    {
      // The document must be saved in binary format
        m_ocafDoc->ChangeStorageFormat("BinOcaf");
    }
    else
    {
        // The document must be saved in standard format
        m_ocafDoc->ChangeStorageFormat("Ocaf-Sample");
    }

    TCollection_ExtendedString s( (char *) fileName.toStdString().c_str() );
    cout << "saving document to " << s << endl;
    try
    {
        // Saves the document in the current application
        m_ocafApp->SaveAs( m_ocafDoc,s );
    }
    catch(...)
    {
        cout << "Error! The file wasn't saved.";
        return;
    }
}

void QoccMainWindow::print()
{
    statusBar()->showMessage(tr("Invoked File|Print"));
    myVC->clearCommand ();
}

void QoccMainWindow::undo()
{
    statusBar()->showMessage(tr("Invoked Edit|Undo"));
    myVC->clearCommand ();
    m_ocafDoc->Undo();
    m_ocafDoc->CommitCommand();
    getContext()->UpdateCurrentViewer();

}

void QoccMainWindow::redo()
{
    statusBar()->showMessage(tr("Invoked Edit|Redo"));
    myVC->clearCommand ();
    m_ocafDoc->Redo();
    m_ocafDoc->CommitCommand();
    getContext()->UpdateCurrentViewer();
}

void QoccMainWindow::cut()
{
    statusBar()->showMessage(tr("Invoked Edit|Cut"));
    myVC->clearCommand ();
}

void QoccMainWindow::copy()
{
    statusBar()->showMessage(tr("Invoked Edit|Copy"));
    myVC->clearCommand ();
}

void QoccMainWindow::paste()
{
    statusBar()->showMessage(tr("Invoked Edit|Paste"));
    myVC->clearCommand ();
}

void QoccMainWindow::about()
{
    statusBar()->showMessage(tr("Invoked Help|About"));
    myVC->clearCommand ();
    QMessageBox::about(this, tr("About Menu"),
                       tr("This Qt<b>OPENCASCADE</b> example shows "
                          "how to create a simple Qt4 Viewer."));
}

void QoccMainWindow::aboutQt()
{
    statusBar()->showMessage(tr("Invoked Help|About Qt"));
    myVC->clearCommand ();
    qApp->aboutQt();
}

void QoccMainWindow::selectNeutralPoint()
{
    statusBar()->showMessage(tr("Invoked Select|Clear Filters"));
    myVC->clearCommand ();
    myVC->clearLocalContexts();
}
void QoccMainWindow::selectFaces()
{
    statusBar()->showMessage(tr("Invoked Select|Faces"));
    myVC->setLocalContext(TopAbs_FACE);
}

void QoccMainWindow::selectVertices()
{
    statusBar()->showMessage(tr("Invoked Select|Vertices"));
    myVC->setLocalContext(TopAbs_VERTEX);
}

void QoccMainWindow::selectEdges()
{
    statusBar()->showMessage(tr("Invoked Select|Edges"));
    myVC->setLocalContext(TopAbs_EDGE);
}
void QoccMainWindow::selectWires()
{
    statusBar()->showMessage(tr("Invoked Select|Wires"));
    myVC->setLocalContext(TopAbs_WIRE);
}
void QoccMainWindow::selectShells()
{
    statusBar()->showMessage(tr("Invoked Select|Shells"));
    myVC->setLocalContext(TopAbs_SHELL);
}
void QoccMainWindow::selectSolids()
{
    statusBar()->showMessage(tr("Invoked Select|Solids"));
    myVC->setLocalContext(TopAbs_SOLID);
}
void QoccMainWindow::selectCompSolids()
{
    statusBar()->showMessage(tr("Invoked Select|Compound Solids"));
    myVC->setLocalContext(TopAbs_COMPSOLID);
}
void QoccMainWindow::selectCompounds()
{
    statusBar()->showMessage(tr("Invoked Select|Compounds"));
    myVC->setLocalContext(TopAbs_COMPOUND);
}
void QoccMainWindow::selectShapes()
{
    statusBar()->showMessage(tr("Invoked Select|Shapes"));
    myVC->setLocalContext(TopAbs_SHAPE);
}

void QoccMainWindow::circleCentre()
{
    statusBar()->showMessage(tr("Invoked Draw|Circle Centre"));
    QoccMakeCircleCentre* cc = new QoccMakeCircleCentre( circleCentreAction );
    myVC->setCommand ( cc );
}

void QoccMainWindow::vertex()
{
    statusBar()->showMessage(tr("Invoked Draw|Vertex"));
    QoccMakePoint* cc = new QoccMakePoint( pointAction );
    myVC->setCommand ( cc );
}

void QoccMainWindow::vertexOnEdge()
{
    statusBar()->showMessage(tr("Invoked Draw|Vertex on Edge"));
    QoccMakePointOnEdge* cc = new QoccMakePointOnEdge( pointOnEdgeAction );
    myVC->setCommand ( cc );
}

void QoccMainWindow::cancelDraw()
{
    statusBar()->showMessage(tr("Cancel Draw"));
    myVC->clearCommand ( );
}

void QoccMainWindow::bottle()
{
    statusBar()->showMessage(tr("Invoked File|Load Bottle"));
    myVC->clearCommand ( );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    myVC->clearLocalContexts();
    myVC->deleteAllObjects ();
    // Call the "bottle factory" to load and display the shape
    LoadBottle (myVC->getContext());
    // Force redraw
    myVC->getViewer()->Redraw();
    QApplication::restoreOverrideCursor();
}
/*
void QoccMainWindow::xyzPosition ( QoccViewWidget* widget)
{
    QString aString;
    QTextStream ts(&aString);
    ts << widget->x() << "," << widget->y() << "," << widget->z();
    statusBar()->showMessage(aString);
}
*/
/*
void QoccMainWindow::addPoint (V3d_Coordinate X,
                               V3d_Coordinate Y,
                               V3d_Coordinate Z)
{
    AddVertex ( X, Y, Z, myVC->getContext() );
}
*/
void QoccMainWindow::statusMessage (const QString aMessage)
{
    statusBar()->showMessage(aMessage);
}
/*!
\brief	This routine is a utility function for creating actions. Use tr("str")
        in the calling function to provide support for language translations.
\param	caption		the menu caption.
\param  shortcut	keyboard shot.
\param	tip			the tool tip text.
\param	slot		the slot function to be called, e.g. use SLOT(fooBar()) .
\param	target		the QObject target of the connection, if not this object.
*/
QAction* QoccMainWindow::createAction(const QString& caption,
                                      const QString& shortcut,
                                      const QString& tip,
                                      const char* slot,
                                      const QObject* target)
{
    QAction* anAction = new QAction( caption, this );
    anAction->setShortcut( shortcut );
    anAction->setStatusTip( tip );
    connect( anAction, SIGNAL(triggered()), target == NULL ? this : target, slot );
    return anAction;
}

void QoccMainWindow::createActions()
{
    newAction   = createAction( tr("&New"),     tr("Ctrl+N"), tr("Create a new file"),         SLOT(newFile()) );
    openAction  = createAction( tr("&Open..."), tr("Ctrl+O"), tr("Open an existing file"),     SLOT(open()) );
    openOcafAction  = createAction( tr("&Open Ocaf file..."), tr("Ctrl+C"), tr("Open Ocaf file"),     SLOT(openOcaf()) );
    saveAction  = createAction( tr("&Save"),    tr("Ctrl+S"), tr("Save the document to disk"), SLOT(save()) );
    printAction = createAction( tr("&Print..."),tr("Ctrl+P"), tr("Print the document"),        SLOT(print()) );
    exitAction  = createAction( tr("E&xit"),    tr("Ctrl+X"), tr("Exit the application"),      SLOT(close()) );

    undoAction  = createAction( tr("&Undo"),    tr("Ctrl+Z"), tr("Undo the last operation"),   SLOT(undo()) );
    redoAction  = createAction( tr("&Redo"),    tr("Ctrl+Y"), tr("Redo the last operation"),   SLOT(redo()) );
    cutAction   = createAction( tr("Cu&t"),     tr("Ctrl+X"), tr("Cut to the clipboard"),      SLOT(cut()) );
    copyAction  = createAction( tr("&Copy"),    tr("Ctrl+C"), tr("Copy to the clipboard"),     SLOT(copy()) );
    pasteAction = createAction( tr("&Paste"),   tr("Ctrl+V"), tr("Paste the clipboard"),       SLOT(paste()) );

    aboutAction   = createAction( tr("&About"),    tr(""), tr("Show the application's About box"),  SLOT(about()) );
    aboutQtAction = createAction( tr("About &Qt"), tr(""), tr("Show the Qt library's About box"),   SLOT(aboutQt()) );
    gridOnAction  = createAction( tr("&Grid On"),  tr("Ctrl+G"), tr("Turn the grid on"), SLOT(gridOn()),  myVC );
    gridOffAction = createAction( tr("Gri&d Off"), tr("Ctrl+D"), tr("Turn the grid off"), SLOT(gridOff()), myVC );

    gridXYAction = createAction( tr("XY Grid"), tr(""), tr("Grid on XY Plane"), SLOT(gridXY()), myVC );
    gridXZAction = createAction( tr("XZ Grid"), tr(""), tr("Grid on XZ Plane"), SLOT(gridXZ()), myVC );
    gridYZAction = createAction( tr("YZ Grid"), tr(""), tr("Grid on YZ Plane"), SLOT(gridYZ()), myVC );

    gridRectAction = createAction( tr("Rectangular"), tr(""), tr("Retangular grid"), SLOT(gridRect()), myVC );
    gridCircAction = createAction( tr("Circular"),    tr(""), tr("Circular grid"),   SLOT(gridCirc()), myVC );

    
    pointAction = new QAction(tr("&Points"), this);
    pointAction->setStatusTip(tr("Points"));
    pointAction->setIcon(QIcon(QString::fromUtf8(":/Resources/icons/point.png")));
    connect( pointAction, SIGNAL( triggered() ), this, SLOT( vertex()) );
    
    circleCentreAction = new QAction(tr("&Circle Centre, Radius"), this);
    circleCentreAction->setStatusTip(tr("Circle - centre, radius"));
    circleCentreAction->setIcon(QIcon(QString::fromUtf8(":/Resources/icons/circlecentre.png")));
    connect( circleCentreAction, SIGNAL( triggered() ), this, SLOT( circleCentre()) );

    pointOnEdgeAction = new QAction(tr("&Point On Edge"), this);
    pointOnEdgeAction->setStatusTip(tr("Select Edge first, then select point on edge to create vertex"));
    pointOnEdgeAction->setIcon(QIcon(QString::fromUtf8(":/Resources/icons/circlecentre.png")));
    connect( pointOnEdgeAction, SIGNAL( triggered() ), this, SLOT( vertexOnEdge()) );

    cancelDrawAction = new QAction(tr("Cancel"), this);
    cancelDrawAction->setStatusTip(tr("Cancel"));

    connect( cancelDrawAction, SIGNAL( triggered() ), this, SLOT( cancelDraw()) );


/*	To be completed
    QAction *selectNeutralAction;
    QAction *selectCompoundAction;	
    QAction *selectCompSolidAction;	
    QAction *selectCompSolid;		
    QAction *selectShapeAction;		
*/
    selectVertexAction  = createAction(tr("&Vertices"), "", tr("Select Vertices"),     SLOT(selectVertices()));
    selectEdgeAction    = createAction(tr("&Edges"),    "", tr("Select Edges"),        SLOT(selectEdges()));
    selectWireAction    = createAction(tr("&Wires"),    "", tr("Select Wires"),        SLOT(selectWires()));
    selectFaceAction    = createAction(tr("&Faces"),    "", tr("Select Faces"),        SLOT(selectFaces()));
    selectShellAction   = createAction(tr("&Shells"),   "", tr("Select Shells"),       SLOT(selectShells()));
    selectNeutralAction = createAction(tr("&Normal"),   "", tr("No selection filter"), SLOT(selectNeutralPoint()));

    // And the bottle example

    bottleAction = new QAction(tr("Load &Bottle"), this);
    bottleAction->setShortcut(tr("Ctrl+B"));
    bottleAction->setStatusTip(tr("Bottle sample."));
    connect(bottleAction, SIGNAL(triggered()), this, SLOT(bottle()));

    boxAction = new QAction(tr("New Box"), this);
    connect(boxAction, SIGNAL(triggered()), this, SLOT(createBox()));

    undoAction = new QAction(tr("Undo"), this);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

#if 1
    fitAction = new QAction(tr("&Fit Window"), this);
    fitAction->setShortcut(tr("Ctrl+F"));
    fitAction->setStatusTip(tr("Fit to window"));
    connect(fitAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(fitExtents()));

    fitAllAction = new QAction(tr("Fit &All"), this);
    fitAllAction->setShortcut(tr("Ctrl+A"));
    fitAllAction->setStatusTip(tr("Fit contents to viewport"));
    connect(fitAllAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(fitAll()));

    zoomAction = new QAction(tr("&Zoom"), this);
    zoomAction->setStatusTip(tr("Zoom in window"));
    connect(zoomAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(fitArea()));

    panAction = new QAction(tr("&Pan"), this);
    panAction->setStatusTip(tr("Window panning"));
    connect(panAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(pan()));

    rotAction = new QAction(tr("&Rotate"), this);
    rotAction->setShortcut(tr("Ctrl+R"));
    rotAction->setStatusTip(tr("Window rotation"));
    connect(rotAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(rotation()));

    viewFrontAction = new QAction(tr("Front"), this);
    viewFrontAction->setStatusTip(tr("View From Front"));
    connect(viewFrontAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewFront()));

    viewBackAction = new QAction(tr("Back"), this);
    viewBackAction->setStatusTip(tr("View From Back"));
    connect(viewBackAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewBack()));

    viewTopAction = new QAction(tr("Top"), this);
    viewTopAction->setStatusTip(tr("View From Top"));
    connect(viewTopAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewTop()));

    viewBottomAction = new QAction(tr("Bottom"), this);
    viewBottomAction->setStatusTip(tr("View From Bottom"));
    connect(viewBottomAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewBottom()));

    viewLeftAction = new QAction(tr("Left"), this);
    viewLeftAction->setStatusTip(tr("View From Left"));
    connect(viewLeftAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewLeft()));

    viewRightAction = new QAction(tr("Right"), this);
    viewRightAction->setStatusTip(tr("View From Right"));
    connect(viewRightAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewRight()));

    viewAxoAction = new QAction(tr("Axonometric"), this);
    viewAxoAction->setStatusTip(tr("Axonometric view"));
    connect(viewAxoAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewAxo()));

    viewGridAction = new QAction(tr("Grid"), this);
    viewGridAction->setStatusTip(tr("View from grid"));
    connect(viewGridAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewGrid()));

    viewResetAction = new QAction(tr("Reset"), this);
    viewResetAction->setStatusTip(tr("Reset the view"));
    connect(viewResetAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(viewReset()));

    backgroundAction = new QAction(tr("&Background"), this);
    backgroundAction->setStatusTip(tr("Change the background colour"));
    connect(backgroundAction, SIGNAL(triggered()), mySketchFrame->getOCCWidget(), SLOT(background()));

    // The co-ordinates from the view
    connect(mySketchFrame->getOCCWidget(), SIGNAL(mouseMoved(V3d_Coordinate, V3d_Coordinate, V3d_Coordinate)),
        this, SLOT(xyzPosition(V3d_Coordinate, V3d_Coordinate, V3d_Coordinate)));

    // Add a point from the view
    connect(mySketchFrame->getOCCWidget(), SIGNAL(pointClicked(V3d_Coordinate, V3d_Coordinate, V3d_Coordinate)),
        this, SLOT(addPoint(V3d_Coordinate, V3d_Coordinate, V3d_Coordinate)));

    connect(mySketchFrame->getOCCWidget(), SIGNAL(sendStatus(const QString)),
        this, SLOT(statusMessage(const QString)));
#endif
}

void QoccMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr("&File") );
        fileMenu->addAction( newAction );
        fileMenu->addAction( openAction );
        fileMenu->addAction( openOcafAction );
        fileMenu->addAction( saveAction );
        fileMenu->addAction( printAction );
/*
        Comment out the 2 lines below to hide the
        Load Bottle menu option - still left in for
        now as a demo feature.
*/
        fileMenu->addSeparator();
        fileMenu->addAction( bottleAction );
        fileMenu->addAction( boxAction );

        fileMenu->addSeparator();
        fileMenu->addAction( exitAction );

    editMenu = menuBar()->addMenu( tr("&Edit") );
        editMenu->addAction( undoAction );
        editMenu->addAction( redoAction );
        editMenu->addSeparator();
        editMenu->addAction( cutAction );
        editMenu->addAction( copyAction );
        editMenu->addAction( pasteAction );

    drawMenu = menuBar()->addMenu( tr("&Draw") );
        drawMenu->addAction( pointAction );
        drawMenu->addAction( circleCentreAction );
        drawMenu->addAction( pointOnEdgeAction );
        drawMenu->addSeparator();
        drawMenu->addAction( cancelDrawAction );

    selectMenu = menuBar()->addMenu( tr("&Select") );
        selectMenu->addAction( selectVertexAction );
        selectMenu->addAction( selectEdgeAction );
        selectMenu->addAction( selectWireAction );
        selectMenu->addAction( selectFaceAction );
        selectMenu->addAction( selectShellAction );
        selectMenu->addSeparator();
        selectMenu->addAction( selectNeutralAction );

    viewMenu = menuBar()->addMenu( tr("&View") );
#if 1
        viewDisplayMenu = viewMenu->addMenu( tr("&Display") );
            viewDisplayMenu->addAction( viewFrontAction );
            viewDisplayMenu->addAction( viewBackAction );
            viewDisplayMenu->addAction( viewTopAction );
            viewDisplayMenu->addAction( viewBottomAction );
            viewDisplayMenu->addAction( viewLeftAction );
            viewDisplayMenu->addAction( viewRightAction );
            viewDisplayMenu->addSeparator();
            viewDisplayMenu->addAction( viewAxoAction );
            viewDisplayMenu->addAction( viewGridAction );
            viewDisplayMenu->addSeparator();
            viewDisplayMenu->addAction( viewResetAction );
            viewDisplayMenu->addSeparator();
            viewDisplayMenu->addAction( backgroundAction );

        viewActionsMenu = viewMenu->addMenu( tr("&Actions") );
            viewActionsMenu->addAction( fitAction );
            viewActionsMenu->addAction( fitAllAction );
            viewActionsMenu->addAction( zoomAction );
            viewActionsMenu->addAction( panAction );
            viewActionsMenu->addAction( rotAction );
#endif
        gridMenu = viewMenu->addMenu( tr("&Grid") );
            gridMenu->addAction( gridOnAction );
            gridMenu->addAction( gridOffAction );
            gridMenu->addSeparator();
            gridMenu->addAction( gridXYAction );
            gridMenu->addAction( gridXZAction );
            gridMenu->addAction( gridYZAction );
            gridMenu->addSeparator();
            gridMenu->addAction( gridRectAction );
            gridMenu->addAction( gridCircAction );

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void QoccMainWindow::createBox()
{
    DlgBox *dlg = new DlgBox( this );
    if( dlg->exec() )
    {
        m_ocafDoc->NewCommand();
        TOcaf_Commands TSC(m_ocafDoc->Main());
        
        TCollection_ExtendedString  s = (char *)(dlg->eName->text().toStdString().c_str());
        // Create a new box using the CNewBoxDlg Dialog parameters as attributes
        TDF_Label L=TSC.CreateBox(dlg->eX->value(), dlg->eY->value(), dlg->eZ->value(), 
                                  dlg->eW->value(), dlg->eH->value(), dlg->eD->value(), s );
    
        // Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
        Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
/*		prs->SetSelectionMode( AIS_Shape::SelectionMode( TopAbs_SHAPE ) );
        prs->SetMode( AIS_Shape::SelectionMode( TopAbs_SHAPE ) );*/
        // Display it
        prs->Display( Standard_True );
//   		getContext()->SetSelected( prs->GetAIS() );
// 		Fit3DViews();
        // Attach an integer attribute to L to memorize it's displayed
        TDataStd_Integer::Set(L, 1);
    
        // Close the command (for undo)
        m_ocafDoc->CommitCommand();
    }
}

