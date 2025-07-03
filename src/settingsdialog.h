#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include "gamesettings.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(GameSettings* settings, QWidget* parent = nullptr);

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onExportClicked();

private:
    void setupUi();
    void loadSettings();
    void saveSettings();
    QWidget* createInputField(const QString& labelText, QSpinBox* spinBox);

    GameSettings* m_settings;
    
    QSpinBox* m_maxBansSpinBox;
    QSpinBox* m_maxPlayersSpinBox;
    QSpinBox* m_rollBuildsCountSpinBox;
    QSpinBox* m_maxBuildsTotalSpinBox;
    QSpinBox* m_gameTimeSpinBox;
    
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
    QPushButton* m_exportButton;
};

#endif // SETTINGSDIALOG_H 