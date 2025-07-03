#ifndef STARSCALCULATORWIDGET_H
#define STARSCALCULATORWIDGET_H

#include "leaderboard.h"
#include "playerstatsdialog.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class LeaderboardWidget;

class StarsCalculatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StarsCalculatorWidget(QWidget* parent = nullptr);

private slots:
    void onAddPlayerClicked();
    void onEditPlayerClicked();
    void onRemovePlayerClicked();

private:
    void setupUi();
    void updateButtons();

    Leaderboard* m_leaderboard;
    LeaderboardWidget* m_leaderboardWidget;
    QPushButton* m_addPlayerButton;
    QPushButton* m_editPlayerButton;
    QPushButton* m_removePlayerButton;
};

#endif // STARSCALCULATORWIDGET_H 