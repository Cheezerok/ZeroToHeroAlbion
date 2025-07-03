#include "buildcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFont>

BuildCard::BuildCard(const Build& build, bool isBanned, QWidget* parent)
    : QWidget(parent)
    , m_build(build)
    , m_isBanned(isBanned)
    , m_isHovered(false)
    , m_isSelected(false)
    , m_nameLabel(nullptr)
    , m_weaponLabel(nullptr)
    , m_armorLabel(nullptr)
    , m_bannedLabel(nullptr)
{
    setFixedSize(200, 250);
    setMouseTracking(true);
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    updateStyle();
}

void BuildCard::setupUi()
{
    // Create main layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    // Create build name label
    m_nameLabel = new QLabel(m_build.getName(), this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "   color: #b388ff;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   background: transparent;"
        "   padding: 2px;"
        "}"
    );
    m_nameLabel->setWordWrap(true);
    layout->addWidget(m_nameLabel);

    // Create banned label if needed
    m_bannedLabel = new QLabel("BANNED", this);
    m_bannedLabel->setAlignment(Qt::AlignCenter);
    m_bannedLabel->setStyleSheet(
        "QLabel {"
        "   color: #ff5252;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   background: transparent;"
        "   padding: 2px;"
        "}"
    );
    m_bannedLabel->setVisible(m_isBanned);
    layout->addWidget(m_bannedLabel);

    // Create items grid
    QGridLayout* itemsGrid = new QGridLayout;
    itemsGrid->setSpacing(5);

    // Helper function to create item icon
    auto createItemIcon = [this](const QString& itemName, Build::Slot slot) -> QLabel* {
        if (itemName.isEmpty()) return nullptr;

        QLabel* iconLabel = new QLabel(this);
        iconLabel->setFixedSize(40, 40);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet(
            "QLabel {"
            "   background: #2d2d2d;"
            "   border: 1px solid #4a148c;"
            "   border-radius: 4px;"
            "}"
        );

        // Load icon based on slot and item name
        QString iconPath;
        QString itemNameLower = itemName.toLower();
        
        switch (slot) {
            case Build::Slot::HEAD: {
                // Check cloth
                iconPath = QString(":/armor/cloth/cowl/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                // Check leather
                iconPath = QString(":/armor/leather/hood/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                // Check metal
                iconPath = QString(":/armor/metal/helmet/%1").arg(itemNameLower);
                break;
            }
            case Build::Slot::CHEST: {
                // Check cloth
                iconPath = QString(":/armor/cloth/robe/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                // Check leather
                iconPath = QString(":/armor/leather/jacket/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                // Check metal
                iconPath = QString(":/armor/metal/armor/%1").arg(itemNameLower);
                break;
            }
            case Build::Slot::SHOES: {
                // Check cloth
                iconPath = QString(":/armor/cloth/sandals/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                // Check leather
                iconPath = QString(":/armor/leather/shoes/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                // Check metal
                iconPath = QString(":/armor/metal/boots/%1").arg(itemNameLower);
                break;
            }
            case Build::Slot::MAIN_HAND: {
                // Try all weapon categories
                const QStringList categories = {
                    "bow", "crossbow", "sword", "axe", "dagger", "mace",
                    "spear", "hammer", "quarterstaff", "wargloves",
                    "fire", "holy", "frost", "nature", "cursed",
                    "shapeshifter", "arcane"
                };
                
                for (const QString& category : categories) {
                    iconPath = QString(":/weapons/%1/%2").arg(category, itemNameLower);
                    if (QFile::exists(iconPath)) break;
                }
                break;
            }
            case Build::Slot::OFF_HAND: {
                // Check all off-hand categories
                iconPath = QString(":/offhand/book/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                iconPath = QString(":/offhand/torch/%1").arg(itemNameLower);
                if (QFile::exists(iconPath)) break;
                
                iconPath = QString(":/offhand/shield/%1").arg(itemNameLower);
                break;
            }
            case Build::Slot::CAPE: {
                iconPath = QString(":/capes/%1").arg(itemNameLower);
                break;
            }
            default:
                break;
        }

        QPixmap icon(iconPath);
        if (!icon.isNull()) {
            icon = icon.scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            iconLabel->setPixmap(icon);
            iconLabel->setToolTip(itemName); // Add tooltip with item name
        } else {
            iconLabel->setText(itemName);
            iconLabel->setStyleSheet(
                "QLabel {"
                "   color: #e1bee7;"
                "   font-size: 10px;"
                "   background: #2d2d2d;"
                "   border: 1px solid #4a148c;"
                "   border-radius: 4px;"
                "   padding: 2px;"
                "}"
            );
            iconLabel->setWordWrap(true);
        }

        return iconLabel;
    };

    // Add item icons to grid
    if (QLabel* headIcon = createItemIcon(m_build.getItemName(Build::Slot::HEAD), Build::Slot::HEAD))
        itemsGrid->addWidget(headIcon, 0, 1);
        
    if (QLabel* capeIcon = createItemIcon(m_build.getItemName(Build::Slot::CAPE), Build::Slot::CAPE))
        itemsGrid->addWidget(capeIcon, 0, 2);

    // Handle all items independently
    QString mainHandItem = m_build.getItemName(Build::Slot::MAIN_HAND);
    QString offHandItem = m_build.getItemName(Build::Slot::OFF_HAND);

    if (!mainHandItem.isEmpty()) {
        QLabel* weaponIcon = createItemIcon(mainHandItem, Build::Slot::MAIN_HAND);
        itemsGrid->addWidget(weaponIcon, 1, 0);
    }
        
    if (QLabel* chestIcon = createItemIcon(m_build.getItemName(Build::Slot::CHEST), Build::Slot::CHEST))
        itemsGrid->addWidget(chestIcon, 1, 1);
        
    if (!offHandItem.isEmpty()) {
        QLabel* offhandIcon = createItemIcon(offHandItem, Build::Slot::OFF_HAND);
        itemsGrid->addWidget(offhandIcon, 1, 2);
    }
        
    if (QLabel* shoesIcon = createItemIcon(m_build.getItemName(Build::Slot::SHOES), Build::Slot::SHOES))
        itemsGrid->addWidget(shoesIcon, 2, 1);

    layout->addLayout(itemsGrid);
    layout->addStretch();
}

void BuildCard::updateStyle()
{
    QString backgroundColor = m_isBanned ? "rgba(61, 32, 32, 180)" : "rgba(30, 30, 30, 180)";
    QString borderColor = m_isSelected ? "#ffd700" : (m_isBanned ? "#ff5252" : (m_isHovered ? "#7c4dff" : "#4a148c"));
    
    setStyleSheet(QString(
        "BuildCard {"
        "   background-color: %1;"
        "   border: 2px solid %2;"
        "   border-radius: 8px;"
        "}"
    ).arg(backgroundColor, borderColor));
    
    // Update labels visibility
    if (m_bannedLabel) {
        m_bannedLabel->setVisible(m_isBanned);
    }
}

bool BuildCard::isBanned() const
{
    return m_isBanned;
}

void BuildCard::setBanned(bool banned)
{
    if (m_isBanned != banned) {
        m_isBanned = banned;
        m_build.setBanned(banned);
        updateStyle();
        emit banStateChanged(this);
    }
}

Build BuildCard::getBuild() const
{
    return m_build;
}

void BuildCard::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(this);
    }
    QWidget::mousePressEvent(event);
}

void BuildCard::enterEvent(QEvent* event)
{
    m_isHovered = true;
    updateStyle();
    QWidget::enterEvent(event);
}

void BuildCard::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    updateStyle();
    QWidget::leaveEvent(event);
}

void BuildCard::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

bool BuildCard::isSelected() const
{
    return m_isSelected;
}

void BuildCard::setSelected(bool selected)
{
    if (m_isSelected != selected) {
        m_isSelected = selected;
        updateStyle();
    }
} 