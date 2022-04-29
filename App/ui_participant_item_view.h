/********************************************************************************
** Form generated from reading UI file 'participant_item_view.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARTICIPANT_ITEM_VIEW_H
#define UI_PARTICIPANT_ITEM_VIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ParticipantItemView
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *labelDisplayName;
    QProgressBar *progressBarVolume;
    QToolButton *toolButtonAudio;
    QToolButton *toolButtonVideo;

    void setupUi(QWidget *ParticipantItemView)
    {
        if (ParticipantItemView->objectName().isEmpty())
            ParticipantItemView->setObjectName(QString::fromUtf8("ParticipantItemView"));
        ParticipantItemView->resize(400, 36);
        horizontalLayout = new QHBoxLayout(ParticipantItemView);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelDisplayName = new QLabel(ParticipantItemView);
        labelDisplayName->setObjectName(QString::fromUtf8("labelDisplayName"));

        horizontalLayout->addWidget(labelDisplayName);

        progressBarVolume = new QProgressBar(ParticipantItemView);
        progressBarVolume->setObjectName(QString::fromUtf8("progressBarVolume"));
        progressBarVolume->setMaximumSize(QSize(10, 50));
        progressBarVolume->setMaximum(100);
        progressBarVolume->setValue(0);
        progressBarVolume->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        progressBarVolume->setTextVisible(false);
        progressBarVolume->setOrientation(Qt::Vertical);

        horizontalLayout->addWidget(progressBarVolume);

        toolButtonAudio = new QToolButton(ParticipantItemView);
        toolButtonAudio->setObjectName(QString::fromUtf8("toolButtonAudio"));

        horizontalLayout->addWidget(toolButtonAudio);

        toolButtonVideo = new QToolButton(ParticipantItemView);
        toolButtonVideo->setObjectName(QString::fromUtf8("toolButtonVideo"));

        horizontalLayout->addWidget(toolButtonVideo);

        horizontalLayout->setStretch(0, 4);
        horizontalLayout->setStretch(1, 4);
        horizontalLayout->setStretch(2, 1);
        horizontalLayout->setStretch(3, 1);

        retranslateUi(ParticipantItemView);

        QMetaObject::connectSlotsByName(ParticipantItemView);
    } // setupUi

    void retranslateUi(QWidget *ParticipantItemView)
    {
        ParticipantItemView->setWindowTitle(QCoreApplication::translate("ParticipantItemView", "Form", nullptr));
        labelDisplayName->setText(QString());
        progressBarVolume->setFormat(QCoreApplication::translate("ParticipantItemView", "%p", nullptr));
        toolButtonAudio->setText(QCoreApplication::translate("ParticipantItemView", "audio", nullptr));
        toolButtonVideo->setText(QCoreApplication::translate("ParticipantItemView", "video", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ParticipantItemView: public Ui_ParticipantItemView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARTICIPANT_ITEM_VIEW_H
