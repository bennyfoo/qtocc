/********************************************************************************
** Form generated from reading UI file 'QoccMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QOCCMAINWINDOW_H
#define UI_QOCCMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QoccMainWindowClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QoccMainWindowClass)
    {
        if (QoccMainWindowClass->objectName().isEmpty())
            QoccMainWindowClass->setObjectName(QString::fromUtf8("QoccMainWindowClass"));
        QoccMainWindowClass->resize(600, 400);
        menuBar = new QMenuBar(QoccMainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QoccMainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QoccMainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QoccMainWindowClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QoccMainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QoccMainWindowClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QoccMainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QoccMainWindowClass->setStatusBar(statusBar);

        retranslateUi(QoccMainWindowClass);

        QMetaObject::connectSlotsByName(QoccMainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *QoccMainWindowClass)
    {
        QoccMainWindowClass->setWindowTitle(QCoreApplication::translate("QoccMainWindowClass", "QoccMainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QoccMainWindowClass: public Ui_QoccMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QOCCMAINWINDOW_H
