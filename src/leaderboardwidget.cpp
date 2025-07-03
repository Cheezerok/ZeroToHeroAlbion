#include "leaderboardwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

LeaderboardWidget::LeaderboardWidget(QWidget* parent)
    : QWidget(parent)
    , m_leaderboard(nullptr)
{
    setupUi();
}

void LeaderboardWidget::setupUi()
{
    setMinimumSize(800, 600);
    setStyleSheet(
        "LeaderboardWidget {"
        "   background: #1a1a1a;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    m_titleLabel = new QLabel("TOURNAMENT LEADERBOARD", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "   border-radius: 10px;"
        "   border: 2px solid #7c4dff;"
        "}"
    );

    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect(this);
    titleShadow->setBlurRadius(10);
    titleShadow->setColor(QColor(0, 0, 0, 100));
    titleShadow->setOffset(0, 4);
    m_titleLabel->setGraphicsEffect(titleShadow);

    mainLayout->addWidget(m_titleLabel);

    // Category filter
    createCategoryFilters();

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4); // Rank, Player, Stars, Category Stars
    m_table->setHorizontalHeaderLabels({"Rank", "Player", "Total Stars", "Category Stars"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    stylizeTable();
    mainLayout->addWidget(m_table);
}

void LeaderboardWidget::createCategoryFilters()
{
    QWidget* filterWidget = new QWidget(this);
    QHBoxLayout* filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setSpacing(10);

    QLabel* filterLabel = new QLabel("Category Filter:", filterWidget);
    filterLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
        "}"
    );
    filterLayout->addWidget(filterLabel);

    QComboBox* categoryCombo = new QComboBox(filterWidget);
    categoryCombo->setStyleSheet(
        "QComboBox {"
        "   background: #2d2d2d;"
        "   color: #b388ff;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   padding: 5px;"
        "   min-width: 150px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "   border-left: 5px solid transparent;"
        "   border-right: 5px solid transparent;"
        "   border-top: 5px solid #b388ff;"
        "   width: 0;"
        "   height: 0;"
        "   margin-right: 5px;"
        "}"
        "QComboBox:hover {"
        "   border: 2px solid #7c4dff;"
        "}"
    );

    categoryCombo->addItem("All Categories");
    categoryCombo->addItem("PvE Fame");
    categoryCombo->addItem("PvP Fame");
    categoryCombo->addItem("Gathering Fame");
    categoryCombo->addItem("Infamy");
    categoryCombo->addItem("Masteries");
    categoryCombo->addItem("Silver");
    categoryCombo->addItem("Brecilien");
    categoryCombo->addItem("Might");
    categoryCombo->addItem("Smuggler");
    categoryCombo->addItem("Special");

    connect(categoryCombo, &QComboBox::currentTextChanged,
            this, &LeaderboardWidget::onCategoryFilterChanged);

    filterLayout->addWidget(categoryCombo);
    filterLayout->addStretch();

    layout()->addWidget(filterWidget);
}

void LeaderboardWidget::stylizeTable()
{
    m_table->setStyleSheet(
        "QTableWidget {"
        "   background: #2d2d2d;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 10px;"
        "   gridline-color: transparent;"
        "}"
        "QTableWidget::item {"
        "   padding: 10px;"
        "   border-bottom: 1px solid #1a1a1a;"
        "}"
        "QTableWidget::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "}"
        "QHeaderView::section {"
        "   background: #4a148c;"
        "   color: white;"
        "   padding: 10px;"
        "   border: none;"
        "   font-weight: bold;"
        "}"
        "QTableWidget::item:alternate {"
        "   background: #262626;"
        "}"
    );

    // Add shadow effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 4);
    m_table->setGraphicsEffect(shadow);
}

void LeaderboardWidget::setLeaderboard(Leaderboard* leaderboard)
{
    if (m_leaderboard) {
        disconnect(m_leaderboard, &Leaderboard::rankingsUpdated,
                  this, &LeaderboardWidget::updateDisplay);
    }

    m_leaderboard = leaderboard;

    if (m_leaderboard) {
        connect(m_leaderboard, &Leaderboard::rankingsUpdated,
                this, &LeaderboardWidget::updateDisplay);
        updateDisplay();
    }
}

void LeaderboardWidget::updateDisplay()
{
    if (!m_leaderboard) return;

    const auto rankings = m_leaderboard->getRankings();
    m_table->setRowCount(rankings.size());

    for (int i = 0; i < rankings.size(); ++i) {
        const auto& rank = rankings[i];

        // Rank
        QTableWidgetItem* rankItem = new QTableWidgetItem(QString::number(rank.rank));
        rankItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, rankItem);

        // Player name
        QTableWidgetItem* nameItem = new QTableWidgetItem(rank.name);
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_table->setItem(i, 1, nameItem);

        // Total stars
        QString starsText = QString::number(rank.stars, 'f', 1);
        if (rank.stars == static_cast<int>(rank.stars)) {
            starsText = QString::number(static_cast<int>(rank.stars));
        }
        QTableWidgetItem* starsItem = new QTableWidgetItem(starsText + "★");
        starsItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, starsItem);

        // Category stars
        if (!m_currentCategory.isEmpty() && m_currentCategory != "All Categories") {
            double categoryStars = rank.categoryStars.value(m_currentCategory, 0.0);
            QString categoryStarsText = QString::number(categoryStars, 'f', 1);
            if (categoryStars == static_cast<int>(categoryStars)) {
                categoryStarsText = QString::number(static_cast<int>(categoryStars));
            }
            QTableWidgetItem* categoryItem = new QTableWidgetItem(categoryStarsText + "★");
            categoryItem->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, 3, categoryItem);
        } else {
            m_table->setItem(i, 3, new QTableWidgetItem("-"));
        }
    }

    m_table->resizeColumnsToContents();
    m_table->horizontalHeader()->setStretchLastSection(true);
}

void LeaderboardWidget::onCategoryFilterChanged(const QString& category)
{
    m_currentCategory = category;
    updateDisplay();
    updateTableHeaders();
}

void LeaderboardWidget::updateTableHeaders()
{
    QStringList headers = {"Rank", "Player", "Total Stars"};
    if (m_currentCategory == "All Categories") {
        headers << "Category Stars";
    } else {
        headers << m_currentCategory + " Stars";
    }
    m_table->setHorizontalHeaderLabels(headers);
}

PlayerStats* LeaderboardWidget::getSelectedPlayer() const
{
    if (!m_leaderboard || !m_table) return nullptr;

    QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) return nullptr;

    int row = selectedItems.first()->row();
    QString playerName = m_table->item(row, 1)->text(); // Column 1 contains player names
    
    const auto& rankings = m_leaderboard->getRankings();
    for (const auto& rank : rankings) {
        if (rank.name == playerName) {
            return m_leaderboard->getPlayer(playerName);
        }
    }
    
    return nullptr;
} 