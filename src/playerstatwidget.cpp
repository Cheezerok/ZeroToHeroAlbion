#include "playerstatwidget.h"
#include <QGraphicsDropShadowEffect>
#include <QPushButton>

CategoryWidget::CategoryWidget(const QString& name, QWidget* parent)
    : QWidget(parent)
    , m_isExpanded(false)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create header
    QWidget* header = new QWidget(this);
    header->setObjectName("categoryHeader");
    header->setStyleSheet(
        "QWidget#categoryHeader {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4a148c, stop:1 #311b92);"
        "   border-radius: 8px;"
        "   margin: 2px;"
        "}"
    );
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(10, 8, 10, 8);

    m_titleLabel = new QLabel(name, header);
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
    );

    m_statsLabel = new QLabel(header);
    m_statsLabel->setStyleSheet(
        "QLabel {"
        "   color: #b388ff;"
        "   font-size: 14px;"
        "}"
    );

    headerLayout->addWidget(m_titleLabel);
    headerLayout->addWidget(m_statsLabel);
    headerLayout->addStretch();

    QPushButton* expandButton = new QPushButton("▼", header);
    expandButton->setFixedSize(24, 24);
    expandButton->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: white;"
        "   border: none;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   color: #b388ff;"
        "}"
    );
    headerLayout->addWidget(expandButton);

    // Create content
    m_content = new QWidget(this);
    m_content->setVisible(false);
    m_content->setStyleSheet(
        "QWidget {"
        "   background: #2d2d2d;"
        "   border-radius: 8px;"
        "   margin: 0px 2px 2px 2px;"
        "}"
    );

    layout->addWidget(header);
    layout->addWidget(m_content);

    // Add shadow effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 2);
    header->setGraphicsEffect(shadow);

    connect(expandButton, &QPushButton::clicked, this, [this, expandButton]() {
        m_isExpanded = !m_isExpanded;
        setExpanded(m_isExpanded);
        expandButton->setText(m_isExpanded ? "▲" : "▼");
    });
}

void CategoryWidget::updateStats(double stars, int achievements)
{
    QString starsText = QString::number(stars, 'f', 1);
    if (stars == static_cast<int>(stars)) {
        starsText = QString::number(static_cast<int>(stars));
    }
    m_statsLabel->setText(QString("%1★ (%2)").arg(starsText).arg(achievements));
}

void CategoryWidget::setExpanded(bool expanded)
{
    m_isExpanded = expanded;
    m_content->setVisible(expanded);
}

PlayerStatWidget::PlayerStatWidget(QWidget* parent)
    : QWidget(parent)
    , m_player(nullptr)
{
    setupUi();
}

void PlayerStatWidget::setupUi()
{
    setMinimumWidth(300);
    setStyleSheet(
        "PlayerStatWidget {"
        "   background: #1a1a1a;"
        "}"
    );

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Player name and total stars
    QWidget* header = new QWidget(this);
    header->setObjectName("playerHeader");
    header->setStyleSheet(
        "QWidget#playerHeader {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6a1b9a, stop:1 #4a148c);"
        "   border-radius: 10px;"
        "   padding: 15px;"
        "}"
    );

    QVBoxLayout* headerLayout = new QVBoxLayout(header);
    headerLayout->setSpacing(5);

    m_nameLabel = new QLabel(header);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "}"
    );

    m_totalStarsLabel = new QLabel(header);
    m_totalStarsLabel->setStyleSheet(
        "QLabel {"
        "   color: #e1bee7;"
        "   font-size: 18px;"
        "}"
    );

    headerLayout->addWidget(m_nameLabel);
    headerLayout->addWidget(m_totalStarsLabel);

    m_mainLayout->addWidget(header);

    // Add scroll area for categories
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet(
        "QScrollArea {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QScrollArea > QWidget > QWidget { background: transparent; }"
        "QScrollBar:vertical {"
        "   background: #2d2d2d;"
        "   width: 12px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #4a148c;"
        "   min-height: 20px;"
        "   border-radius: 3px;"
        "   margin: 2px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "   background: #2d2d2d;"
        "   border-radius: 3px;"
        "}"
    );

    QWidget* scrollContent = new QWidget(m_scrollArea);
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(10);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollContent);

    m_mainLayout->addWidget(m_scrollArea);
}

void PlayerStatWidget::setPlayer(PlayerStats* player)
{
    m_player = player;
    if (m_player) {
        m_nameLabel->setText(m_player->getPlayerName());
        updateStats();
    }
}

void PlayerStatWidget::updateStats()
{
    if (!m_player) return;

    // Update total stars
    updateTotalStars();

    // Update or create category widgets
    for (const auto& category : m_player->getCategoryTotals()) {
        if (!m_categoryWidgets.contains(category.name)) {
            createCategoryWidget(category.name);
        }
        m_categoryWidgets[category.name]->updateStats(category.totalStars, category.achievementsCount);
    }
}

void PlayerStatWidget::createCategoryWidget(const QString& category)
{
    CategoryWidget* widget = new CategoryWidget(category, m_scrollArea->widget());
    m_categoryWidgets[category] = widget;
    m_scrollArea->widget()->layout()->addWidget(widget);
}

void PlayerStatWidget::updateTotalStars()
{
    if (!m_player) return;
    
    QString starsText = QString::number(m_player->getTotalStars(), 'f', 1);
    if (m_player->getTotalStars() == static_cast<int>(m_player->getTotalStars())) {
        starsText = QString::number(static_cast<int>(m_player->getTotalStars()));
    }
    m_totalStarsLabel->setText(QString("Total: %1★").arg(starsText));
} 