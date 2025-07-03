#include "playerstats.h"

PlayerStats::PlayerStats(const QString& playerName)
    : m_playerName(playerName)
    , m_totalStars(0)
{
    initializeAchievements();
    updateCategoryTotals();
}

void PlayerStats::addAchievement(const QString& name, double stars, const QString& category)
{
    m_achievements.append(Achievement(name, stars, category));
}

void PlayerStats::setAchieved(const QString& name, bool achieved)
{
    for (auto& achievement : m_achievements) {
        if (achievement.name == name) {
            achievement.achieved = achieved;
            break;
        }
    }
    updateCategoryTotals();
}

void PlayerStats::updateCategoryTotals()
{
    m_categoryTotals.clear();
    m_totalStars = 0;
    QMap<QString, CategoryTotal> totals;

    for (const auto& achievement : m_achievements) {
        if (achievement.achieved) {
            auto& category = totals[achievement.category];
            if (category.name.isEmpty()) {
                category.name = achievement.category;
            }
            category.totalStars += achievement.stars;
            category.achievementsCount++;
            m_totalStars += achievement.stars;
        }
    }

    for (const auto& category : totals) {
        m_categoryTotals.append(category);
    }
}

void PlayerStats::initializeAchievements()
{
    addPveAchievements();
    addPvpAchievements();
    addGatheringAchievements();
    addInfamyAchievements();
    addMasteryAchievements();
    addSilverAchievements();
    addBrecilienAchievements();
    addMightAchievements();
    addSmugglerAchievements();
    addSpecialAchievements();
}

void PlayerStats::addPveAchievements()
{
    addAchievement("0.5M PvE", 1, "PvE Fame");
    addAchievement("0.75M PvE", 3, "PvE Fame");
    addAchievement("1M PvE", 5, "PvE Fame");
    addAchievement("1.5M PvE", 10, "PvE Fame");
}

void PlayerStats::addPvpAchievements()
{
    addAchievement("5k PvP", 1, "PvP Fame");
    addAchievement("10k PvP", 3, "PvP Fame");
    addAchievement("30k PvP", 5, "PvP Fame");
    addAchievement("60k PvP", 10, "PvP Fame");
}

void PlayerStats::addGatheringAchievements()
{
    addAchievement("0.5k Gathering", 1, "Gathering Fame");
    addAchievement("1k Gathering", 3, "Gathering Fame");
    addAchievement("2k Gathering", 5, "Gathering Fame");
    addAchievement("4k Gathering", 10, "Gathering Fame");
}

void PlayerStats::addInfamyAchievements()
{
    addAchievement("2k Infamy", 1, "Infamy");
    addAchievement("8k Infamy", 3, "Infamy");
    addAchievement("15k Infamy", 5, "Infamy");
    addAchievement("25k Infamy", 10, "Infamy");
}

void PlayerStats::addMasteryAchievements()
{
    addAchievement("20 Masteries", 1, "Masteries");
    addAchievement("25 Masteries", 3, "Masteries");
    addAchievement("30 Masteries", 5, "Masteries");
    addAchievement("35 Masteries", 10, "Masteries");
}

void PlayerStats::addSilverAchievements()
{
    addAchievement("0.25M Silver", 1, "Silver");
    addAchievement("0.5M Silver", 3, "Silver");
    addAchievement("0.75M Silver", 5, "Silver");
    addAchievement("1M Silver", 10, "Silver");
}

void PlayerStats::addBrecilienAchievements()
{
    addAchievement("5k Brecilien", 1, "Brecilien");
    addAchievement("10k Brecilien", 3, "Brecilien");
    addAchievement("20k Brecilien", 5, "Brecilien");
    addAchievement("40k Brecilien", 10, "Brecilien");
}

void PlayerStats::addMightAchievements()
{
    addAchievement("2k Might", 1, "Might");
    addAchievement("5k Might", 3, "Might");
    addAchievement("11k Might", 5, "Might");
    addAchievement("20k Might", 10, "Might");
}

void PlayerStats::addSmugglerAchievements()
{
    addAchievement("5k Smuggler", 1, "Smuggler");
    addAchievement("10k Smuggler", 3, "Smuggler");
    addAchievement("15k Smuggler", 5, "Smuggler");
    addAchievement("25k Smuggler", 10, "Smuggler");
}

void PlayerStats::addSpecialAchievements()
{
    addAchievement("Unlock T7 Reaver", 1, "Special");
    addAchievement("Unlock T8 Reaver", 3, "Special");
    addAchievement("Every 5k PvP fame above 60k", 1, "Special");
    addAchievement("Every 100k PvE fame above 1.5M", 1, "Special");
}

void PlayerStats::checkPveFame(double fame)
{
    if (fame >= 1500000) {
        setAchieved("1.5M PvE");
        int extraStars = static_cast<int>((fame - 1500000) / 100000);
        for (int i = 0; i < extraStars; i++) {
            setAchieved("Every 100k PvE fame above 1.5M");
        }
    }
    if (fame >= 1000000) setAchieved("1M PvE");
    if (fame >= 750000) setAchieved("0.75M PvE");
    if (fame >= 500000) setAchieved("0.5M PvE");
}

void PlayerStats::checkPvpFame(double fame)
{
    if (fame >= 60000) {
        setAchieved("60k PvP");
        int extraStars = static_cast<int>((fame - 60000) / 5000);
        for (int i = 0; i < extraStars; i++) {
            setAchieved("Every 5k PvP fame above 60k");
        }
    }
    if (fame >= 30000) setAchieved("30k PvP");
    if (fame >= 10000) setAchieved("10k PvP");
    if (fame >= 5000) setAchieved("5k PvP");
}

void PlayerStats::checkGatheringFame(double fame)
{
    if (fame >= 4000) setAchieved("4k Gathering");
    if (fame >= 2000) setAchieved("2k Gathering");
    if (fame >= 1000) setAchieved("1k Gathering");
    if (fame >= 500) setAchieved("0.5k Gathering");
}

void PlayerStats::checkInfamyPoints(double points)
{
    if (points >= 25000) setAchieved("25k Infamy");
    if (points >= 15000) setAchieved("15k Infamy");
    if (points >= 8000) setAchieved("8k Infamy");
    if (points >= 2000) setAchieved("2k Infamy");
}

void PlayerStats::checkMasteriesCount(int count)
{
    if (count >= 35) setAchieved("35 Masteries");
    if (count >= 30) setAchieved("30 Masteries");
    if (count >= 25) setAchieved("25 Masteries");
    if (count >= 20) setAchieved("20 Masteries");
}

void PlayerStats::checkSilverAmount(double amount)
{
    if (amount >= 1000000) setAchieved("1M Silver");
    if (amount >= 750000) setAchieved("0.75M Silver");
    if (amount >= 500000) setAchieved("0.5M Silver");
    if (amount >= 250000) setAchieved("0.25M Silver");
}

void PlayerStats::checkBrecilienPoints(int points)
{
    if (points >= 40000) setAchieved("40k Brecilien");
    if (points >= 20000) setAchieved("20k Brecilien");
    if (points >= 10000) setAchieved("10k Brecilien");
    if (points >= 5000) setAchieved("5k Brecilien");
}

void PlayerStats::checkMightPoints(int points)
{
    if (points >= 20000) setAchieved("20k Might");
    if (points >= 11000) setAchieved("11k Might");
    if (points >= 5000) setAchieved("5k Might");
    if (points >= 2000) setAchieved("2k Might");
}

void PlayerStats::checkSmugglerPoints(int points)
{
    if (points >= 25000) setAchieved("25k Smuggler");
    if (points >= 15000) setAchieved("15k Smuggler");
    if (points >= 10000) setAchieved("10k Smuggler");
    if (points >= 5000) setAchieved("5k Smuggler");
} 