#ifndef ITEMSLOT_H
#define ITEMSLOT_H

#include <QLabel>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPixmap>
#include "build.h"

class ItemSlot : public QLabel {
    Q_OBJECT
public:
    explicit ItemSlot(Build::Slot slot, QWidget* parent = nullptr);
    virtual ~ItemSlot() override = default;
    
    void setItem(const QString& itemName, const QPixmap& icon = QPixmap());
    void clearItem();
    QString getItemName() const { return m_itemName; }
    Build::Slot getSlot() const { return m_slot; }

signals:
    void itemChanged(Build::Slot slot, const QString& itemName);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    Build::Slot m_slot;
    QString m_itemName;
};

#endif // ITEMSLOT_H 