#include "gamesettings.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

GameSettings::GameSettings(QObject* parent)
    : QObject(parent)
    , m_maxBans(3)          // По умолчанию можно забанить 3 билда
    , m_maxPlayers(10)      // По умолчанию максимум 10 игроков
    , m_rollBuildsCount(3)  // По умолчанию выпадает 3 билда
    , m_maxBuildsTotal(20)  // По умолчанию максимум 20 билдов в списке
    , m_gameTimeMinutes(30) // По умолчанию 30 минут на игру
{
}

QString GameSettings::exportToHash() const
{
    QJsonObject json;
    json["maxBans"] = m_maxBans;
    json["maxPlayers"] = m_maxPlayers;
    json["rollBuildsCount"] = m_rollBuildsCount;
    json["maxBuildsTotal"] = m_maxBuildsTotal;
    json["gameTimeMinutes"] = m_gameTimeMinutes;

    QJsonDocument doc(json);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact).toBase64());
}

bool GameSettings::importFromHash(const QString& hash)
{
    try {
        QByteArray jsonData = QByteArray::fromBase64(hash.toUtf8());
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        
        if (!doc.isObject())
            return false;
            
        QJsonObject json = doc.object();
        
        if (json.contains("maxBans"))
            m_maxBans = json["maxBans"].toInt();
        if (json.contains("maxPlayers"))
            m_maxPlayers = json["maxPlayers"].toInt();
        if (json.contains("rollBuildsCount"))
            m_rollBuildsCount = json["rollBuildsCount"].toInt();
        if (json.contains("maxBuildsTotal"))
            m_maxBuildsTotal = json["maxBuildsTotal"].toInt();
        if (json.contains("gameTimeMinutes"))
            m_gameTimeMinutes = json["gameTimeMinutes"].toInt();
            
        emit settingsChanged();
        return true;
    }
    catch (...) {
        return false;
    }
} 