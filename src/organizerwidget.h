#ifndef ORGANIZERWIDGET_H
#define ORGANIZERWIDGET_H

#include <QWidget>
#include <QVector>
#include "build.h"
#include "gamesettings.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QSpacerItem;
class BuildCard;
class BuildPool;
class QScrollArea;

class OrganizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OrganizerWidget(BuildPool* buildPool, GameSettings* settings, QWidget* parent = nullptr);
    void addBuild(const Build& build);
    QVector<Build> getBuilds() const;

signals:
    void buildsUpdated();

private slots:
    void onBuildCardClicked(BuildCard* card);
    void onBuildCardBanChanged(BuildCard* card);
    void onAddClicked();
    void onRemoveClicked();
    void onExportClicked();
    void onBanClicked();
    void onSettingsClicked();

private:
    void setupUi();
    void updateBuildsDisplay();
    void updateButtonStates();
    void updateCardStyle(BuildCard* card);
    QVector<BuildCard*> getSelectedCards() const;
    
    BuildPool* m_buildPool;
    GameSettings* m_settings;
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QGridLayout* m_buildsLayout;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QPushButton* m_exportButton;
    QPushButton* m_banButton;
    QPushButton* m_settingsButton;
    QVector<BuildCard*> m_buildCards;
    QVector<Build> m_builds;
};

#endif // ORGANIZERWIDGET_H 