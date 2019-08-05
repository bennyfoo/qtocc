/****************************************************************************
**
** Copyright (C) 2003-2007 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** Trolltech sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifdef Q_WS_WIN

#ifndef QTDOTNETSTYLE_H
#define QTDOTNETSTYLE_H

#include <QtGui/QWindowsVistaStyle>

class QLinearGradient;
class QBrush;

#if defined(Q_WS_WIN)
#  if !defined(QT_QTDOTNETSTYLE_EXPORT) && !defined(QT_QTDOTNETSTYLE_IMPORT)
#    define QT_QTDOTNETSTYLE_EXPORT
#  elif defined(QT_QTDOTNETSTYLE_IMPORT)
#    if defined(QT_QTDOTNETSTYLE_EXPORT)
#      undef QT_QTDOTNETSTYLE_EXPORT
#    endif
#    define QT_QTDOTNETSTYLE_EXPORT __declspec(dllimport)
#  elif defined(QT_QTDOTNETSTYLE_EXPORT)
#    undef QT_QTDOTNETSTYLE_EXPORT
#    define QT_QTDOTNETSTYLE_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTDOTNETSTYLE_EXPORT
#endif

class QtDotNetStylePrivate;
class QT_QTDOTNETSTYLE_EXPORT QtDotNetStyle : public QWindowsVistaStyle
{
public:
    enum ColorTheme {
        Standard,
        Office
    };
    QtDotNetStyle();

    QtDotNetStyle(ColorTheme palette);

    ~QtDotNetStyle();

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = 0) const;
    void drawControl(ControlElement element, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget) const;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const;

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                         SubControl sc, const QWidget *widget) const;

    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                    const QPoint &pos, const QWidget *widget = 0) const;

    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                                    const QWidget *widget = 0) const;
    int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0,
                  QStyleHintReturn *returnData = 0) const;
    int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;
    void polish(QWidget *widget);
    void unpolish(QWidget *widget);
    void polish(QPalette &pal);
    void polish(QApplication *app);
    void unpolish(QApplication *app);
    QPalette standardPalette() const;
    QtDotNetStylePrivate *d;
};

#endif

#endif