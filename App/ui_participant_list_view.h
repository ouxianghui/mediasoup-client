/********************************************************************************
** Form generated from reading UI file 'participant_list_view.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARTICIPANT_LIST_VIEW_H
#define UI_PARTICIPANT_LIST_VIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ParticipantListView
{
public:
    QGridLayout *gridLayout;
    QListWidget *listWidget;

    void setupUi(QWidget *ParticipantListView)
    {
        if (ParticipantListView->objectName().isEmpty())
            ParticipantListView->setObjectName(QString::fromUtf8("ParticipantListView"));
        ParticipantListView->resize(400, 300);
        gridLayout = new QGridLayout(ParticipantListView);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        listWidget = new QListWidget(ParticipantListView);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 0, 0, 1, 1);


        retranslateUi(ParticipantListView);

        QMetaObject::connectSlotsByName(ParticipantListView);
    } // setupUi

    void retranslateUi(QWidget *ParticipantListView)
    {
        ParticipantListView->setWindowTitle(QCoreApplication::translate("ParticipantListView", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ParticipantListView: public Ui_ParticipantListView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARTICIPANT_LIST_VIEW_H
