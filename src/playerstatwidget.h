#ifndef PLAYERSTATWIDGET_H
#define PLAYERSTATWIDGET_H

#include "playerstats.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

class CategoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoryWidget(const QString& name, QWidget* parent = nullptr);
    void updateStats(double stars, int achievements);
    void setExpanded(bool expanded);

private:
    QLabel* m_titleLabel;
    QLabel* m_statsLabel;
    QWidget* m_content;
    bool m_isExpanded;
};

class PlayerStatWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlayerStatWidget(QWidget* parent = nullptr);
    void setPlayer(PlayerStats* player);
    void updateStats();

private:
    PlayerStats* m_player;
    QLabel* m_nameLabel;
    QLabel* m_totalStarsLabel;
    QMap<QString, CategoryWidget*> m_categoryWidgets;
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;

    void setupUi();
    void createCategoryWidget(const QString& category);
    void updateTotalStars();
};

#endif // PLAYERSTATWIDGET_H 