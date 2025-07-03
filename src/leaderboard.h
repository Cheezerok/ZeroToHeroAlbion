#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "playerstats.h"
#include <QObject>
#include <QVector>
#include <QMap>

class Leaderboard : public QObject {
    Q_OBJECT
public:
    struct PlayerRank {
        QString name;
        double stars;
        QMap<QString, double> categoryStars;
        int rank;
        
        PlayerRank() : stars(0), rank(0) {}
    };

    explicit Leaderboard(QObject* parent = nullptr);
    
    void addPlayer(PlayerStats* player);
    void removePlayer(const QString& playerName);
    void updateRankings();
    
    QVector<PlayerRank> getRankings() const { return m_rankings; }
    PlayerRank getPlayerRank(const QString& playerName) const;
    int getPlayerCount() const { return m_players.count(); }
    PlayerStats* getPlayer(const QString& playerName) const { return m_players.value(playerName); }
    
signals:
    void rankingsUpdated();
    
private:
    QMap<QString, PlayerStats*> m_players;
    QVector<PlayerRank> m_rankings;
    
    void calculateRanks();
};

#endif // LEADERBOARD_H 