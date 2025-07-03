#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include "leaderboard.h"
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>

class LeaderboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit LeaderboardWidget(QWidget* parent = nullptr);
    void setLeaderboard(Leaderboard* leaderboard);
    PlayerStats* getSelectedPlayer() const;
    void updateDisplay();
    
private slots:
    void onCategoryFilterChanged(const QString& category);
    
private:
    Leaderboard* m_leaderboard;
    QTableWidget* m_table;
    QLabel* m_titleLabel;
    QString m_currentCategory;
    
    void setupUi();
    void createCategoryFilters();
    void updateTableHeaders();
    void stylizeTable();
};

#endif // LEADERBOARDWIDGET_H 