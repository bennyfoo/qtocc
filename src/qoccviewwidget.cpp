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
\class	QoccViewWidget
\brief	This class provides the view element of the QtOpenCascade Toolkit.
        This class provides a Widget to view OpenCASCADE objects within a Qt4
        hosted application.
\author	Peter C. Dolbey
*/
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

#include <QApplication>
#include <QBitmap>
#include <QtGui/QPainter>
#include <QtGui/QInputEvent>
#include <QColorDialog>
//#include <QPlastiqueStyle>
#include <QtCore/QTextStream>

#include "qoccviewwidget.h"
#include "qoccinternal.h"

QString	QoccViewWidget::myStatusMessage = QObject::tr("");

/*!
\brief	Constructor for QoccViewWidget.
        This constructor creates an uninitialised window for rendering a
        Handle_V3d_View based OpenCASCADE 3D viewer.
\param	aContext The widget's Qocc context
\param	parent The widget's parent widget
\param	wflags The widget's window configuration flags
\see	initialize
*/
QoccViewWidget::QoccViewWidget( const QoccViewerContext* aContext,
                                QWidget *parent, Qt::WindowFlags f )
: QWidget( parent, f | Qt::MSWindowsOwnDC )
{
    // Intialise mamber variables,
    // gcc should stop complaining!
    myRubberBand      = NULL;
    myMode            = CurAction3d_Undefined;
    myGridSnap        = Standard_False;
    myViewResized     = Standard_False;
    myViewInitialized = Standard_False;
    myPrecision       = INITIAL_PRECISION;
    myViewPrecision   = 0.0;
    myDetection       = AIS_SOD_Nothing;
    myKeyboardFlags   = Qt::NoModifier;
    myMouseFlags     = Qt::NoButton;

    myContext = (QoccViewerContext *) aContext;

    myView = myContext->getViewer()->CreateView();
    myView->SetScale( 2 );

    // Needed to generate mouse events
    setMouseTracking( true );

    // Avoid Qt background clears to improve resizing speed,
    // along with a couple of other attributes
    setAutoFillBackground( false );
    setAttribute( Qt::WA_NoSystemBackground );

    // This next attribute seems to be the secret of allowing OCC on Win32
    // to "own" the window, even though its only supposed to work on X11.
    setAttribute( Qt::WA_PaintOnScreen );

    // Here's a modified pick point cursor from AutoQ3D
    QBitmap curb1( 48, 48 );
    QBitmap curb2( 48, 48 );
    curb1.fill( QColor( 255, 255, 255 ) );
    curb2.fill( QColor( 255, 255, 255 ) );
    QPainter p;

    p.begin( &curb1 );
        p.drawLine( 24,  0, 24, 47 );
        p.drawLine(  0, 24, 47, 24 );
        p.setBrush( Qt::NoBrush );
        p.drawRect( 18, 18, 12, 12 );
    p.end();
    myCrossCursor = QCursor( curb2, curb1, 24, 24 );

    // Create a rubber band box for later mouse activity
    myRubberBand = new QRubberBand( QRubberBand::Rectangle, this );
    if (myRubberBand)
    {
        // If you don't set a style, QRubberBand doesn't work properly
        // take this line out if you don't believe me.
      //  myRubberBand->setStyle( (QStyle*) new QPlastiqueStyle() );
    }
}

/*!
\brief	Default destructor for QoccViewWidget.
        This should delete any memory and release any resources. No parameters
        required.
*/
QoccViewWidget::~QoccViewWidget()
{
    if ( myRubberBand )
    {
        delete myRubberBand;
    }
}

/*!
\brief	The initializeOCC() procedure.
        This function creates the widget's view using the interactive context
        provided. Currently it also creates a trihedron object in the lower left
        corner - this will eventually be managed by an external system setting.
\param	aContext Handle to the AIS Interactive Context managing the view
\return	nothing
*/
void QoccViewWidget::initializeOCC()
{
    Aspect_RenderingContext rc = 0;
    //myContext = aContext;

    WId windowHandle = (WId) winId();

    // rc = (Aspect_RenderingContext) wglGetCurrentContext();
    myWindow = new WNT_Window((Aspect_Handle)windowHandle);
    // Turn off background erasing in OCC's window
  //  myWindow->SetFlags( WDF_NOERASEBKGRND );

    if (!myView.IsNull())
    {
        // Set my window (Hwnd) into the OCC view
       // myView->SetWindow( myWindow, rc , paintCallBack, this  );
        // Set up axes (Trihedron) in lower left corner.
//		myView->SetScale( 2 );			// Choose a "nicer" intial scale

        // Set up axes (Trihedron) in lower left corner.
#ifdef OCC_PATCHED
        myView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_ZBUFFER );
#else
        myView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_WIREFRAME );
#endif
        myView->SetWindow(myWindow);
        // For testing OCC patches
        // myView->ColorScaleDisplay();
        // Map the window
        if (!myWindow->IsMapped())
        {
            myWindow->Map();
        }
        // Force a redraw to the new window on next paint event
        myViewResized = Standard_True;

        // This is to signal any connected slots that the view is ready.
        myViewInitialized = Standard_True;

        // Set default cursor as a cross
        setMode( CurAction3d_Nothing );

        emit initialized();
    }
}

gp_Dir QoccViewWidget::getDirection()
{
    return myContext->getViewer()->PrivilegedPlane().Direction();
}

gp_Pln QoccViewWidget::getPlane()
{
    return myContext->getViewer()->PrivilegedPlane();
}

gp_Pnt QoccViewWidget::getPoint()
{
    gp_Pnt thePoint( x(), y(), z() );
    return thePoint;
}

/*!
\brief	Returns a NULL QPaintEngine
        This should result in a minor performance benefit.
*/
QPaintEngine* QoccViewWidget::paintEngine() const
{
    return NULL;
}

/*!
\brief	Paint Event
        Called when the Widget needs to repaint itself
\param	e The event data.
*/
void QoccViewWidget::paintEvent ( QPaintEvent * /* e */)
{
    if ( !myViewInitialized )
    {
        if ( winId() )
        {
            initializeOCC( );
        }
    }
    if ( !myView.IsNull() )
    {
        redraw( true );
    }
}

/*!
\brief	Resize event.
        Called when the Widget needs to resize itself, but seeing as a paint
        event always follows a resize event, we'll move the work into the
        paint event.
\param	e The event data.
*/
void QoccViewWidget::resizeEvent ( QResizeEvent * /* e */ )
{
    myViewResized = Standard_True;
}

/*!
\brief	Mouse press event
\param	e The event data.
*/
void QoccViewWidget::mousePressEvent( QMouseEvent* e )
{
    myMouseFlags = e->button();
    myMouseState = e->buttons();

    // Cache the keyboard flags for the whole gesture
    myKeyboardFlags = e->modifiers();

    // The button mappings can be used as a mask. This code prevents conflicts
    // when more than one button pressed simutaneously.
    if ( e->button() & Qt::LeftButton )
    {
        onLeftButtonDown  ( myKeyboardFlags, e->pos() );
    }
    else if ( e->button() & Qt::RightButton )
    {
        onRightButtonDown ( myKeyboardFlags, e->pos() );
    }
    else if ( e->button() & Qt::MidButton )
    {
        onMiddleButtonDown( myKeyboardFlags, e->pos() );
    }
}

/*!
\brief	Mouse release event
\param	e The event data.
*/
void QoccViewWidget::mouseReleaseEvent(QMouseEvent* e)
{
    myMouseFlags = e->button();
    myMouseState = e->buttons();
    redraw();							// Clears up screen when menu selected but not used.
    hideRubberBand();
    if ( e->button() & Qt::LeftButton )
    {
        onLeftButtonUp  ( myKeyboardFlags, e->pos() );
    }
    else if ( e->button() & Qt::RightButton )
    {
        onRightButtonUp ( myKeyboardFlags, e->pos() );
    }
    else if ( e->button() & Qt::MidButton )
    {
        onMiddleButtonUp( myKeyboardFlags, e->pos() );
    }

    if ( !(myKeyboardFlags & CASCADESHORTCUTKEY) )
    {
        emit mouseClicked( this );
    }
}

/*!
\brief	Mouse move event, driven from application message loop
\param	e The event data.
*/
void QoccViewWidget::mouseMoveEvent( QMouseEvent* e )
{
    Standard_Real X, Y, Z;

    myMouseFlags = e->button();
    myMouseState = e->buttons();

    myCurrentPoint = e->pos();
    //Check if the grid is active and that we're snapping to it
    if( myContext->getViewer()->Grid()->IsActive() && myGridSnap )
    {
        myView->ConvertToGrid( myCurrentPoint.x(), myCurrentPoint.y(), myV3dX, myV3dY, myV3dZ );
        emit mouseMoved( this );
        sendStatusLocation();
    }
    else //	this is the standard case
    {
        if ( convertToPlane( myCurrentPoint.x(), myCurrentPoint.y(), X, Y, Z ) )
        {
            myV3dX = precision( X );
            myV3dY = precision( Y );
            myV3dZ = precision( Z );
            emit mouseMoved( this );
            sendStatusLocation();
        }
        else
        {
            emit sendStatus ( tr("Indeterminate Point") );
        }
    }
    onMouseMove( e->buttons(), myKeyboardFlags, e->pos() );
}

/*!
\brief	A leave event is sent to the widget when the mouse cursor leaves
        the widget.
        This sub-classed event handler fixes redraws when gestures are interrupted
        by use of parent menus etc. (Candidate for change)
\param	e	The event data
*/
void QoccViewWidget::leaveEvent ( QEvent* )
{
    myMouseFlags = Qt::NoButton;
}

/*!
\brief	The QWheelEvent class contains parameters that describe a wheel event.
*/
void QoccViewWidget::wheelEvent ( QWheelEvent* e )
{
    if ( !myView.IsNull() )
    {
        Standard_Real currentScale = myView->Scale();
        if ( e->delta() > 0 )
        {
            currentScale *= 1.10; // +10%
        }
        else
        {
            currentScale /= 1.10; // -10%
        }
        myView->SetScale( currentScale );
        viewPrecision( true );

    }
    else
    {
        e->ignore();
    }
}

/*!
\brief	Go idle
        This called from various locations, and also exposed as a slot.
*/
void QoccViewWidget::idle( )
{
    setMode( CurAction3d_Nothing );
}
/*!
\brief	The main redraw function
        This called from various locations.
*/
void QoccViewWidget::redraw( bool isPainting )
{
    if ( !myView.IsNull() )					// Defensive test.
    {
        if ( myViewResized )
        {
            myView->MustBeResized();
            viewPrecision( true );
        }
        else
        {
            // Don't repaint if we are already redrawing
            // elsewhere due to a keypress or mouse gesture
            if ( !isPainting || ( isPainting && myMouseFlags == Qt::NoButton ) )
            {
                myView->Redraw();
            }
        }
    }
    myViewResized = Standard_False;
}

/*!
\brief	Just fits the current window
        This function just fits the current objects to the window, without
        either reducing or increasing the Z extents. This can cause clipping
        of the objects after rotation.
\return	Nothing
*/
void QoccViewWidget::fitExtents( void )
{
    if (!myView.IsNull())
    {
        myView->FitAll();
        viewPrecision( true );
    }
}

/*!
\brief	Fits the model to view extents
        This function fits the current objects to the window,
        reducing or increasing the Z extents as needed.
\return	Nothing
*/
void QoccViewWidget::fitAll( void )
{
    if (!myView.IsNull())
    {
        myView->ZFitAll();
        myView->FitAll();
        viewPrecision( true );
    }
}

//-----------------------------------------------------------------------------
/*!
\brief	Sets up the view for a rubberband window zoom
\return	Nothing
*/
void QoccViewWidget::fitArea( void )
{
    setMode( CurAction3d_WindowZooming );
}

/*!
\brief	Sets up the view for a dynamic zoom
\return	Nothing
*/
void QoccViewWidget::zoom( void )
{
    setMode( CurAction3d_DynamicZooming );
}

/*!
\brief	Sets up the view for panning
\return	Nothing
*/
void QoccViewWidget::pan( void )
{
    setMode( CurAction3d_DynamicPanning );
}

/*!
\brief	Sets up the view for dynamic rotation
\return	Nothing
*/
void QoccViewWidget::rotation( void )
{
    setMode( CurAction3d_DynamicRotation );
}

/*!
\brief	Sets up the view for global panning, whatever that is!
\return	Nothing
*/
void QoccViewWidget::globalPan()
{
    if ( !myView.IsNull() )
    {
        // save the current zoom value
        myCurZoom = myView->Scale();
        // Do a Global Zoom
        myView->FitAll();
        viewPrecision( true );
        // Set the mode
        setMode( CurAction3d_GlobalPanning );
    }
}

/*!
\brief	This aligns the view to the current privilege plane.
        Not so interesting at the moment, but wait until custom
        grid planes are implemented!
\return	Nothing
*/
void QoccViewWidget::viewGrid()
{
    if( !myView.IsNull() )
    {
        myView->SetFront();
    }
}

/*!
\brief	View from canonical "front".
\return	Nothing
*/
void QoccViewWidget::viewFront()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_Yneg );
    }
}
/*!
\brief	View from canonical "back".
\return	Nothing
*/
void QoccViewWidget::viewBack()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_Ypos );
    }
}
/*!
\brief	View from canonical "top".
        This is traditional XOY axis.
\return	Nothing
*/
void QoccViewWidget::viewTop()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_Zpos );
    }
}

/*!
\brief	View from canonical "bottom".
\return	Nothing
*/
void QoccViewWidget::viewBottom()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_Zneg );
    }
}
/*!
\brief	View from canonical "left".
\return	Nothing
*/
void QoccViewWidget::viewLeft()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_Xneg );
    }
}
/*!
\brief	View from canonical "right".
\return	Nothing
*/
void QoccViewWidget::viewRight()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_Xpos );
    }
}
/*!
\brief	View using axonometric projection.
\return	Nothing
*/

void QoccViewWidget::viewAxo()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_XnegYnegZpos );
    }
}

void QoccViewWidget::viewTopFront()
{
    if( !myView.IsNull() )
    {
        myView->SetProj( V3d_YnegZpos );
    }
}


void QoccViewWidget::viewReset()
{
    if( !myView.IsNull() )
    {
        myView->Reset();
    }
}

void QoccViewWidget::hiddenLineOff()
{
    if( !myView.IsNull() )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        myView->SetComputedMode( Standard_False );
        QApplication::restoreOverrideCursor();
    }
}

void QoccViewWidget::hiddenLineOn()
{
    if( !myView.IsNull() )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        myView->SetComputedMode( Standard_True );
        QApplication::restoreOverrideCursor();
    }
}

void QoccViewWidget::background()
{
    QColor aColor ;
    Standard_Real R1;
    Standard_Real G1;
    Standard_Real B1;
    myView->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    aColor.setRgb(( int )R1*255, ( int )G1*255, ( int )B1*255);

    QColor aRetColor = QColorDialog::getColor(aColor);

    if( aRetColor.isValid() )
    {
        R1 = aRetColor.red()/255.;
        G1 = aRetColor.green()/255.;
        B1 = aRetColor.blue()/255.;
        myView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    }
    redraw();
}

void QoccViewWidget::setReset ()
{
    if( !myView.IsNull() )
    {
        myView->SetViewOrientationDefault() ;
        viewPrecision( true );
    }
}

/*!
\brief	This function handles left button down events from the mouse.
*/
void QoccViewWidget::onLeftButtonDown(  Qt::KeyboardModifiers nFlags, const QPoint point )
{
    myStartPoint = point;
    if ( nFlags & CASCADESHORTCUTKEY )
    {
        setMode( CurAction3d_DynamicZooming );
    }
    else
    {
        switch ( myMode )
        {
            case CurAction3d_Nothing:
                // emit pointClicked( myV3dX, myV3dY, myV3dZ );
                break;

            case CurAction3d_Picking:
                break;

            case CurAction3d_DynamicZooming:
                break;

            case CurAction3d_WindowZooming:
                break;

            case CurAction3d_DynamicPanning:
                break;

            case CurAction3d_GlobalPanning:
                break;

            case CurAction3d_DynamicRotation:
                myView->StartRotation( myStartPoint.x(), myStartPoint.y() );
                break;

            default:
                Standard_Failure::Raise( "Incompatible Current Mode" );
                break;
        }
    }
}

/*!
\brief	This function handles middle button down events from the mouse.
*/
void QoccViewWidget::onMiddleButtonDown(  Qt::KeyboardModifiers nFlags, const QPoint point )
{
    myStartPoint = point;
    if ( nFlags & CASCADESHORTCUTKEY )
    {
        setMode( CurAction3d_DynamicPanning );
    }
}

/*!
\brief	This function handles right button down events from the mouse.
*/
void QoccViewWidget::onRightButtonDown(  Qt::KeyboardModifiers nFlags, const QPoint point )
{
    myStartPoint = point;
    if ( nFlags & CASCADESHORTCUTKEY )
    {
        setMode( CurAction3d_DynamicRotation );
        myView->StartRotation( point.x(), point.y() );
    }
    else
    {
        emit popupMenu ( this, point );
    }
}

/*!
\brief	This function handles left button up events from the mouse.
        This marks the end of the gesture.
*/
void QoccViewWidget::onLeftButtonUp(  Qt::KeyboardModifiers nFlags, const QPoint point )
{
    myCurrentPoint = point;
    if ( nFlags & CASCADESHORTCUTKEY )
    {
        // Deactivates dynamic zooming
        setMode( CurAction3d_Nothing );
    }
    else
    {
        switch( myMode )
        {

            case CurAction3d_Nothing:
                //break;

            case CurAction3d_Picking:
                // Shouldn't get here yet
                if ( myCurrentPoint == myStartPoint )
                {
                    inputEvent( nFlags & MULTISELECTIONKEY );
                }
                else
                {
                    dragEvent( myStartPoint, myCurrentPoint, nFlags & MULTISELECTIONKEY );
                }
                break;

            case CurAction3d_DynamicZooming:
                viewPrecision( true );
                break;

            case CurAction3d_WindowZooming:
                if ( (abs( myCurrentPoint.x() - myStartPoint.x() ) > ValZWMin ) ||
                     (abs( myCurrentPoint.y() - myStartPoint.y() ) > ValZWMin ) )
                {
                    myView->WindowFitAll( myStartPoint.x(),
                                          myStartPoint.y(),
                                          myCurrentPoint.x(),
                                          myCurrentPoint.y() );
                }
                viewPrecision( true );
                break;

            case CurAction3d_DynamicPanning:
                break;

            case CurAction3d_GlobalPanning :
                myView->Place( myCurrentPoint.x(), myCurrentPoint.y(), myCurZoom );
                break;

            case CurAction3d_DynamicRotation:
                break;

            default:
                Standard_Failure::Raise(" Incompatible Current Mode ");
                break;
        }
    }
    emit selectionChanged();
}
/*!
\brief	Middle button up event handler.
        This marks the end of the gesture.
*/
void QoccViewWidget::onMiddleButtonUp(  Qt::KeyboardModifiers /* nFlags */, const QPoint /* point */ )
{
    setMode( CurAction3d_Nothing );
}

/*!
\brief	Right button up event handler.
        This marks the end of the gesture.
*/
void QoccViewWidget::onRightButtonUp(  Qt::KeyboardModifiers nFlags, const QPoint point )
{
    myCurrentPoint = point;
    if ( nFlags & CASCADESHORTCUTKEY )
    {
        setMode( CurAction3d_Nothing );
    }
    else
    {
        if ( myMode == CurAction3d_Nothing )
        {
            emit popupMenu ( this, point );
        }
        else
        {
            setMode( CurAction3d_Nothing );
        }
    }
}

/*!
\brief	Mouse move event handler.
\param	buttons
\param	nFlags
\param	point
\return Nothing
*/
void QoccViewWidget::onMouseMove( Qt::MouseButtons buttons,
                                  Qt::KeyboardModifiers nFlags,
                                  const QPoint point )
{
    myCurrentPoint = point;

    if ( buttons & Qt::LeftButton  || buttons & Qt::RightButton || buttons & Qt::MidButton )
    {
        switch ( myMode )
        {
            case CurAction3d_Nothing:
                // break;

            case CurAction3d_Picking:
                // Shouldn't get here yet
                drawRubberBand ( myStartPoint, myCurrentPoint );
                dragEvent( myStartPoint, myCurrentPoint, nFlags & MULTISELECTIONKEY );
                break;

            case CurAction3d_DynamicZooming:
                myView->Zoom(	myStartPoint.x(),
                                myStartPoint.y(),
                                myCurrentPoint.x(),
                                myCurrentPoint.y() );
                viewPrecision( true );
                myStartPoint = myCurrentPoint;
                break;

            case CurAction3d_WindowZooming:
                drawRubberBand ( myStartPoint, myCurrentPoint );
                break;

            case CurAction3d_DynamicPanning:
                myView->Pan( myCurrentPoint.x() - myStartPoint.x(),
                             myStartPoint.y() - myCurrentPoint.y() );
                myStartPoint = myCurrentPoint;
                break;

            case CurAction3d_GlobalPanning:
                break;

            case CurAction3d_DynamicRotation:
                myView->Rotation( myCurrentPoint.x(), myCurrentPoint.y() );
                break;

            default:
                Standard_Failure::Raise( "Incompatible Current Mode" );
                break;
        }
    }
    else
    {
        moveEvent( myCurrentPoint );
    }
}
/*!
\brief	Move event detection handler
*/
AIS_StatusOfDetection QoccViewWidget::moveEvent( QPoint point )
{
    AIS_StatusOfDetection status;
    status = myContext->getContext()->MoveTo( point.x(), point.y(), myView ,Standard_True);
    return status;
}

/*!
\brief	Drag event handler.
\param	startPoint	The gesture start point.
\param	endPoint	The gesture end point.
\param	multi		Allows selection of multiple objects.
\return The status of pick.
*/
AIS_StatusOfPick QoccViewWidget::dragEvent( const QPoint startPoint, const QPoint endPoint, const bool multi )
{
    AIS_StatusOfPick pick = AIS_SOP_NothingSelected;
    if (multi)
    {
        pick = myContext->getContext()->ShiftSelect( min (startPoint.x(), endPoint.x()),
                                                     min (startPoint.y(), endPoint.y()),
                                                     max (startPoint.x(), endPoint.x()),
                                                     max (startPoint.y(), endPoint.y()),
                                                     myView,Standard_True);
    }
    else
    {
        pick = myContext->getContext()->Select( min (startPoint.x(), endPoint.x()),
                                                min (startPoint.y(), endPoint.y()),
                                                max (startPoint.x(), endPoint.x()),
                                                max (startPoint.y(), endPoint.y()),
                                                myView,Standard_True );
    }
    emit selectionChanged();
    return pick;
}
/*!
\brief	This handles object highlighting during movement of the mouse across the view.
\param	multi	Selects multiple objects if true (default false).
\return The status of the objects under the cursor
*/
AIS_StatusOfPick QoccViewWidget::inputEvent( bool multi )
{
    AIS_StatusOfPick pick = AIS_SOP_NothingSelected;

    if (multi)
    {
        pick = myContext->getContext()->ShiftSelect(Standard_True);
    }
    else
    {
        pick = myContext->getContext()->Select(Standard_True);
    }
    if ( pick != AIS_SOP_NothingSelected )
    {
        emit selectionChanged();
    }
    return pick;
}

bool QoccViewWidget::dump(Standard_CString theFile)
{
  redraw();
  return myView->Dump(theFile);
}

/*!
    \brief This function sets the current cursor for the given interraction mode.
    \param mode		The interraction mode
*/
void QoccViewWidget::setMode( const CurrentAction3d mode )
{
    if ( mode != myMode )
    {
        switch( mode )
        {
            case CurAction3d_DynamicPanning:
                setCursor( Qt::SizeAllCursor );
                break;
            case CurAction3d_DynamicZooming:
                setCursor( Qt::CrossCursor );
                break;
            case CurAction3d_DynamicRotation:
                setCursor( Qt::CrossCursor );
                break;
            case CurAction3d_GlobalPanning:
                setCursor( Qt::CrossCursor );
                break;
            case CurAction3d_WindowZooming:
                setCursor( Qt::PointingHandCursor );
                break;
            case CurAction3d_Nothing:
                //setCursor( myCrossCursor );
                setCursor( Qt::CrossCursor );
                break;
            default:
                setCursor( Qt::ArrowCursor );
                break;
        }
        myMode = mode;
    }
}

/*!
    \brief This is a Utility function for rounding the input value to a specific DP
*/
Standard_Real QoccViewWidget::precision( Standard_Real aReal )
{
    Standard_Real preciseReal;
    Standard_Real thePrecision = max (myPrecision, viewPrecision());

    if ( myPrecision != 0.0 )
    {
        preciseReal =  SIGN(aReal) * floor((std::abs(aReal) + thePrecision * 0.5) / thePrecision) * thePrecision;
    }
    else
    {
        preciseReal = aReal;
    }
    return preciseReal;
}

/*! ------------------------------------------------------------------------------------
\brief	ConvertToPlane convert 2d window position to 3d point on priviledged plane.
        This routine was provided by Matra Datavision during Foundation training.
        There still appears to be a pixel error in y co-ordinate transforms.
\param	Xs				The screen's x co-ordinate (in)
\param	Ys				The screen's y co-ordinate (in)
\param	X				The output x position on the privileged plane (out)
\param	Y				The output y position on the privileged plane (out)
\param	Z				The output z position on the privileged plane (out)
\return	Standard_Boolean indicating success or failure
*/
Standard_Boolean QoccViewWidget::convertToPlane(const Standard_Integer Xs,
                                                 const Standard_Integer Ys,
                                                 Standard_Real& X,
                                                 Standard_Real& Y,
                                                 Standard_Real& Z)
{
    Standard_Real Xv, Yv, Zv;
    Standard_Real Vx, Vy, Vz;
    gp_Pln aPlane(myView->Viewer()->PrivilegedPlane());

#ifdef OCC_PATCHED
    myView->Convert( Xs, Ys, Xv, Yv, Zv );
#else
    // The + 1 overcomes a fault in OCC, in "OpenGl_togl_unproject_raster.c",
    // which transforms the Y axis ordinate. The function uses the height of the
    // window, not the Y maximum which is (height - 1).
    myView->Convert( Xs, Ys + 1, Xv, Yv, Zv );
#endif

    myView->Proj( Vx, Vy, Vz );
    gp_Lin aLine(gp_Pnt(Xv, Yv, Zv), gp_Dir(Vx, Vy, Vz));
    IntAna_IntConicQuad theIntersection( aLine, aPlane, Precision::Angular() );
    if (theIntersection.IsDone())
    {
        if (!theIntersection.IsParallel())
        {
            if (theIntersection.NbPoints() > 0)
            {
                gp_Pnt theSolution(theIntersection.Point(1));
                X = theSolution.X();
                Y = theSolution.Y();
                Z = theSolution.Z();
                return Standard_True;
            }
        }
    }
    return Standard_False;
}
/*!
\brief	Draws the rubberband box
        This function is designed to reduce "flicker" as the box is redrawn,
        especially when the origin in the bottom corner of the window
\param	origin		A QPoint defining the screen origin
\param	position	A QPoint defining the current cursor screen location
*/
void QoccViewWidget::drawRubberBand( const QPoint origin, const QPoint position )
{
    if ( myRubberBand )
    {
        redraw();
        hideRubberBand();
        myRubberBand->setGeometry( QRect( origin, position ).normalized() );
        showRubberBand();
    }
}
/*!
\brief	Shows the rubberband box
*/
void QoccViewWidget::showRubberBand( void )
{
    if ( myRubberBand )
    {
        myRubberBand->show();
    }
}
/*!
\brief	Hides the rubberband box
*/
void QoccViewWidget::hideRubberBand( void )
{
    if ( myRubberBand )
    {
        myRubberBand->hide();
    }
}
/*!
\brief	Static OpenCascade callback proxy
*/
#if 0
int QoccViewWidget::paintCallBack (Aspect_Drawable /* drawable */,
                                  void* aPointer,
                                  Aspect_GraphicCallbackStruct* /* data */)
{
  QoccViewWidget *aWidget = (QoccViewWidget *) aPointer;
  aWidget->paintOCC();
  return 0;
}
#endif
/*!
\brief	The OpenGL paint routine in the callback.
*/
void QoccViewWidget::paintOCC( void )
{
    glDisable( GL_LIGHTING );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    GLfloat left   = -1.0f;
    GLfloat right  =  1.0f;
    GLfloat bottom = -1.0f;
    GLfloat top    =  1.0f;
    GLfloat depth  =  1.0f;

    glOrtho( left, right, bottom, top, 1.0, -1.0 );

#ifndef OCC_PATCHED
    glEnable(GL_BLEND);
#if 0
    if (myView->ColorScaleIsDisplayed())
    {
        // Not needed on patched OCC 6.2 versions, but is the lowest
        // common denominator working code on collaborators OpenGL
        // graphics cards.
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    }
#endif
#endif

    glBegin( GL_QUADS);
    {
      glColor4f  (  0.1f, 0.1f, 0.1f, 1.0f );
      glVertex3d (  left, bottom, depth );
      glVertex3d ( right, bottom, depth );
      glColor4f  (  0.8f, 0.8f, 0.9f, 1.0f );
      glVertex3d ( right,    top, depth );
      glVertex3d (  left,    top, depth );
    }
    glEnd();

    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

}
/*!
\brief	This routine calculates the minimum sensible precision for the point
        selection routines, by setting an minumum resolution to a decade one
        higher than the equivalent pixel width.
\param	resized		Indicates that recaculation os required due to state
                    changes in the view.
*/
Standard_Real QoccViewWidget::viewPrecision( bool resized )
{

    Standard_Real X1, Y1, Z1;
    Standard_Real X2, Y2, Z2;
    if (!myWindow.IsNull())
    {
        if (resized || myViewPrecision == 0.0)
        {
            myView->Convert( 0, 0, X1, Y1, Z1 );
            myView->Convert( 1, 0, X2, Y2, Z2 );
            Standard_Real pixWidth = std::sqrt(  std::pow( X2 - X1, 2 )
                                          + std::pow( Y2 - Y1, 2 )
                                          + std::pow( Z2 - Z1, 2 ) ) ;
            if ( pixWidth != 0.0 )
            {
                // Return the precision as the next highest decade above the pixel width
                myViewPrecision = std::pow( 10.0, std::floor( std::log10( pixWidth ) + 1.0 ) );
            }
            else
            {
                // Return the user precision if window not defined
                myViewPrecision = myPrecision;
            }
        }
    }
    return myViewPrecision;
}

void QoccViewWidget::newShape()
{
    // Model changed
    fitAll();
}

void QoccViewWidget::refresh()
{
    // Model changed
    redraw( false );
}

void QoccViewWidget::setStatusMessage (const QString& aString)
{
    myStatusMessage = aString;
    emit sendStatus(aString);
}

void QoccViewWidget::sendStatusLocation ()
{
    QString aString;
    QTextStream ts(&aString);
    ts << myStatusMessage << "(" << x() << "," << y() << "," << z() << ")";
    emit sendStatus(aString);
}