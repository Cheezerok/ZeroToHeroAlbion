#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <QObject>
#include <QString>

class GameSettings : public QObject
{
    Q_OBJECT

public:
    explicit GameSettings(QObject* parent = nullptr);

    // Геттеры
    int getMaxBans() const { return m_maxBans; }
    int getMaxPlayers() const { return m_maxPlayers; }
    int getRollBuildsCount() const { return m_rollBuildsCount; }
    int getMaxBuildsTotal() const { return m_maxBuildsTotal; }
    int getGameTimeMinutes() const { return m_gameTimeMinutes; }

    // Сеттеры
    void setMaxBans(int value) { m_maxBans = value; emit settingsChanged(); }
    void setMaxPlayers(int value) { m_maxPlayers = value; emit settingsChanged(); }
    void setRollBuildsCount(int value) { m_rollBuildsCount = value; emit settingsChanged(); }
    void setMaxBuildsTotal(int value) { m_maxBuildsTotal = value; emit settingsChanged(); }
    void setGameTimeMinutes(int value) { m_gameTimeMinutes = value; emit settingsChanged(); }

    // Импорт/экспорт настроек
    QString exportToHash() const;
    bool importFromHash(const QString& hash);

signals:
    void settingsChanged();

private:
    int m_maxBans;           // Максимальное количество банов
    int m_maxPlayers;        // Максимальное количество игроков
    int m_rollBuildsCount;   // Количество билдов при ролле
    int m_maxBuildsTotal;    // Максимальное количество билдов в списке
    int m_gameTimeMinutes;   // Время партии в минутах
};

#endif // GAMESETTINGS_H 