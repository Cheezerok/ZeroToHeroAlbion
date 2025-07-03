#ifndef ROLLBUILDWIDGET_H
#define ROLLBUILDWIDGET_H

#include <QWidget>
#include <QVector>
#include "build.h"
#include "buildpool.h"
#include <QDateTime>
#include "gamesettings.h"
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDialog>
#include <QLineEdit>
#include <QScrollArea>
#include "buildcard.h"

class QLabel;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLineEdit;
class QScrollArea;
class QTimer;

class RollResultDialog : public QDialog {
    Q_OBJECT
public:
    explicit RollResultDialog(const QVector<Build>& builds, QWidget* parent = nullptr);

private:
    void setupUi();
    QVector<Build> m_builds;
};

class RollBuildWidget : public QWidget {
    Q_OBJECT
public:
    explicit RollBuildWidget(GameSettings* settings, QWidget* parent = nullptr);
    void setBuilds(const QVector<Build>& builds);

private slots:
    void onRollClicked();
    void updateAnimation();
    void onHashEntered();

private:
    void setupUi();
    void startRoll();
    void stopRoll();
    void showResults();
    void updateBuildsDisplay();
    
    QLabel* m_buildLabel;
    QPushButton* m_rollButton;
    QTimer* m_animationTimer;
    QTimer* m_rollDuration;
    GameSettings* m_settings;
    BuildPool* m_buildPool;
    QVector<Build> m_builds;
    QVector<Build> m_rolledBuilds;
    bool m_isRolling;
    double m_animationSpeed;
    QLineEdit* m_hashInput;

    // Для отображения билдов
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QGridLayout* m_buildsLayout;
    QVector<BuildCard*> m_buildCards;
};

#endif // ROLLBUILDWIDGET_H 