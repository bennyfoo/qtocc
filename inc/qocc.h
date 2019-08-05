/****************************************************************************
**
** This file is part of the QtOpenCascade Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Copyright (C) 2006, 2007, Peter Dolbey, Marc Britten, Stephane Routelous
**               Stefan Boeykens, Pawel Dobrolowski, Walter Steffe
**               Álvaro Castro Castilla, Dirk Burski.
**
** All rights reserved.
**
****************************************************************************/
/*!
\mainpage

			This is part of the the QtOpenCascade Toolkit.

			This may be used under the terms of the GNU General Public
			License version 2.0 as published by the Free Software Foundation
			and appearing in the file LICENSE.GPL included in the packaging of
			this file.

			Copyright (C) 2006, 2007, Peter Dolbey, Marc Britten, Stephane Routelous
			Stefan Boeykens, Pawel Dobrolowski, Walter Steffe
			Álvaro Castro Castilla, Dirk Burski.

			All rights reserved.

			This is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
			WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

\section	install Installation

			To install and compile the library, unzip the source file into a folder
			of your choice and open a command prompt there.
			<pre>
				qmake
				make / nmake
			</pre>
			to build the package.
*/


#ifndef QOCC_H
#define QOCC_H

#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Aspect_GridType.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Standard_TypeDef.hxx>
#include <Quantity_Factor.hxx>
#include <Quantity_Length.hxx>
#include <Quantity_NameOfColor.hxx>
#include <V3d_Coordinate.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <WNT_Window.hxx>

#define QOCC_STATIC

#ifdef QOCC_STATIC
#define QOCC_DECLSPEC
#else
#ifdef QOCC_MAKEDLL
#define QOCC_DECLSPEC Q_DECL_EXPORT
#else
#define QOCC_DECLSPEC Q_DECL_IMPORT
#endif
#endif

#define SIGN(X) ((X) < 0. ? -1 : ((X) > 0. ? 1 : 0.))
#define INITIAL_PRECISION 0.001

#endif // Qocc_H

