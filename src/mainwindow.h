#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gamesettings.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include "buildconstructor.h"
#include "organizerwidget.h"
#include "rollbuildwidget.h"
#include "starscalculatorwidget.h"

class QWidget;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOrganizerClicked();
    void onRollClicked();
    void onStarsClicked();
    void onBuildsClicked();
    void onCloseClicked();
    void onStartClicked();
    void updateTimer();

private:
    void setupUi();
    void showRoleDialog();
    void showRoleSelection();
    bool importSettings(const QString& hash);
    void createMainMenu();
    void formatTime(int seconds, QString& result);

    QStackedWidget* m_stackedWidget;
    GameSettings* m_settings;
    bool m_isOrganizer;
    QWidget* m_centralWidget;
    BuildConstructor* m_buildConstructor;
    OrganizerWidget* m_organizerWidget;
    RollBuildWidget* m_rollBuildWidget;
    StarsCalculatorWidget* m_starsCalculatorWidget;
    
    // Таймер
    QTimer* m_timer;
    int m_remainingSeconds;
    QLabel* m_timerLabel;
    
    // Кнопки главного меню
    QPushButton* m_buildsButton;
    QPushButton* m_organizerButton;
    QPushButton* m_rollBuildButton;
    QPushButton* m_starsCalculatorButton;
    QPushButton* m_startButton;
    QPushButton* m_closeButton;
    QPushButton* m_playerButton;
    QPushButton* m_leaderboardButton;
    QPushButton* m_settingsButton;

    // Виджеты
    QWidget* m_playerWidget;
    QWidget* m_leaderboardWidget;
};

#endif // MAINWINDOW_H 