//
// C++ Interface: dlgbox
//
// Description: 
//
//
// Author: Dirk Burski <dirk.burski@nogrid.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DLGBOX_H
#define DLGBOX_H

#include <QDialog>
#include <QtGui>
#include <QDoubleSpinBox>

/**
    @author Dirk Burski <dirk.burski@nogrid.de>
*/
class DlgBox : public QDialog
{
Q_OBJECT
public:
    DlgBox(QWidget * parent = 0, Qt::WindowFlags f = 0 );

    ~DlgBox();
    QDoubleSpinBox *eW;
    QDoubleSpinBox *eD;
    QDoubleSpinBox *eH;
    QDoubleSpinBox *eX;
    QDoubleSpinBox *eY;
    QDoubleSpinBox *eZ;
    QLineEdit *eName;

};

#endif
