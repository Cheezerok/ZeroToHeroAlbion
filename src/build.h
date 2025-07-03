#ifndef BUILD_H
#define BUILD_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QMap>
#include <QtCore/qhash.h>

class Build {
public:
    enum class Slot {
        HEAD,
        CAPE,
        CHEST,
        SHOES,
        MAIN_HAND,
        OFF_HAND
    };

    Build();
    explicit Build(const QString& name);
    
    void setName(const QString& name);
    QString getName() const;
    
    void setItem(Slot slot, const QString& itemName);
    QString getItemName(Slot slot) const;
    bool hasItem(Slot slot) const;
    
    void setBanned(bool banned);
    bool isBanned() const;
    
    QMap<Slot, QString> getItems() const { return m_items; }
    
    QString generateHash() const;
    static Build fromHash(const QString& hash);
    
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject& json);

    bool operator==(const Build& other) const;
    bool operator!=(const Build& other) const;

    bool operator<(const Build& other) const {
        return m_name < other.m_name;
    }

private:
    QString m_name;
    QMap<Slot, QString> m_items;
    bool m_banned = false;
    QMap<Slot, QPixmap> m_icons;
    // TODO: Add more build properties (weapons, armor, etc.)
};

inline uint qHash(const Build& build, uint seed = 0) noexcept
{
    return qHash(build.getName(), seed);
}

#endif // BUILD_H 