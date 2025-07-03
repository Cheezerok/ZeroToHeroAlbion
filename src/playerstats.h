#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QString>
#include <QMap>
#include <QVector>

class PlayerStats {
public:
    struct Achievement {
        QString name;
        double stars;
        QString category;
        bool achieved;
        
        Achievement(const QString& n = "", double s = 0, const QString& c = "", bool a = false)
            : name(n), stars(s), category(c), achieved(a) {}
    };

    struct CategoryTotal {
        QString name;
        double totalStars;
        int achievementsCount;
        
        CategoryTotal(const QString& n = "", double s = 0, int c = 0)
            : name(n), totalStars(s), achievementsCount(c) {}
    };

    PlayerStats(const QString& playerName);

    // Getters
    QString getPlayerName() const { return m_playerName; }
    double getTotalStars() const { return m_totalStars; }
    QVector<CategoryTotal> getCategoryTotals() const { return m_categoryTotals; }
    QVector<Achievement> getAchievements() const { return m_achievements; }

    // Achievement management
    void addAchievement(const QString& name, double stars, const QString& category);
    void setAchieved(const QString& name, bool achieved = true);
    void updateCategoryTotals();

    // PvE achievements
    void checkPveFame(double fame);
    void checkPvpFame(double fame);
    void checkGatheringFame(double fame);
    void checkInfamyPoints(double points);
    void checkMasteriesCount(int count);
    void checkSilverAmount(double amount);
    void checkBrecilienPoints(int points);
    void checkMightPoints(int points);
    void checkSmugglerPoints(int points);

private:
    QString m_playerName;
    double m_totalStars;
    QVector<Achievement> m_achievements;
    QVector<CategoryTotal> m_categoryTotals;

    void initializeAchievements();
    void addPveAchievements();
    void addPvpAchievements();
    void addGatheringAchievements();
    void addInfamyAchievements();
    void addMasteryAchievements();
    void addSilverAchievements();
    void addBrecilienAchievements();
    void addMightAchievements();
    void addSmugglerAchievements();
    void addSpecialAchievements();
};

#endif // PLAYERSTATS_H 