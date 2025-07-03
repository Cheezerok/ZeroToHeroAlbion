#include "starscalculatorwidget.h"
#include "leaderboardwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>

StarsCalculatorWidget::StarsCalculatorWidget(QWidget* parent)
    : QWidget(parent)
    , m_leaderboard(new Leaderboard(this))
    , m_leaderboardWidget(new LeaderboardWidget(this))
    , m_addPlayerButton(new QPushButton("Добавить игрока", this))
    , m_editPlayerButton(new QPushButton("Редактировать", this))
    , m_removePlayerButton(new QPushButton("Удалить", this))
{
    setupUi();
    m_leaderboardWidget->setLeaderboard(m_leaderboard);
    updateButtons();
}

void StarsCalculatorWidget::setupUi()
{
    setMinimumSize(1000, 600);
    setWindowTitle("Калькулятор звёзд");
    
    setStyleSheet(
        "StarsCalculatorWidget {"
        "   background: #1a1a1a;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "   min-width: 120px;"
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
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Buttons panel
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(m_addPlayerButton);
    buttonLayout->addWidget(m_editPlayerButton);
    buttonLayout->addWidget(m_removePlayerButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_leaderboardWidget);

    // Connect signals
    connect(m_addPlayerButton, &QPushButton::clicked, this, &StarsCalculatorWidget::onAddPlayerClicked);
    connect(m_editPlayerButton, &QPushButton::clicked, this, &StarsCalculatorWidget::onEditPlayerClicked);
    connect(m_removePlayerButton, &QPushButton::clicked, this, &StarsCalculatorWidget::onRemovePlayerClicked);
}

void StarsCalculatorWidget::onAddPlayerClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Новый игрок",
                                       "Введите имя игрока:", QLineEdit::Normal,
                                       "", &ok);
    if (ok && !name.isEmpty()) {
        PlayerStats* player = new PlayerStats(name);
        PlayerStatsDialog* dialog = new PlayerStatsDialog(this);
        dialog->setPlayer(player);
        
        if (dialog->exec() == QDialog::Accepted) {
            m_leaderboard->addPlayer(player);
            updateButtons();
        } else {
            delete player;
        }
    }
}

void StarsCalculatorWidget::onEditPlayerClicked()
{
    auto selectedPlayer = m_leaderboardWidget->getSelectedPlayer();
    if (!selectedPlayer) {
        QMessageBox::warning(this, "Ошибка", "Выберите игрока для редактирования");
        return;
    }

    PlayerStatsDialog* dialog = new PlayerStatsDialog(this);
    dialog->setPlayer(selectedPlayer);
    
    if (dialog->exec() == QDialog::Accepted) {
        m_leaderboard->updateRankings();
        m_leaderboardWidget->updateDisplay();
    }
    
    delete dialog;
}

void StarsCalculatorWidget::onRemovePlayerClicked()
{
    auto selectedPlayer = m_leaderboardWidget->getSelectedPlayer();
    if (!selectedPlayer) {
        QMessageBox::warning(this, "Ошибка", "Выберите игрока для удаления");
        return;
    }

    if (QMessageBox::question(this, "Подтверждение",
                            QString("Вы уверены, что хотите удалить игрока %1?")
                            .arg(selectedPlayer->getPlayerName()),
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_leaderboard->removePlayer(selectedPlayer->getPlayerName());
        updateButtons();
    }
}

void StarsCalculatorWidget::updateButtons()
{
    bool hasPlayers = m_leaderboard->getPlayerCount() > 0;
    m_editPlayerButton->setEnabled(hasPlayers);
    m_removePlayerButton->setEnabled(hasPlayers);
} 