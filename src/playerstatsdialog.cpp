#include "playerstatsdialog.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

PlayerStatsDialog::PlayerStatsDialog(QWidget* parent)
    : QDialog(parent)
    , m_player(nullptr)
{
    setupUi();
}

void PlayerStatsDialog::setupUi()
{
    setWindowTitle("Player Statistics");
    setMinimumWidth(500);
    setStyleSheet(
        "PlayerStatsDialog {"
        "   background: #1a1a1a;"
        "}"
        "QLabel {"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QLineEdit, QSpinBox, QDoubleSpinBox {"
        "   background: #2d2d2d;"
        "   color: #b388ff;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   padding: 5px;"
        "   min-width: 150px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {"
        "   border: 2px solid #7c4dff;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6a1b9a, stop:1 #4a148c);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #6a1b9a);"
        "}"
        "QPushButton:disabled {"
        "   background: #424242;"
        "   color: #9e9e9e;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title
    QLabel* titleLabel = new QLabel("PLAYER STATISTICS", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 15px;"
        "   border-radius: 8px;"
        "   border: 2px solid #7c4dff;"
        "}"
    );

    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect(this);
    titleShadow->setBlurRadius(10);
    titleShadow->setColor(QColor(0, 0, 0, 100));
    titleShadow->setOffset(0, 4);
    titleLabel->setGraphicsEffect(titleShadow);

    mainLayout->addWidget(titleLabel);

    // Form
    QWidget* formWidget = new QWidget(this);
    formWidget->setStyleSheet(
        "QWidget {"
        "   background: #2d2d2d;"
        "   border-radius: 8px;"
        "   padding: 20px;"
        "}"
    );

    QGraphicsDropShadowEffect* formShadow = new QGraphicsDropShadowEffect(this);
    formShadow->setBlurRadius(20);
    formShadow->setColor(QColor(0, 0, 0, 100));
    formShadow->setOffset(0, 4);
    formWidget->setGraphicsEffect(formShadow);

    QGridLayout* formLayout = new QGridLayout(formWidget);
    formLayout->setSpacing(15);

    // Create input fields
    m_nameEdit = new QLineEdit(this);
    m_pveFame = new QDoubleSpinBox(this);
    m_pvpFame = new QDoubleSpinBox(this);
    m_gatheringFame = new QDoubleSpinBox(this);
    m_infamyPoints = new QDoubleSpinBox(this);
    m_masteriesCount = new QSpinBox(this);
    m_silverAmount = new QDoubleSpinBox(this);
    m_brecilienPoints = new QSpinBox(this);
    m_mightPoints = new QSpinBox(this);
    m_smugglerPoints = new QSpinBox(this);

    // Configure spin boxes
    const int maxValue = 100000000;
    m_pveFame->setRange(0, maxValue);
    m_pvpFame->setRange(0, maxValue);
    m_gatheringFame->setRange(0, maxValue);
    m_infamyPoints->setRange(0, maxValue);
    m_masteriesCount->setRange(0, 1000);
    m_silverAmount->setRange(0, maxValue);
    m_brecilienPoints->setRange(0, maxValue);
    m_mightPoints->setRange(0, maxValue);
    m_smugglerPoints->setRange(0, maxValue);

    // Add fields to layout
    int row = 0;
    createInputField(formLayout, row++, "Player Name:", m_nameEdit);
    createInputField(formLayout, row++, "PvE Fame:", m_pveFame, "M");
    createInputField(formLayout, row++, "PvP Fame:", m_pvpFame, "k");
    createInputField(formLayout, row++, "Gathering Fame:", m_gatheringFame, "k");
    createInputField(formLayout, row++, "Infamy Points:", m_infamyPoints, "k");
    createInputField(formLayout, row++, "Masteries Count:", m_masteriesCount);
    createInputField(formLayout, row++, "Silver Amount:", m_silverAmount, "M");
    createInputField(formLayout, row++, "Brecilien Points:", m_brecilienPoints, "k");
    createInputField(formLayout, row++, "Might Points:", m_mightPoints, "k");
    createInputField(formLayout, row++, "Smuggler Points:", m_smugglerPoints, "k");

    mainLayout->addWidget(formWidget);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    QPushButton* cancelButton = new QPushButton("Cancel", this);
    m_saveButton = new QPushButton("Save", this);
    m_saveButton->setEnabled(false);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(m_saveButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(cancelButton, &QPushButton::clicked, this, &PlayerStatsDialog::onCancelClicked);
    connect(m_saveButton, &QPushButton::clicked, this, &PlayerStatsDialog::onSaveClicked);
    connect(m_nameEdit, &QLineEdit::textChanged, this, &PlayerStatsDialog::validateInputs);
}

void PlayerStatsDialog::createInputField(QGridLayout* layout, int row, const QString& label,
                                       QWidget* input, const QString& suffix)
{
    QLabel* labelWidget = new QLabel(label, this);
    layout->addWidget(labelWidget, row, 0);

    if (!suffix.isEmpty()) {
        QHBoxLayout* inputLayout = new QHBoxLayout();
        inputLayout->setSpacing(5);
        inputLayout->addWidget(input);
        
        QLabel* suffixLabel = new QLabel(suffix, this);
        suffixLabel->setStyleSheet("color: #b388ff;");
        inputLayout->addWidget(suffixLabel);
        inputLayout->addStretch();
        
        layout->addLayout(inputLayout, row, 1);
    } else {
        layout->addWidget(input, row, 1);
    }
}

void PlayerStatsDialog::setPlayer(PlayerStats* player)
{
    m_player = player;
    if (m_player) {
        loadPlayerData();
    }
}

void PlayerStatsDialog::loadPlayerData()
{
    if (!m_player) return;
    
    m_nameEdit->setText(m_player->getPlayerName());
    
    // Convert values to M/k for display
    const auto& achievements = m_player->getAchievements();
    
    // Find highest achieved values for each category
    double maxPveFame = 0;
    double maxPvpFame = 0;
    double maxGatheringFame = 0;
    double maxInfamyPoints = 0;
    int maxMasteries = 0;
    double maxSilver = 0;
    int maxBrecilienPoints = 0;
    int maxMightPoints = 0;
    int maxSmugglerPoints = 0;

    for (const auto& achievement : achievements) {
        if (!achievement.achieved) continue;

        if (achievement.name.endsWith("PvE")) {
            double fame = achievement.name.split(" ")[0].toDouble();
            if (achievement.name.contains("M")) fame *= 1000000;
            if (achievement.name.contains("k")) fame *= 1000;
            maxPveFame = qMax(maxPveFame, fame);
        }
        else if (achievement.name.endsWith("PvP")) {
            double fame = achievement.name.split(" ")[0].toDouble();
            if (achievement.name.contains("k")) fame *= 1000;
            maxPvpFame = qMax(maxPvpFame, fame);
        }
        else if (achievement.name.endsWith("Gathering")) {
            double fame = achievement.name.split(" ")[0].toDouble();
            if (achievement.name.contains("k")) fame *= 1000;
            maxGatheringFame = qMax(maxGatheringFame, fame);
        }
        else if (achievement.name.endsWith("Infamy")) {
            double points = achievement.name.split(" ")[0].toDouble();
            if (achievement.name.contains("k")) points *= 1000;
            maxInfamyPoints = qMax(maxInfamyPoints, points);
        }
        else if (achievement.name.endsWith("Masteries")) {
            int count = achievement.name.split(" ")[0].toInt();
            maxMasteries = qMax(maxMasteries, count);
        }
        else if (achievement.name.endsWith("Silver")) {
            double amount = achievement.name.split(" ")[0].toDouble();
            if (achievement.name.contains("M")) amount *= 1000000;
            if (achievement.name.contains("k")) amount *= 1000;
            maxSilver = qMax(maxSilver, amount);
        }
        else if (achievement.name.endsWith("Brecilien")) {
            int points = achievement.name.split(" ")[0].toInt();
            if (achievement.name.contains("k")) points *= 1000;
            maxBrecilienPoints = qMax(maxBrecilienPoints, points);
        }
        else if (achievement.name.endsWith("Might")) {
            int points = achievement.name.split(" ")[0].toInt();
            if (achievement.name.contains("k")) points *= 1000;
            maxMightPoints = qMax(maxMightPoints, points);
        }
        else if (achievement.name.endsWith("Smuggler")) {
            int points = achievement.name.split(" ")[0].toInt();
            if (achievement.name.contains("k")) points *= 1000;
            maxSmugglerPoints = qMax(maxSmugglerPoints, points);
        }
    }

    // Set values in spinboxes (convert back to M/k)
    m_pveFame->setValue(maxPveFame / 1000000.0); // Convert to M
    m_pvpFame->setValue(maxPvpFame / 1000.0); // Convert to k
    m_gatheringFame->setValue(maxGatheringFame / 1000.0); // Convert to k
    m_infamyPoints->setValue(maxInfamyPoints / 1000.0); // Convert to k
    m_masteriesCount->setValue(maxMasteries);
    m_silverAmount->setValue(maxSilver / 1000000.0); // Convert to M
    m_brecilienPoints->setValue(maxBrecilienPoints / 1000.0); // Convert to k
    m_mightPoints->setValue(maxMightPoints / 1000.0); // Convert to k
    m_smugglerPoints->setValue(maxSmugglerPoints / 1000.0); // Convert to k
}

void PlayerStatsDialog::savePlayerData()
{
    if (!m_player) return;

    // Convert values to the correct units
    double pveFame = m_pveFame->value() * 1000000; // Convert from M to actual value
    double pvpFame = m_pvpFame->value() * 1000; // Convert from k to actual value
    double gatheringFame = m_gatheringFame->value() * 1000; // Convert from k to actual value
    double infamyPoints = m_infamyPoints->value() * 1000; // Convert from k to actual value
    double silverAmount = m_silverAmount->value() * 1000000; // Convert from M to actual value
    int brecilienPoints = m_brecilienPoints->value() * 1000; // Convert from k to actual value
    int mightPoints = m_mightPoints->value() * 1000; // Convert from k to actual value
    int smugglerPoints = m_smugglerPoints->value() * 1000; // Convert from k to actual value

    // Update player stats
    m_player->checkPveFame(pveFame);
    m_player->checkPvpFame(pvpFame);
    m_player->checkGatheringFame(gatheringFame);
    m_player->checkInfamyPoints(infamyPoints);
    m_player->checkMasteriesCount(m_masteriesCount->value());
    m_player->checkSilverAmount(silverAmount);
    m_player->checkBrecilienPoints(brecilienPoints);
    m_player->checkMightPoints(mightPoints);
    m_player->checkSmugglerPoints(smugglerPoints);
}

void PlayerStatsDialog::onSaveClicked()
{
    savePlayerData();
    accept();
}

void PlayerStatsDialog::onCancelClicked()
{
    reject();
}

void PlayerStatsDialog::validateInputs()
{
    m_saveButton->setEnabled(!m_nameEdit->text().trimmed().isEmpty());
} 