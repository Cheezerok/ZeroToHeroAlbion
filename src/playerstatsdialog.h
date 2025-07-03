#ifndef PLAYERSTATSDIALOG_H
#define PLAYERSTATSDIALOG_H

#include "playerstats.h"
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QPushButton>
#include <QMap>

class PlayerStatsDialog : public QDialog {
    Q_OBJECT
public:
    explicit PlayerStatsDialog(QWidget* parent = nullptr);
    void setPlayer(PlayerStats* player);
    
private slots:
    void onSaveClicked();
    void onCancelClicked();
    void validateInputs();
    
private:
    PlayerStats* m_player;
    QLineEdit* m_nameEdit;
    QDoubleSpinBox* m_pveFame;
    QDoubleSpinBox* m_pvpFame;
    QDoubleSpinBox* m_gatheringFame;
    QDoubleSpinBox* m_infamyPoints;
    QSpinBox* m_masteriesCount;
    QDoubleSpinBox* m_silverAmount;
    QSpinBox* m_brecilienPoints;
    QSpinBox* m_mightPoints;
    QSpinBox* m_smugglerPoints;
    QPushButton* m_saveButton;
    
    void setupUi();
    void createInputField(QGridLayout* layout, int row, const QString& label, 
                         QWidget* input, const QString& suffix = QString());
    void loadPlayerData();
    void savePlayerData();
};

#endif // PLAYERSTATSDIALOG_H 