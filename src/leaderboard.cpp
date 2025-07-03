#include "leaderboard.h"
#include <algorithm>

Leaderboard::Leaderboard(QObject* parent)
    : QObject(parent)
{
}

void Leaderboard::addPlayer(PlayerStats* player)
{
    if (!player) return;
    
    m_players[player->getPlayerName()] = player;
    updateRankings();
}

void Leaderboard::removePlayer(const QString& playerName)
{
    m_players.remove(playerName);
    updateRankings();
}

void Leaderboard::updateRankings()
{
    m_rankings.clear();
    
    // Create ranking entries for all players
    for (auto it = m_players.begin(); it != m_players.end(); ++it) {
        PlayerStats* player = it.value();
        PlayerRank rank;
        rank.name = player->getPlayerName();
        
        // Calculate total stars and stars per category
        const auto& achievements = player->getAchievements();
        QMap<QString, double> categoryTotals;
        
        for (const auto& achievement : achievements) {
            if (achievement.achieved) {
                rank.stars += achievement.stars;
                categoryTotals[achievement.category] += achievement.stars;
            }
        }
        
        // Store category totals
        rank.categoryStars = categoryTotals;
        
        m_rankings.append(rank);
    }
    
    // Sort rankings by total stars
    std::sort(m_rankings.begin(), m_rankings.end(),
              [](const PlayerRank& a, const PlayerRank& b) {
                  return a.stars > b.stars;
              });
    
    calculateRanks();
    emit rankingsUpdated();
}

void Leaderboard::calculateRanks()
{
    if (m_rankings.isEmpty()) return;
    
    // First player always gets rank 1
    m_rankings[0].rank = 1;
    
    // For subsequent players
    for (int i = 1; i < m_rankings.size(); ++i) {
        if (m_rankings[i].stars == m_rankings[i-1].stars) {
            // Same stars = same rank
            m_rankings[i].rank = m_rankings[i-1].rank;
        } else {
            // Different stars = position + 1
            m_rankings[i].rank = i + 1;
        }
    }
}

Leaderboard::PlayerRank Leaderboard::getPlayerRank(const QString& playerName) const
{
    for (const auto& rank : m_rankings) {
        if (rank.name == playerName) {
            return rank;
        }
    }
    return PlayerRank();
} 