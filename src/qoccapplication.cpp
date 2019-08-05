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

/*
\class	QtApplication
\brief	This class provides an application extension for the QtOpenCascade Toolkit.
\author	Peter C. Dolbey
*/

#include <QtCore/QEvent>
#include <QtCore/QDataStream>
#include <QtGui/QCursor>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QThread>

#include "qoccinternal.h"
#include "qoccapplication.h"

QoccApplication::QoccApplication(int &argc, char **argv, int _internal ) :
	QApplication (argc, argv, _internal),
	mySplash (NULL)
{

#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
	setAttribute(Qt::AA_NativeWindows);
#endif

}

QoccApplication::~QoccApplication()
{
	if ( mySplash )
	{
		delete mySplash;
		mySplash = NULL;
	}
}

void QoccApplication::splashScreen ( const QPixmap &pixmap )
{
	QPixmap* p = (QPixmap*) &pixmap;
	if ( p->isNull() )
	{
		p = new QPixmap( ":/Resources/style/qoccsplash.png" );
	}
	mySplash = new QSplashScreen( *p, Qt::WindowStaysOnTopHint );
	if ( mySplash )
	{
		mySplash->show();
		splashMessage( tr( "Initializing Application..." ), Qt::AlignRight | Qt::AlignTop );
	}
}

void QoccApplication::splashMessage(const QString &message, int alignment, const QColor &color)
{
	if ( mySplash )
	{
		mySplash->showMessage( message, alignment, color );
	}
}

void QoccApplication::splashFinish ( QWidget* widget, long millisecs )
{
	if ( mySplash )
	{
		msleep ( millisecs );
		mySplash->finish( widget );
		delete mySplash;
		mySplash = NULL;
	}
}

void QoccApplication::msleep(unsigned long millisecs)
{
	QMutex mutex;
	QWaitCondition waitCondition;
	mutex.lock();
	waitCondition.wait( &mutex, millisecs );
	mutex.unlock();
}
