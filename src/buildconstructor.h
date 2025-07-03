#ifndef BUILDCONSTRUCTOR_H
#define BUILDCONSTRUCTOR_H

#include "build.h"
#include "gamesettings.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QPoint>
#include "itemslot.h"

class ItemCategory : public QWidget {
    Q_OBJECT
public:
    explicit ItemCategory(const QString& title, QWidget* parent = nullptr);
    void addItem(const QString& name, const QPixmap& icon);

private:
    QGridLayout* m_layout;
    int m_row = 0;
    int m_col = 0;
};

class BuildConstructor : public QWidget {
    Q_OBJECT
public:
    explicit BuildConstructor(GameSettings* settings, QWidget* parent = nullptr);
    
    Build getCurrentBuild() const { return m_currentBuild; }
    void setCurrentBuild(const Build& build);
    QVector<Build> getAllBuilds() const { return m_builds; }

signals:
    void buildChanged();

private slots:
    void onItemChanged(Build::Slot slot, const QString& itemName);
    void onWeaponTypeChanged(const QString& type);
    void onWeaponSelected(const QString& weapon);
    void onArmorTypeChanged(const QString& type);
    void onArmorSelected(const QString& armor);
    void onOffhandTypeChanged(const QString& type);
    void onOffhandSelected(const QString& offhand);
    void onAddBuildClicked();
    void onRemoveBuildClicked();
    void onExportBuildsClicked();
    void onBuildNameChanged(const QString& name);
    void onBuildSelected(QListWidgetItem* item);

private:
    void setupUi();
    void setupSlotPositions();
    void loadResources();
    void updateWeaponList(const QString& type);
    void updateArmorList(const QString& type);
    void updateOffhandList(const QString& type);
    void updatePreview();
    void updateBuildsList();
    void clearCurrentBuild();
    
    QLabel* m_templateBackground;
    QMap<Build::Slot, ItemSlot*> m_slots;
    Build m_currentBuild;
    QVector<Build> m_builds;
    GameSettings* m_settings;
    
    struct SlotPosition {
        int x;
        int y;
    };
    QMap<Build::Slot, SlotPosition> m_slotPositions;

    // UI elements
    QComboBox* m_weaponTypeCombo;
    QComboBox* m_weaponCombo;
    QComboBox* m_armorTypeCombo;
    QComboBox* m_armorCombo;
    QComboBox* m_offhandTypeCombo;
    QComboBox* m_offhandCombo;
    
    // Build management UI elements
    QLineEdit* m_buildNameEdit;
    QListWidget* m_buildsList;
    QPushButton* m_addBuildButton;
    QPushButton* m_removeBuildButton;
    QPushButton* m_exportBuildsButton;
    QWidget* m_buildsPanel;
};

#endif // BUILDCONSTRUCTOR_H 