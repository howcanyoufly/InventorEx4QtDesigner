/********************************************************************************
** Form generated from reading UI file 'QuarterViewer.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUARTERVIEWER_H
#define UI_QUARTERVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>
#include "Quarter/QuarterWidget.h"

QT_BEGIN_NAMESPACE

class Ui_QuarterViewer
{
public:
    QGridLayout *gridLayout;
    SIM::Coin3D::Quarter::QuarterWidget *QuarterWidget;

    void setupUi(QWidget *QuarterViewer)
    {
        if (QuarterViewer->objectName().isEmpty())
            QuarterViewer->setObjectName(QString::fromUtf8("QuarterViewer"));
        QuarterViewer->resize(400, 300);
        gridLayout = new QGridLayout(QuarterViewer);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        QuarterWidget = new SIM::Coin3D::Quarter::QuarterWidget(QuarterViewer);
        QuarterWidget->setObjectName(QString::fromUtf8("QuarterWidget"));
        QuarterWidget->setAcceptDrops(true);
        QuarterWidget->setNavigationModeFile(QUrl(QString::fromUtf8("coin:/scxml/navigation/examiner.xml")));

        gridLayout->addWidget(QuarterWidget, 0, 0, 1, 1);


        retranslateUi(QuarterViewer);

        QMetaObject::connectSlotsByName(QuarterViewer);
    } // setupUi

    void retranslateUi(QWidget *QuarterViewer)
    {
        QuarterViewer->setWindowTitle(QCoreApplication::translate("QuarterViewer", "QuarterViewer", nullptr));
#if QT_CONFIG(whatsthis)
        QuarterWidget->setWhatsThis(QCoreApplication::translate("QuarterViewer", "The QuarterWidget displays Open Inventor scene graphs.", nullptr));
#endif // QT_CONFIG(whatsthis)
    } // retranslateUi

};

namespace Ui {
    class QuarterViewer: public Ui_QuarterViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUARTERVIEWER_H
