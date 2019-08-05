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
#ifndef QOCCCOMMAND_H
#define QOCCCOMMAND_H

#include <QtCore/QObject>
#include "qocc.h"

class QAction;
class QoccViewerContext;
class QoccViewWidget;

class QOCC_DECLSPEC QoccCommand : public QObject
{
    Q_OBJECT

public:

    QoccCommand( QAction* startingAction = NULL, QWidget* parent = NULL );
    ~QoccCommand( );

    virtual bool isDrawing() { return myIsDrawing; }

    virtual void clickEvent( QoccViewWidget* ) {}
    virtual void moveEvent ( QoccViewWidget* ) {}

protected: 

    QAction*		myAction;
    bool			myIsDrawing;	
};

#endif // QOCCCOMMAND_H
