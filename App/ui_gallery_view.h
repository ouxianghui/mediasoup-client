/********************************************************************************
** Form generated from reading UI file 'gallery_view.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GALLERY_VIEW_H
#define UI_GALLERY_VIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>

QT_BEGIN_NAMESPACE

class Ui_GalleryView
{
public:

    void setupUi(QFrame *GalleryView)
    {
        if (GalleryView->objectName().isEmpty())
            GalleryView->setObjectName(QString::fromUtf8("GalleryView"));
        GalleryView->resize(400, 300);

        retranslateUi(GalleryView);

        QMetaObject::connectSlotsByName(GalleryView);
    } // setupUi

    void retranslateUi(QFrame *GalleryView)
    {
        GalleryView->setWindowTitle(QCoreApplication::translate("GalleryView", "Frame", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GalleryView: public Ui_GalleryView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GALLERY_VIEW_H
