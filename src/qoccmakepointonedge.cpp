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
\class	QoccPointOnEdgeDlg
\brief	This class provides the keyboard entry for the circle-centre command
\author	Peter C. Dolbey
*/
#include <QAction>
#include <QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>

#include <qnamespace.h>
#include "qoccinternal.h"
#include "qoccviewwidget.h"
#include "qoccmakepointonedge.h"
#include "GeomAPI_ProjectPointOnCurve.hxx" //where should this go? opencascade.h?
#include "ProjLib.hxx"
#include "ElSLib.hxx"
#include "BRepBndLib.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"

QoccPointOnEdgeDlg::QoccPointOnEdgeDlg( )
{

}

QoccPointOnEdgeDlg::~QoccPointOnEdgeDlg( )
{
}

/*!
\class	QoccMakePointOnEdge
\brief	This class provides the interactive entry for the vertexOnEdge command
\author	Peter C. Dolbey
*/

QoccMakePointOnEdge::QoccMakePointOnEdge( QAction* startingAction, QWidget* parent )
:QoccCommand( startingAction, parent )
{
// 	parameter = Centre;
    edge.Nullify();
    aisPoint.Nullify();
    parameter = Selection;
    if ( myAction )
    {
        myAction->setDisabled(true);
    }
    QBitmap curb1( 1, 1 );
    QBitmap curb2( 1, 1 );
    curb1.fill( QColor( 255, 255, 255 ) );
    curb2.fill( QColor( 255, 255, 255 ) );
    QPainter p;

    p.begin( &curb1 );
    p.setBrush( Qt::NoBrush );
    p.end();
    myInvisibleCursor = QCursor( curb2, curb1, 1, 1 );
    edgeLength=0.0;
}

QoccMakePointOnEdge::~QoccMakePointOnEdge( )
{
    if ( myAction )
    {
        myAction->setDisabled(false);
    }
}

/*
TopoDS_Shape QoccMakePointOnEdge::buildCircle( QoccViewWidget* widget )
{
    gp_Pnt p(widget->x(), widget->y(), widget->z());
    dRadius = p.Distance(ptCentre);
    gp_Ax2 aix(ptCentre, dirNormal);
    gp_Circ circle(aix, dRadius);
    BRepBuilderAPI_MakeEdge builder(circle);
    TopoDS_Shape shape = builder.Shape();
    return shape;
}
*/
// from http://www.opencascade.org/org/forum/thread_3185/
gp_Pnt ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle_V3d_View aView)
{
     
    V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
    aView->Eye(XEye,YEye,ZEye);
    aView->At(XAt,YAt,ZAt);
    gp_Pnt EyePoint(XEye,YEye,ZEye);
    gp_Pnt AtPoint(XAt,YAt,ZAt);

    gp_Vec EyeVector(EyePoint,AtPoint);
    gp_Dir EyeDir(EyeVector);

    gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);
    Standard_Real X,Y,Z;
    aView->Convert(x,y,X,Y,Z);
    gp_Pnt ConvertedPoint(X,Y,Z);
    gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);
     
    gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
    ConvertedPointOnPlane.Y(),
    PlaneOfTheView);
    return ResultPoint;
}

gp_Pnt QoccMakePointOnEdge::ConvertMouseToPoint(int x, int y, QoccViewWidget* widget)
{
     
    Handle_V3d_View aView = widget->getView();
    V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
    aView->Eye(XEye,YEye,ZEye);
    aView->At(XAt,YAt,ZAt);
    gp_Pnt EyePoint(XEye,YEye,ZEye);
    gp_Pnt AtPoint(XAt,YAt,ZAt);

    gp_Vec EyeVector(EyePoint,AtPoint);
// 	QString s = "eye vector = (" + QString::number( EyeVector.X() ) + "," + QString::number( EyeVector.Y() ) + "," + QString::number( EyeVector.Z() ) +")";
// 	qDebug( s.toAscii() );
    gp_Dir EyeDir(EyeVector);

    gp_Pnt pCenter( (0.5*edgeBnd1).Added(0.5*edgeBnd2) );
        
    //gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);
    gp_Pln PlaneOfTheView = gp_Pln(pCenter,EyeDir);
    Standard_Real X,Y,Z;
    aView->Convert(x,y,X,Y,Z);
    gp_Pnt ConvertedPoint(X,Y,Z);
    gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);

    gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
                                    ConvertedPointOnPlane.Y(),
                                    PlaneOfTheView);
    return ResultPoint;
}

TopoDS_Shape QoccMakePointOnEdge::buildPoint( QoccViewWidget* widget )
{
    TopoDS_Shape sh;
    sh.Nullify();
    Standard_Real fParamStart, fParamEnd, fParamDist;
    Handle_Geom_Curve curve = BRep_Tool::Curve( TopoDS::Edge( edge ), fParamStart, fParamEnd );
    fParamDist = fParamEnd - fParamStart;
    if( !curve.IsNull() )
    {
        /*
        //calculate the projection of the mouse pos on the edge & display the projection
        //gp_Pnt p( widget->x(), widget->y(), widget->z() );
        gp_Pnt p = ConvertMouseToPoint( widget->getCurrentPoint().x(), widget->getCurrentPoint().y(), widget );
        GeomAPI_ProjectPointOnCurve ppoc( p, curve );
        if( ppoc.NbPoints() > 0 )
        {
            sh = BRepBuilderAPI_MakeVertex( ppoc.Point( 1 ) ); 
        }
        else 
            qDebug( "GeomAPI_ProjectPointOnCurve not done" );
        */
        int scrWidth, scrHeight;
        scrWidth = widget->width();
        scrHeight = widget->height();
        QPoint pos = widget->getCurrentPoint();
/*		
        static int reduce = 0;
        reduce++;
        if( reduce == 10 )
        {
            QString msg = "widget->getCurrentPoint() == (" + QString::number( pos.x() ) + "," +  QString::number( pos.y() ) + ")"; 
            qDebug( msg.toAscii() );
            msg = "scrWidth == " + QString::number( scrWidth ); 
            qDebug( msg.toAscii() );
            msg = "scrHeight == " + QString::number( scrHeight ); 
            qDebug( msg.toAscii() );
            msg = ".25*scrWidth == " + QString::number( .25*scrWidth ); 
            qDebug( msg.toAscii() );
            reduce = 0;
        }
*/
        Standard_Real fPointParam ;
        if( pos.x() <  .25*scrWidth )
            fPointParam = fParamStart;
        else if( pos.x() > .75*scrWidth )
            fPointParam = fParamEnd;
        else
        {
            fPointParam = ( ( pos.x() - .25*scrWidth )/( .50*scrWidth ) ) * fParamDist + fParamStart;
        }
        gp_Pnt p = curve->Value( fPointParam );
        sh = BRepBuilderAPI_MakeVertex( p ); 
    }
    else
        qDebug( "curve is NULL" );
    return sh;
}

void QoccMakePointOnEdge::moveEvent( QoccViewWidget* widget )
{
    static QPoint oldpos(0,0);
    QPoint pos = widget->getCurrentPoint();
    
    QString s = "point distance = " + QString::number( sqrt( pow(oldpos.x()-pos.x(), 2.0 )+ pow( oldpos.y()-pos.y() , 2.0 ) ) );
// 	qDebug( s.toAscii() );
    oldpos=pos;
    
    switch (parameter)
    {
        case Selection:
            break;
        case Point:
        {
            if( !aisPoint.IsNull() )
            {
                widget->getContext()->Remove( aisPoint ,Standard_True);
                aisPoint.Nullify();
            }
            TopoDS_Shape s = buildPoint( widget );
            aisPoint = new AIS_Shape( s );
            aisPoint->SetColor( Quantity_NOC_YELLOW );
            widget->getContext()->Display(aisPoint,Standard_True); 
            break;
        }
    }
}

void QoccMakePointOnEdge::clickEvent( QoccViewWidget* widget )
{
// 	cout << "clickEvent called" << endl;
    switch (parameter)
    {
    case Selection:
        widget->getContext()->InitSelected();
        if( widget->getContext()->MoreSelected() )
        {
            edge = widget->getContext()->SelectedShape();
            if( !edge.IsNull() && edge.ShapeType() == TopAbs_EDGE )
            {
                Bnd_Box bounds;
                BRepBndLib::Add( edge, bounds );
                bounds.SetGap( 0.0 );
                Standard_Real fXMin, fYMin, fZMin, fXMax, fYMax, fZMax;
                bounds.Get( fXMin, fYMin, fZMin, fXMax, fYMax, fZMax );
                edgeBnd1 = gp_XYZ( fXMin,fYMin,fZMin );
                edgeBnd1 = gp_XYZ( fXMax,fYMax,fZMax );
                
                GProp_GProps myProps;
                BRepGProp::LinearProperties(edge, myProps);
                edgeLength = myProps.Mass();
                
            
                myIsDrawing = true;
                parameter = Point;
// 				QApplication::setOverrideCursor( myInvisibleCursor );
            }
        }
        break;
    case Point:
    {
        if (!aisPoint.IsNull())
        {
            widget->getContext()->Remove(aisPoint,Standard_True);
            aisPoint.Nullify();
        }
        TopoDS_Shape s = buildPoint( widget );
        aisPoint = new AIS_Shape( s );
        aisPoint->SetColor( Quantity_NOC_YELLOW );
        widget->getContext()->Display(aisPoint,Standard_True); 
        aisPoint.Nullify();
        myIsDrawing = false;
        parameter = Selection;
// 		QApplication::restoreOverrideCursor();
        break;
    }
    }
}

