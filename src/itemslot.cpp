#include "itemslot.h"
#include <QPixmap>
#include <QDrag>
#include <QMimeData>
#include <QDataStream>

ItemSlot::ItemSlot(Build::Slot slot, QWidget *parent)
    : QLabel(parent)
    , m_slot(slot)
{
    setFixedSize(64, 64);
    setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 128); border: 1px solid white; }");
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
}

void ItemSlot::setItem(const QString& itemName, const QPixmap& icon)
{
    m_itemName = itemName;
    if (!icon.isNull()) {
        setPixmap(icon.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    setToolTip(itemName);
    emit itemChanged(m_slot, itemName);
}

void ItemSlot::clearItem()
{
    m_itemName.clear();
    clear();
    setToolTip("");
    emit itemChanged(m_slot, "");
}

void ItemSlot::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-item")) {
        event->acceptProposedAction();
        setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 128); border: 2px solid yellow; }");
    }
}

void ItemSlot::dragLeaveEvent(QDragLeaveEvent* event)
{
    setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 128); border: 1px solid white; }");
    QLabel::dragLeaveEvent(event);
}

void ItemSlot::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-item")) {
        QByteArray itemData = event->mimeData()->data("application/x-item");
        QDataStream stream(itemData);
        QString itemName;
        QPixmap icon;
        stream >> itemName >> icon;
        
        setItem(itemName, icon);
        event->acceptProposedAction();
    }
    setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 128); border: 1px solid white; }");
}

void ItemSlot::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !m_itemName.isEmpty()) {
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;
        
        QByteArray itemData;
        QDataStream stream(&itemData, QIODevice::WriteOnly);
        stream << m_itemName << *pixmap();
        
        mimeData->setData("application/x-item", itemData);
        drag->setMimeData(mimeData);
        drag->setPixmap(*pixmap());
        drag->setHotSpot(event->pos());
        
        if (drag->exec(Qt::CopyAction | Qt::MoveAction) == Qt::MoveAction) {
            clearItem();
        }
    }
    QLabel::mousePressEvent(event);
} 