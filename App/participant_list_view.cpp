#include "participant_list_view.h"
#include "ui_participant_list_view.h"
#include "service/i_participant.h"
#include "service/i_room_client.h"
#include "participant_item_view.h"
#include <QListWidgetItem>
#include <QVariant>


ParticipantListView::ParticipantListView(std::shared_ptr<vi::IRoomClient> vrc, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParticipantListView())
    , _vrc(vrc)
{
    ui->setupUi(this);
    ui->listWidget->setViewMode(QListView::ListMode);
}

ParticipantListView::~ParticipantListView()
{
    removeAll();
}

void ParticipantListView::addParticipant(std::shared_ptr<vi::IParticipant> participant)
{
    if (!participant) {
        return;
    }

    if (getItem(participant->id())) {
        return;
    }

    ParticipantItemView* view = new ParticipantItemView(participant, _vrc.lock(), this);
    view->setMinimumHeight(64);
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(0, 64));
    QVariant var(participant->id().c_str());
    item->setData(Qt::UserRole+1, var);
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, view);

}

void ParticipantListView::updateParticipant(std::shared_ptr<vi::IParticipant> participant)
{
    if (!participant) {
        return;
    }

    QListWidgetItem* item = getItem(participant->id());
    if (!item) {
        return;
    }

    ParticipantItemView* view = static_cast<ParticipantItemView*>(ui->listWidget->itemWidget(item));
    if (!view) {
        return;
    }

    view->update(participant);
}

void ParticipantListView::removeParticipant(std::shared_ptr<vi::IParticipant> participant)
{
    if (!participant) {
        return;
    }
    int count = ui->listWidget->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item) {
            QVariant var = item->data(Qt::UserRole+1);
            std::string value = std::string((const char*)var.toString().toLocal8Bit());
            if (value == participant->id()) {
                QWidget *widget = ui->listWidget->itemWidget(item);
                if (nullptr != widget)
                {
                    ui->listWidget->removeItemWidget(item);
                    widget->deleteLater();
                }

                item = ui->listWidget->takeItem(i);
                delete item;
                item = nullptr;

                ui->listWidget->update();
                break;
            }
        }
     }
}

void ParticipantListView::removeAll()
{
    while (0 != ui->listWidget->count()) {
        QListWidgetItem *item = ui->listWidget->item(0);
        QWidget *widget = ui->listWidget->itemWidget(item);
        if (nullptr != widget)
        {
            ui->listWidget->removeItemWidget(item);
            widget->deleteLater();
        }

        item = ui->listWidget->takeItem(0);
        delete item;
        item = nullptr;
    }
}

QListWidgetItem* ParticipantListView::getItem(const std::string& pid)
{
    if (pid.empty()) {
        return nullptr;
    }
    int count = ui->listWidget->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item) {
            QVariant var = item->data(Qt::UserRole+1);
            std::string value = std::string((const char*)var.toString().toLocal8Bit());
            if (value == pid) {
                return item;
            }
        }
     }
    return nullptr;
}

bool ParticipantListView::hasParticipant(const std::string& pid)
{
    return getItem(pid) != nullptr;
}
