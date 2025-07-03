#include "buildconstructor.h"
#include "itemslot.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDrag>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QScrollArea>
#include <QIcon>
#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QImageReader>
#include <QMessageBox>
#include <QClipboard>
#include <QJsonArray>
#include <QJsonDocument>
#include <QListWidgetItem>
#include <QToolTip>

ItemCategory::ItemCategory(const QString& title, QWidget* parent)
    : QWidget(parent), m_layout(new QGridLayout)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QLabel* titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("QLabel { color: #00ff00; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(m_layout);
}

void ItemCategory::addItem(const QString& name, const QPixmap& icon)
{
    QLabel* itemLabel = new QLabel(this);
    itemLabel->setPixmap(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    itemLabel->setToolTip(name);
    itemLabel->setFixedSize(48, 48);
    itemLabel->setAlignment(Qt::AlignCenter);
    itemLabel->setStyleSheet("QLabel { background-color: #2d2d2d; border: 1px solid #3d3d3d; }");
    
    m_layout->addWidget(itemLabel, m_row, m_col);
    
    m_col++;
    if (m_col >= 4) {
        m_col = 0;
        m_row++;
    }
}

BuildConstructor::BuildConstructor(GameSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_templateBackground(nullptr)
    , m_buildNameEdit(nullptr)
    , m_buildsList(nullptr)
    , m_addBuildButton(nullptr)
    , m_removeBuildButton(nullptr)
    , m_exportBuildsButton(nullptr)
{
    qDebug() << "BuildConstructor: Initializing...";
    qDebug() << "Application directory:" << QApplication::applicationDirPath();
    qDebug() << "Current directory:" << QDir::currentPath();
    
    // List all available resources
    QDirIterator it(":", QDirIterator::Subdirectories);
    qDebug() << "Available resources:";
    while (it.hasNext()) {
        qDebug() << "  " << it.next();
    }
    
    setupSlotPositions();
    setupUi();
    loadResources();
    
    qDebug() << "BuildConstructor: Initialization complete";
}

void BuildConstructor::setupSlotPositions()
{
    // Define positions for each slot relative to the template image
    m_slotPositions[Build::Slot::HEAD] = {205,55};      // Head position (top center)
    m_slotPositions[Build::Slot::CAPE] = {325, 50};     // Cape position (right center)
    m_slotPositions[Build::Slot::CHEST] = {205, 160};    // Chest position (center)
    m_slotPositions[Build::Slot::MAIN_HAND] = {83, 160}; // Main hand position (moved higher)
    m_slotPositions[Build::Slot::OFF_HAND] = {325, 160}; // Off hand position (moved higher)
    m_slotPositions[Build::Slot::SHOES] = {205, 265};    // Shoes position (bottom center)
}

void BuildConstructor::setupUi()
{
    setMinimumSize(800, 600);
    setStyleSheet("BuildConstructor { background: #1a1a1a; }");
    
    // Create main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    
    // Create left panel for build creation
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(500);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    mainLayout->addWidget(leftPanel);
    
    // Create container for template and slots
    QWidget* templateContainer = new QWidget(leftPanel);
    templateContainer->setFixedSize(500, 400);
    leftLayout->addWidget(templateContainer);
    
    // Create and setup the template background
    m_templateBackground = new QLabel(templateContainer);
    
    // Try loading the image from resources
    QPixmap templatePixmap(":/images/main.png");
    if (templatePixmap.isNull()) {
        qDebug() << "Failed to load template image from resources";
    } else {
        qDebug() << "Successfully loaded template from resources";
        templatePixmap = templatePixmap.scaled(500, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_templateBackground->setPixmap(templatePixmap);
        m_templateBackground->setGeometry(0, 0, 500, 400);
        m_templateBackground->setAlignment(Qt::AlignCenter);
        m_templateBackground->lower();
    }

    // Create slots
    m_slots[Build::Slot::HEAD] = new ItemSlot(Build::Slot::HEAD, templateContainer);
    m_slots[Build::Slot::CAPE] = new ItemSlot(Build::Slot::CAPE, templateContainer);
    m_slots[Build::Slot::CHEST] = new ItemSlot(Build::Slot::CHEST, templateContainer);
    m_slots[Build::Slot::SHOES] = new ItemSlot(Build::Slot::SHOES, templateContainer);
    m_slots[Build::Slot::MAIN_HAND] = new ItemSlot(Build::Slot::MAIN_HAND, templateContainer);
    m_slots[Build::Slot::OFF_HAND] = new ItemSlot(Build::Slot::OFF_HAND, templateContainer);

    // Position slots according to the template
    for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
        if (!it.value()) continue;
        const SlotPosition& pos = m_slotPositions[it.key()];
        it.value()->move(pos.x, pos.y);
        connect(it.value(), &ItemSlot::itemChanged, this, &BuildConstructor::onItemChanged);
    }

    // Create build management panel
    m_buildsPanel = new QWidget(leftPanel);
    QVBoxLayout* buildsPanelLayout = new QVBoxLayout(m_buildsPanel);
    leftLayout->addWidget(m_buildsPanel);

    // Add build name input
    QHBoxLayout* nameLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel("Название билда:", m_buildsPanel);
    nameLabel->setStyleSheet("QLabel { color: #b388ff; font-size: 14px; }");
    m_buildNameEdit = new QLineEdit(m_buildsPanel);
    m_buildNameEdit->setStyleSheet(
        "QLineEdit {"
        "   background: #2d2d2d;"
        "   color: #b388ff;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   padding: 5px;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #7c4dff;"
        "}"
    );
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(m_buildNameEdit);
    buildsPanelLayout->addLayout(nameLayout);

    // Add buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    
    m_addBuildButton = new QPushButton("ДОБАВИТЬ БИЛД", m_buildsPanel);
    m_removeBuildButton = new QPushButton("УДАЛИТЬ БИЛД", m_buildsPanel);
    m_exportBuildsButton = new QPushButton("ЭКСПОРТ БИЛДОВ", m_buildsPanel);

    QString buttonStyle = 
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #424242, stop:1 #212121);"
        "   color: white;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   border: 2px solid #7c4dff;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #311b92, stop:1 #4a148c);"
        "   border: 2px solid #b388ff;"
        "}";

    m_addBuildButton->setStyleSheet(buttonStyle);
    m_removeBuildButton->setStyleSheet(buttonStyle);
    m_exportBuildsButton->setStyleSheet(buttonStyle);

    buttonsLayout->addWidget(m_addBuildButton);
    buttonsLayout->addWidget(m_removeBuildButton);
    buttonsLayout->addWidget(m_exportBuildsButton);
    buildsPanelLayout->addLayout(buttonsLayout);

    // Create right panel with builds list and items
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    mainLayout->addWidget(rightPanel);

    // Add builds list
    QLabel* buildsListLabel = new QLabel("Список билдов:", rightPanel);
    buildsListLabel->setStyleSheet("QLabel { color: #b388ff; font-size: 16px; }");
    rightLayout->addWidget(buildsListLabel);

    m_buildsList = new QListWidget(rightPanel);
    m_buildsList->setStyleSheet(
        "QListWidget {"
        "   background: #2d2d2d;"
        "   color: #b388ff;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   font-size: 14px;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "}"
        "QListWidget::item:selected {"
        "   background: #4a148c;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background: #311b92;"
        "   color: white;"
        "}"
    );
    rightLayout->addWidget(m_buildsList);

    // Add scrollable item list
    QScrollArea* scrollArea = new QScrollArea(rightPanel);
    scrollArea->setWidgetResizable(true);
    rightLayout->addWidget(scrollArea);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "   background-color: transparent; "
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
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

    QWidget* itemsContainer = new QWidget(scrollArea);
    itemsContainer->setStyleSheet("background: transparent;");
    QVBoxLayout* itemsLayout = new QVBoxLayout(itemsContainer);
    
    // Update category headers style
    auto createCategoryLabel = [](const QString& text) -> QLabel* {
        QLabel* label = new QLabel(text);
        label->setStyleSheet(
            "QLabel {"
            "   color: #b388ff;"
            "   font-weight: bold;"
            "   font-size: 16px;"
            "   padding: 5px;"
            "   background: transparent;"
            "}"
        );
        return label;
    };

    // Update subcategory headers style
    auto createSubcategoryLabel = [](const QString& text) -> QLabel* {
        QLabel* label = new QLabel(text);
        label->setStyleSheet(
            "QLabel {"
            "   color: #7c4dff;"
            "   margin-left: 10px;"
            "   font-size: 14px;"
            "}"
        );
        return label;
    };

    // Update button style for items
    auto createItemButton = [](const QString& iconPath, const QString& tooltip) -> QPushButton* {
        QPushButton* button = new QPushButton;
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(32, 32));
        button->setFixedSize(40, 40);
        
        // Convert filename to display name
        QString displayName = tooltip;
        displayName[0] = displayName[0].toUpper();
        button->setToolTip(QString("<span style='color: #b388ff;'>%1</span>").arg(displayName));
        
        button->setStyleSheet(
            "QPushButton {"
            "   background: #2d2d2d;"
            "   border: 1px solid #4a148c;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background: #311b92;"
            "   border: 1px solid #7c4dff;"
            "}"
        );
        return button;
    };

    // Add weapon categories with updated styles
    itemsLayout->addWidget(createCategoryLabel("Weapons"));

    // Add Magic Weapons section
    itemsLayout->addWidget(createSubcategoryLabel("Magic Weapons"));

    // Add all magic weapon types
    QStringList magicTypes = {
        "arcane", "cursed", "fire", "frost", "holy", "nature", "shapeshifter"
    };

    for (const QString& type : magicTypes) {
        // Add section header
        QString headerText = type;
        headerText[0] = headerText[0].toUpper();
        headerText += " Staffs";
        
        auto* header = new QLabel(headerText, itemsContainer);
        header->setStyleSheet("QLabel { color: #7c4dff; margin-left: 20px; }");
        itemsLayout->addWidget(header);
        
        // Add section container
        auto* section = new QWidget(itemsContainer);
        auto* layout = new QGridLayout(section);
        layout->setSpacing(2);
        
        // Get all weapons of this type from resources
        QString resourcePath = QString(":/weapons/%1").arg(type);
        QDir resourceDir(resourcePath);
        QStringList entries = resourceDir.entryList(QDir::Files);
        
        int row = 0, col = 0;
        for (const QString& entry : entries) {
            QString baseName = entry;
            
            QString iconPath = QString(":/weapons/%1/%2").arg(type).arg(baseName);
            QPixmap icon(iconPath);
            
            if (!icon.isNull()) {
                QPushButton* button = createItemButton(iconPath, baseName);
                layout->addWidget(button, row, col);
                
                connect(button, &QPushButton::pressed, this, [this, baseName, icon]() {
                    QDrag* drag = new QDrag(this);
                    QMimeData* mimeData = new QMimeData;
                    
                    QByteArray itemData;
                    QDataStream stream(&itemData, QIODevice::WriteOnly);
                    stream << baseName << icon;
                    
                    mimeData->setData("application/x-item", itemData);
                    drag->setMimeData(mimeData);
                    drag->setPixmap(icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    drag->setHotSpot(QPoint(20, 20));
                    
                    drag->exec(Qt::CopyAction | Qt::MoveAction);
                });
                
                col++;
                if (col >= 4) {
                    col = 0;
                    row++;
                }
            }
        }
        
        itemsLayout->addWidget(section);
    }
    
    // Add Melee Weapons section
    itemsLayout->addWidget(createSubcategoryLabel("Melee Weapons"));

    // Add all melee weapon types
    QStringList meleeTypes = {
        "axe", "dagger", "hammer", "mace", "quarterstaff", "spear", "sword", "wargloves"
    };

    for (const QString& type : meleeTypes) {
        // Add section header
        QString headerText = type;
        headerText[0] = headerText[0].toUpper();
        if (type == "quarterstaff") {
            headerText = "Quarterstaves";
        } else {
            headerText += "s";
        }
        
        auto* header = new QLabel(headerText, itemsContainer);
        header->setStyleSheet("QLabel { color: #7c4dff; margin-left: 20px; }");
        itemsLayout->addWidget(header);
        
        // Add section container
        auto* section = new QWidget(itemsContainer);
        auto* layout = new QGridLayout(section);
        layout->setSpacing(2);
        
        // Get all weapons of this type from resources
        QString resourcePath = QString(":/weapons/%1").arg(type);
        QDir resourceDir(resourcePath);
        QStringList entries = resourceDir.entryList(QDir::Files);
        
        int row = 0, col = 0;
        for (const QString& entry : entries) {
            QString baseName = entry;
            
            QString iconPath = QString(":/weapons/%1/%2").arg(type).arg(baseName);
            QPixmap icon(iconPath);
            
            if (!icon.isNull()) {
                QPushButton* button = createItemButton(iconPath, baseName);
                layout->addWidget(button, row, col);
                
                connect(button, &QPushButton::pressed, this, [this, baseName, icon]() {
                    QDrag* drag = new QDrag(this);
                    QMimeData* mimeData = new QMimeData;
                    
                    QByteArray itemData;
                    QDataStream stream(&itemData, QIODevice::WriteOnly);
                    stream << baseName << icon;
                    
                    mimeData->setData("application/x-item", itemData);
                    drag->setMimeData(mimeData);
                    drag->setPixmap(icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    drag->setHotSpot(QPoint(20, 20));
                    
                    drag->exec(Qt::CopyAction | Qt::MoveAction);
                });
                
                col++;
                if (col >= 4) {
                    col = 0;
                    row++;
                }
            }
        }
        
        itemsLayout->addWidget(section);
    }
    
    // Add Ranged Weapons section
    itemsLayout->addWidget(createSubcategoryLabel("Ranged Weapons"));

    // Add all ranged weapon types
    QStringList rangedTypes = {"bow", "crossbow"};

    for (const QString& type : rangedTypes) {
        // Add section header
        QString headerText = type;
        headerText[0] = headerText[0].toUpper();
        headerText += "s";
        
        auto* header = new QLabel(headerText, itemsContainer);
        header->setStyleSheet("QLabel { color: #7c4dff; margin-left: 20px; }");
        itemsLayout->addWidget(header);
        
        // Add section container
        auto* section = new QWidget(itemsContainer);
        auto* layout = new QGridLayout(section);
        layout->setSpacing(2);
        
        // Get all weapons of this type from resources
        QString resourcePath = QString(":/weapons/%1").arg(type);
        QDir resourceDir(resourcePath);
        QStringList entries = resourceDir.entryList(QDir::Files);
        
        int row = 0, col = 0;
        for (const QString& entry : entries) {
            QString baseName = entry;
            
            QString iconPath = QString(":/weapons/%1/%2").arg(type).arg(baseName);
            QPixmap icon(iconPath);
            
            if (!icon.isNull()) {
                QPushButton* button = createItemButton(iconPath, baseName);
                layout->addWidget(button, row, col);
                
                connect(button, &QPushButton::pressed, this, [this, baseName, icon]() {
                    QDrag* drag = new QDrag(this);
                    QMimeData* mimeData = new QMimeData;
                    
                    QByteArray itemData;
                    QDataStream stream(&itemData, QIODevice::WriteOnly);
                    stream << baseName << icon;
                    
                    mimeData->setData("application/x-item", itemData);
                    drag->setMimeData(mimeData);
                    drag->setPixmap(icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    drag->setHotSpot(QPoint(20, 20));
                    
                    drag->exec(Qt::CopyAction | Qt::MoveAction);
                });
                
                col++;
                if (col >= 4) {
                    col = 0;
                    row++;
                }
            }
        }
        
        itemsLayout->addWidget(section);
    }
    
    // Add Armor category
    itemsLayout->addWidget(createCategoryLabel("Armor"));

    // Add armor types
    QMap<QString, QStringList> armorTypes = {
        {"Cloth", {"cowl", "robe", "sandals"}},
        {"Leather", {"hood", "jacket", "shoes"}},
        {"Metal", {"helmet", "armor", "boots"}}
    };

    for (auto it = armorTypes.begin(); it != armorTypes.end(); ++it) {
        // Add armor type header
        QString typeHeader = it.key() + " Armor";
        auto* header = new QLabel(typeHeader, itemsContainer);
        header->setStyleSheet("QLabel { color: #7c4dff; margin-left: 20px; }");
        itemsLayout->addWidget(header);

        QString armorType = it.key().toLower();
        for (const QString& slot : it.value()) {
            // Add slot header
            QString slotHeader = slot;
            slotHeader[0] = slotHeader[0].toUpper();
            if (slot != "armor") {
                slotHeader += "s";
            }
            
            auto* slotHeaderLabel = new QLabel(slotHeader, itemsContainer);
            slotHeaderLabel->setStyleSheet("QLabel { color: #7c4dff; margin-left: 20px; }");
            itemsLayout->addWidget(slotHeaderLabel);

            // Add section container
            auto* section = new QWidget(itemsContainer);
            auto* layout = new QGridLayout(section);
            layout->setSpacing(2);

            // Get all items of this type from resources
            QString resourcePath = QString(":/armor/%1/%2").arg(armorType).arg(slot);
            QDir resourceDir(resourcePath);
            QStringList entries = resourceDir.entryList(QDir::Files);

            int row = 0, col = 0;
            for (const QString& entry : entries) {
                QString baseName = entry;
                
                QString iconPath = QString(":/armor/%1/%2/%3").arg(armorType).arg(slot).arg(baseName);
                QPixmap icon(iconPath);

                if (!icon.isNull()) {
                    QPushButton* button = createItemButton(iconPath, baseName);
                    layout->addWidget(button, row, col);
                    
                    connect(button, &QPushButton::pressed, this, [this, baseName, icon]() {
                        QDrag* drag = new QDrag(this);
                        QMimeData* mimeData = new QMimeData;

                        QByteArray itemData;
                        QDataStream stream(&itemData, QIODevice::WriteOnly);
                        stream << baseName << icon;

                        mimeData->setData("application/x-item", itemData);
                        drag->setMimeData(mimeData);
                        drag->setPixmap(icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        drag->setHotSpot(QPoint(20, 20));

                        drag->exec(Qt::CopyAction | Qt::MoveAction);
                    });

                    col++;
                    if (col >= 4) {
                        col = 0;
                        row++;
                    }
                }
            }

            itemsLayout->addWidget(section);
        }
    }

    // Add Off-hand category
    itemsLayout->addWidget(createCategoryLabel("Off-hand Items"));

    // Add off-hand types
    QStringList offHandTypes = {"book", "shield", "torch"};

    for (const QString& type : offHandTypes) {
        // Add type header
        auto* header = new QLabel(type + "s", itemsContainer);
        header->setStyleSheet("QLabel { color: #7c4dff; margin-left: 20px; }");
        itemsLayout->addWidget(header);

        // Add section container
        auto* section = new QWidget(itemsContainer);
        auto* layout = new QGridLayout(section);
        layout->setSpacing(2);

        // Get all items of this type from resources
        QString resourcePath = QString(":/offhand/%1").arg(type);
        QDir resourceDir(resourcePath);
        QStringList entries = resourceDir.entryList(QDir::Files);

        int row = 0, col = 0;
        for (const QString& entry : entries) {
            QString baseName = entry;
            
            QString iconPath = QString(":/offhand/%1/%2").arg(type).arg(baseName);
            QPixmap icon(iconPath);

            if (!icon.isNull()) {
                QPushButton* button = createItemButton(iconPath, baseName);
                layout->addWidget(button, row, col);
                
                connect(button, &QPushButton::pressed, this, [this, baseName, icon]() {
                    QDrag* drag = new QDrag(this);
                    QMimeData* mimeData = new QMimeData;

                    QByteArray itemData;
                    QDataStream stream(&itemData, QIODevice::WriteOnly);
                    stream << baseName << icon;

                    mimeData->setData("application/x-item", itemData);
                    drag->setMimeData(mimeData);
                    drag->setPixmap(icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    drag->setHotSpot(QPoint(20, 20));

                    drag->exec(Qt::CopyAction | Qt::MoveAction);
                });

                col++;
                if (col >= 4) {
                    col = 0;
                    row++;
                }
            }
        }

        itemsLayout->addWidget(section);
    }

    // Add Capes category
    itemsLayout->addWidget(createCategoryLabel("Capes"));

    // Add capes section
    auto* capesSection = new QWidget(itemsContainer);
    auto* capesLayout = new QGridLayout(capesSection);
    capesLayout->setSpacing(2);

    // Get all capes from resources
    QDir capesDir(":/capes");
    QStringList capes = capesDir.entryList(QDir::Files);

    int row = 0, col = 0;
    for (const QString& cape : capes) {
        QString baseName = cape;
        
        QString iconPath = QString(":/capes/%1").arg(baseName);
        QPixmap icon(iconPath);

        if (!icon.isNull()) {
            QPushButton* button = createItemButton(iconPath, baseName);
            capesLayout->addWidget(button, row, col);
            
            connect(button, &QPushButton::pressed, this, [this, baseName, icon]() {
                QDrag* drag = new QDrag(this);
                QMimeData* mimeData = new QMimeData;

                QByteArray itemData;
                QDataStream stream(&itemData, QIODevice::WriteOnly);
                stream << baseName << icon;

                mimeData->setData("application/x-item", itemData);
                drag->setMimeData(mimeData);
                drag->setPixmap(icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                drag->setHotSpot(QPoint(20, 20));

                drag->exec(Qt::CopyAction | Qt::MoveAction);
            });

            col++;
            if (col >= 4) {
                col = 0;
                row++;
            }
        }
    }

    itemsLayout->addWidget(capesSection);
    scrollArea->setWidget(itemsContainer);

    // Connect signals
    connect(m_addBuildButton, &QPushButton::clicked, this, &BuildConstructor::onAddBuildClicked);
    connect(m_removeBuildButton, &QPushButton::clicked, this, &BuildConstructor::onRemoveBuildClicked);
    connect(m_exportBuildsButton, &QPushButton::clicked, this, &BuildConstructor::onExportBuildsClicked);
    connect(m_buildNameEdit, &QLineEdit::textChanged, this, &BuildConstructor::onBuildNameChanged);
    connect(m_buildsList, &QListWidget::itemClicked, this, &BuildConstructor::onBuildSelected);
}

void BuildConstructor::onAddBuildClicked()
{
    // Проверяем, не превышен ли лимит билдов
    if (m_builds.size() >= m_settings->getMaxBuildsTotal()) {
        QMessageBox::warning(
            this,
            "Предупреждение",
            QString("Достигнут лимит билдов (%1).\nУдалите существующие билды, чтобы добавить новые.")
                .arg(m_settings->getMaxBuildsTotal())
        );
        return;
    }

    QString buildName = m_buildNameEdit->text().trimmed();
    if (buildName.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Введите название билда");
        return;
    }

    // Добавляем билд
    m_currentBuild.setName(buildName);
    m_builds.append(m_currentBuild);
    
    // Обновляем список и очищаем текущий билд
    updateBuildsList();
    clearCurrentBuild();
    m_buildNameEdit->clear();
}

void BuildConstructor::onRemoveBuildClicked()
{
    QListWidgetItem* currentItem = m_buildsList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Ошибка", "Выберите билд для удаления");
        return;
    }

    int index = m_buildsList->row(currentItem);
    m_builds.removeAt(index);
    updateBuildsList();
}

void BuildConstructor::onExportBuildsClicked()
{
    if (m_builds.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет билдов для экспорта");
        return;
    }

    QJsonArray buildsArray;
    for (const Build& build : m_builds) {
        QJsonObject buildObject;
        buildObject["name"] = build.getName();
        
        QJsonObject itemsObject;
        for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
            if (build.hasItem(it.key())) {
                itemsObject[QString::number(static_cast<int>(it.key()))] = build.getItemName(it.key());
            }
        }
        buildObject["items"] = itemsObject;
        buildsArray.append(buildObject);
    }

    QJsonDocument doc(buildsArray);
    QByteArray json = doc.toJson();
    QString hash = QString::fromLatin1(json.toBase64());

    // Копируем хэш в буфер обмена
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(hash);

    QMessageBox::information(this, "Экспорт билдов",
        QString("Хэш %1 билдов скопирован в буфер обмена:\n\n%2").arg(m_builds.size()).arg(hash));
}

void BuildConstructor::onBuildSelected(QListWidgetItem* item)
{
    int index = m_buildsList->row(item);
    if (index >= 0 && index < m_builds.size()) {
        setCurrentBuild(m_builds[index]);
        m_buildNameEdit->setText(m_builds[index].getName());
    }
}

void BuildConstructor::updateBuildsList()
{
    m_buildsList->clear();
    for (const Build& build : m_builds) {
        m_buildsList->addItem(build.getName());
    }
}

void BuildConstructor::clearCurrentBuild()
{
    for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
        if (it.value()) {
            it.value()->clearItem();
        }
    }
    m_currentBuild = Build();
}

void BuildConstructor::onBuildNameChanged(const QString& name)
{
    m_currentBuild.setName(name);
}

void BuildConstructor::onItemChanged(Build::Slot slot, const QString& itemName)
{
    m_currentBuild.setItem(slot, itemName);
    emit buildChanged();
}

void BuildConstructor::setCurrentBuild(const Build& build)
{
    m_currentBuild = build;
    
    // Update all slots
    for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
        Build::Slot slot = it.key();
        ItemSlot* itemSlot = it.value();
        
        if (build.hasItem(slot)) {
            itemSlot->setItem(build.getItemName(slot));
        } else {
            itemSlot->clearItem();
        }
    }
}

void BuildConstructor::onWeaponTypeChanged(const QString& type)
{
    updateWeaponList(type);
}

void BuildConstructor::onWeaponSelected(const QString& weapon)
{
    if (!weapon.isEmpty()) {
        m_currentBuild.setItem(Build::Slot::MAIN_HAND, weapon);
        updatePreview();
    }
}

void BuildConstructor::onArmorTypeChanged(const QString& type)
{
    updateArmorList(type);
}

void BuildConstructor::onArmorSelected(const QString& armor)
{
    if (!armor.isEmpty()) {
        m_currentBuild.setItem(Build::Slot::CHEST, armor);
        updatePreview();
    }
}

void BuildConstructor::onOffhandTypeChanged(const QString& type)
{
    updateOffhandList(type);
}

void BuildConstructor::onOffhandSelected(const QString& offhand)
{
    if (!offhand.isEmpty()) {
        m_currentBuild.setItem(Build::Slot::OFF_HAND, offhand);
        updatePreview();
    }
}

void BuildConstructor::updateWeaponList(const QString& type)
{
    m_weaponCombo->clear();
    
    QString resourcePath = QString(":/weapons/%1").arg(type.toLower());
    QDir resourceDir(resourcePath);
    
    foreach(QString fileName, resourceDir.entryList(QDir::Files)) {
        QString baseName = fileName.section('.', 0, 0);
        QString iconPath = QString(":/weapons/%1/%2").arg(type.toLower()).arg(fileName);
        QPixmap icon(iconPath);
        
        if (!icon.isNull()) {
            m_weaponCombo->addItem(QIcon(icon), baseName);
        }
    }
}

void BuildConstructor::updateArmorList(const QString& type)
{
    m_armorCombo->clear();
    
    QString resourcePath = QString(":/armor/%1").arg(type.toLower());
    QDir resourceDir(resourcePath);
    
    foreach(QString fileName, resourceDir.entryList(QDir::Files)) {
        QString baseName = fileName.section('.', 0, 0);
        QString iconPath = QString(":/armor/%1/%2").arg(type.toLower()).arg(fileName);
        QPixmap icon(iconPath);
        
        if (!icon.isNull()) {
            m_armorCombo->addItem(QIcon(icon), baseName);
        }
    }
}

void BuildConstructor::updateOffhandList(const QString& type)
{
    m_offhandCombo->clear();
    
    QString resourcePath = QString(":/offhand/%1").arg(type.toLower());
    QDir resourceDir(resourcePath);
    
    foreach(QString fileName, resourceDir.entryList(QDir::Files)) {
        QString baseName = fileName.section('.', 0, 0);
        QString iconPath = QString(":/offhand/%1/%2").arg(type.toLower()).arg(fileName);
        QPixmap icon(iconPath);
        
        if (!icon.isNull()) {
            m_offhandCombo->addItem(QIcon(icon), baseName);
        }
    }
}

void BuildConstructor::updatePreview()
{
    // Update slot icons based on current build
    for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
        Build::Slot slot = it.key();
        ItemSlot* itemSlot = it.value();
        
        if (m_currentBuild.hasItem(slot)) {
            QString itemName = m_currentBuild.getItemName(slot);
            QString type;
            
            // Determine item type and path
            switch (slot) {
                case Build::Slot::MAIN_HAND:
                    type = m_weaponTypeCombo->currentText().toLower();
                    break;
                case Build::Slot::CHEST:
                    type = m_armorTypeCombo->currentText().toLower();
                    break;
                case Build::Slot::OFF_HAND:
                    type = m_offhandTypeCombo->currentText().toLower();
                    break;
                default:
                    continue;
            }
            
            // Load and set icon
            QString iconPath;
            switch (slot) {
                case Build::Slot::MAIN_HAND:
                    iconPath = QString(":/weapons/%1/%2.png").arg(type).arg(itemName);
                    break;
                case Build::Slot::CHEST:
                    iconPath = QString(":/armor/%1/%2.png").arg(type).arg(itemName);
                    break;
                case Build::Slot::OFF_HAND:
                    iconPath = QString(":/offhand/%1/%2.png").arg(type).arg(itemName);
                    break;
                default:
                    continue;
            }
            
            QPixmap icon(iconPath);
            if (!icon.isNull()) {
                itemSlot->setItem(itemName, icon);
            }
        }
    }
    
    emit buildChanged();
}

void BuildConstructor::loadResources()
{
    // В данном случае нам не нужно загружать ресурсы, так как они уже загружаются в setupUi
} 