//
// C++ Implementation: dlgbox
//
// Description: 
//
//
// Author: Dirk Burski <dirk.burski@nogrid.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dlgbox.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


DlgBox::DlgBox(QWidget * parent , Qt::WindowFlags f )
 : QDialog(parent , f)
{
	QGridLayout *layout = new QGridLayout( this );
	eX = new QDoubleSpinBox( this );
	eY = new QDoubleSpinBox( this );
	eZ = new QDoubleSpinBox( this );
	eW = new QDoubleSpinBox( this );
	eW->setValue( 99.0 );
	eD = new QDoubleSpinBox( this );
	eD->setValue( 99.0 );
	eH = new QDoubleSpinBox( this );
	eH->setValue( 99.0 );
	layout->addWidget( new QLabel( "X" ), 0, 0 );
	layout->addWidget( new QLabel( "Y" ), 1, 0 );
	layout->addWidget( new QLabel( "Z" ), 2, 0 );
	layout->addWidget( new QLabel( "Width" ), 3, 0 );
	layout->addWidget( new QLabel( "Height" ), 4, 0 );
	layout->addWidget( new QLabel( "Depth" ), 5, 0 );
	layout->addWidget( eX, 0, 1 );
	layout->addWidget( eY, 1, 1 );
	layout->addWidget( eZ, 2, 1 );
	layout->addWidget( eW, 3, 1 );
	layout->addWidget( eD, 4, 1 );
	layout->addWidget( eH, 5, 1 );
	eName = new QLineEdit( this );
	layout->addWidget( new QLabel( "Name" ), 6, 0 );
	layout->addWidget( eName, 6, 1 );

	QPushButton *okButton = new QPushButton ( tr ( "&Ok" ) );
	QPushButton *cancelButton = new QPushButton ( tr ( "&Cancel" ) );
	connect ( okButton, SIGNAL ( clicked() ), this, SLOT ( accept() ) );
	connect ( cancelButton, SIGNAL ( clicked() ), this, SLOT ( reject() ) );
	layout->addWidget( okButton, 7, 0 );
	layout->addWidget( cancelButton, 7, 1 );
	
	
}


DlgBox::~DlgBox()
{
}


