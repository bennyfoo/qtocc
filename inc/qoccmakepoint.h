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
#ifndef QOCCMAKEPOINT_H
#define QOCCMAKEPOINT_H

#include <QtCore/QObject>
#include "qocc.h"
#include "qocccommand.h"

class QOCC_DECLSPEC QoccPointDlg : public QObject
{
    Q_OBJECT

public:

    QoccPointDlg( );
    ~QoccPointDlg( );

private: 

    
};

class QOCC_DECLSPEC QoccMakePoint : public QoccCommand
{
    Q_OBJECT

public:

    QoccMakePoint( QAction* startingAction = NULL, QWidget* parent = NULL );
    ~QoccMakePoint( );

    virtual void clickEvent(QoccViewWidget* widget);
    virtual void moveEvent(QoccViewWidget*  widget);

};

#endif // QOCCMAKEPOINT_H
