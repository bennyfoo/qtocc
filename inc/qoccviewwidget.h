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

#ifndef QOCCVIEWWIDGET_H
#define QOCCVIEWWIDGET_H

#include <QRubberBand>
#include <QToolBar>

#include "qocc.h"
#include "qoccviewercontext.h"

/** the key for multi selection */
#define MULTISELECTIONKEY  Qt::ShiftModifier   

/** The key for shortcut ( use to activate dynamic rotation, panning ) */
#define CASCADESHORTCUTKEY Qt::ControlModifier 

#define ValZWMin 1 /** For elastic bean selection */

class Handle_AIS_InteractiveContext;
class Handle_V3d_Viewer;
class Handle_V3d_View;

class QOCC_DECLSPEC QoccViewWidget : 
public QWidget 
{
    Q_OBJECT

public:

    enum CurrentAction3d {	CurAction3d_Undefined,
                            CurAction3d_Nothing, 
                            CurAction3d_Picking,
                            CurAction3d_DynamicZooming,
                            CurAction3d_WindowZooming, 
                            CurAction3d_DynamicPanning,
                            CurAction3d_GlobalPanning, 
                            CurAction3d_DynamicRotation };
/*
    enum ViewAction {	ViewFitAllId, 
                        ViewFitAreaId, 
                        ViewZoomId, 
                        ViewPanId, 
                        ViewGlobalPanId,
                        ViewFrontId, 
                        ViewBackId, 
                        ViewTopId, 
                        ViewBottomId, 
                        ViewLeftId, 
                        ViewRightId,
                        ViewAxoId, 
                        ViewRotationId, 
                        ViewResetId, 
                        ViewHlrOffId, 
                        ViewHlrOnId };
*/
public:

    QoccViewWidget(  const QoccViewerContext* aContext = NULL, 
                     QWidget *parent = NULL, 
                     Qt::WindowFlags wflags = 0 );

    ~QoccViewWidget();

    inline Handle_AIS_InteractiveContext&	getContext( void ) { return myContext->getContext(); }
    inline Handle_V3d_Viewer&				getViewer( void )  { return myContext->getViewer(); }
    inline Handle_V3d_View&					getView( void )    { return myView; }

    inline V3d_Coordinate x( void ) { return myV3dX; }
    inline V3d_Coordinate y( void ) { return myV3dY; }
    inline V3d_Coordinate z( void ) { return myV3dZ; }

    inline Qt::MouseButtons      buttons()  const { return myMouseState; }
    inline Qt::MouseButtons      button()   const { return myMouseFlags; }
    inline Qt::KeyboardModifiers keyboard() const { return	myKeyboardFlags; }
    inline QPoint getCurrentPoint( void ) { return myCurrentPoint; }

    void setStatusMessage (const QString& aString);

    // Overrides
    QPaintEngine* paintEngine() const;

    // Accessors
    gp_Dir getDirection();
    gp_Pln getPlane();
    gp_Pnt getPoint();
    
    void redraw( bool isPainting = false );

signals:

    void initialized();
    void selectionChanged();
    void mouseMoved   ( QoccViewWidget* widget );
    void mouseClicked ( QoccViewWidget* widget );
    void sendStatus   ( const QString aMessage );
    //! Just a placeholder for now
    void popupMenu ( const QoccViewWidget* aView, const QPoint aPoint ); 
    void error ( int errorCode, QString& errorDescription );

public slots:

    void idle();
    void fitExtents();
    void fitAll();
    void fitArea();
    void zoom();
    void pan();
    void globalPan();
    void rotation();
    void hiddenLineOn();
    void hiddenLineOff();
    void background();
    void viewFront();
    void viewBack();
    void viewTop();
    void viewBottom();
    void viewLeft();
    void viewRight();
    void viewAxo();
    void viewTopFront();
    void viewGrid();
    void viewReset();
    void setReset();

    void newShape();
    void refresh();

protected: // methods

    void initializeOCC();

    virtual void paintEvent        ( QPaintEvent* e );
    virtual void resizeEvent       ( QResizeEvent* e );
    virtual void mousePressEvent   ( QMouseEvent* e );
    virtual void mouseReleaseEvent ( QMouseEvent* e );
    virtual void mouseMoveEvent    ( QMouseEvent* e );
    virtual void wheelEvent        ( QWheelEvent* e );

    virtual void leaveEvent		   ( QEvent * );

private: // members

    Handle_WNT_Window				myWindow;

    Handle_V3d_View                 myView;
    QoccViewerContext*				myContext;
                    
    Standard_Boolean	  myViewResized;
    Standard_Boolean	  myViewInitialized;
    CurrentAction3d       myMode;
    Quantity_Factor       myCurZoom;
    Standard_Boolean	  myGridSnap;
    AIS_StatusOfDetection myDetection;

    V3d_Coordinate        myV3dX;
    V3d_Coordinate        myV3dY;
    V3d_Coordinate        myV3dZ;
        
    QRubberBand*          myRubberBand;
    QPoint		          myStartPoint;
    QPoint		          myCurrentPoint;
    
    Standard_Real	      myPrecision;
    Standard_Real	      myViewPrecision;
    Standard_Boolean      myMapIsValid;

    Qt::KeyboardModifiers myKeyboardFlags;
    Qt::MouseButton		  myMouseFlags;
    Qt::MouseButtons	  myMouseState;
    QCursor				  myCrossCursor;
    QToolBar*			  myToolBar;
    
    // Shared message
    static QString		  myStatusMessage;

private: // methods

    void onLeftButtonDown  ( Qt::KeyboardModifiers nFlags, const QPoint point );
    void onMiddleButtonDown( Qt::KeyboardModifiers nFlags, const QPoint point );
    void onRightButtonDown ( Qt::KeyboardModifiers nFlags, const QPoint point );
    void onLeftButtonUp    ( Qt::KeyboardModifiers nFlags, const QPoint point );
    void onMiddleButtonUp  ( Qt::KeyboardModifiers nFlags, const QPoint point );
    void onRightButtonUp   ( Qt::KeyboardModifiers nFlags, const QPoint point );
    
    void onMouseMove  ( Qt::MouseButtons buttons, 
                        Qt::KeyboardModifiers nFlags, const QPoint point );

    AIS_StatusOfPick		dragEvent ( const QPoint startPoint, const QPoint endPoint, const bool multi = false );
    AIS_StatusOfPick		inputEvent( const bool multi = false );
    AIS_StatusOfDetection	moveEvent ( const QPoint point );
    
    void setMode( const CurrentAction3d mode );
    
    Standard_Real precision( Standard_Real aReal );
    Standard_Real viewPrecision( bool resized = false );

    void drawRubberBand( const QPoint origin, const QPoint position );
    void showRubberBand( void );
    void hideRubberBand( void );

    void sendStatusLocation ( void );

    Standard_Boolean convertToPlane(const Standard_Integer Xs, 
                                    const Standard_Integer Ys, 
                                          Standard_Real& X,
                                          Standard_Real& Y,
                                          Standard_Real& Z);
                                          
    void paintOCC();
#if 0
    static int paintCallBack (Aspect_Drawable, 
                              void*, 
                              Aspect_GraphicCallbackStruct*);
#endif

public:

    bool dump(Standard_CString theFile);

};

#endif // QoccViewWidget_H
