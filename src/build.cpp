#include "build.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QDebug>

Build::Build()
    : m_name("Unnamed Build")
    , m_banned(false)
{
}

Build::Build(const QString& name)
    : m_name(name)
    , m_banned(false)
{
}

void Build::setName(const QString& name)
{
    m_name = name;
}

QString Build::getName() const
{
    return m_name;
}

void Build::setItem(Slot slot, const QString& itemName)
{
    m_items[slot] = itemName;
}

QString Build::getItemName(Slot slot) const
{
    return m_items.value(slot);
}

bool Build::hasItem(Slot slot) const
{
    return m_items.contains(slot) && !m_items[slot].isEmpty();
}

void Build::setBanned(bool banned)
{
    m_banned = banned;
}

bool Build::isBanned() const
{
    return m_banned;
}

QJsonObject Build::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["banned"] = m_banned;
    
    QJsonObject items;
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        items[QString::number(static_cast<int>(it.key()))] = it.value();
    }
    json["items"] = items;
    return json;
}

bool Build::fromJson(const QJsonObject& json)
{
    if (json.isEmpty()) {
        return false;
    }
    
    m_name = json["name"].toString();
    m_banned = json["banned"].toBool();
    m_items.clear();
    
    QJsonObject items = json["items"].toObject();
    for (auto it = items.begin(); it != items.end(); ++it) {
        bool ok;
        int slotValue = it.key().toInt(&ok);
        if (ok && slotValue >= 0 && slotValue <= static_cast<int>(Slot::OFF_HAND)) {
            m_items[static_cast<Slot>(slotValue)] = it.value().toString();
        }
    }
    
    return true;
}

QString Build::generateHash() const
{
    QJsonObject json = toJson();
    QJsonDocument doc(json);
    return QString::fromLatin1(doc.toJson().toBase64());
}

Build Build::fromHash(const QString& hash)
{
    QByteArray jsonData = QByteArray::fromBase64(hash.toLatin1());
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    
    Build build;
    if (!doc.isNull() && doc.isObject()) {
        build.fromJson(doc.object());
    }
    return build;
}

bool Build::operator==(const Build& other) const
{
    return m_name == other.m_name &&
           m_items == other.m_items &&
           m_banned == other.m_banned;
}

bool Build::operator!=(const Build& other) const
{
    return !(*this == other);
} 