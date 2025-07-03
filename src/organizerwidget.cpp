#include "organizerwidget.h"
#include "buildcard.h"
#include "buildpool.h"
#include "addbuildialog.h"
#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QLabel>
#include <QScrollArea>

OrganizerWidget::OrganizerWidget(BuildPool* buildPool, GameSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_buildPool(buildPool)
    , m_settings(settings)
    , m_mainLayout(new QVBoxLayout(this))
    , m_scrollArea(new QScrollArea(this))
    , m_scrollContent(new QWidget(m_scrollArea))
    , m_buildsLayout(new QGridLayout(m_scrollContent))
    , m_addButton(new QPushButton("ДОБАВИТЬ", this))
    , m_removeButton(new QPushButton("УДАЛИТЬ", this))
    , m_exportButton(new QPushButton("ЭКСПОРТ", this))
    , m_banButton(new QPushButton("БАН/АНБАН", this))
    , m_settingsButton(new QPushButton("НАСТРОЙКИ", this))
{
    setupUi();
    updateBuildsDisplay();
}

void OrganizerWidget::setupUi()
{
    setWindowTitle("Организатор");
    resize(800, 600);
    setStyleSheet(
        "QWidget {"
        "   background-color: #1a1a1a;"
        "   color: white;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #424242, stop:1 #212121);"
        "   border: 2px solid #7c4dff;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   border: 2px solid #b388ff;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #311b92, stop:1 #4a148c);"
        "}"
        "QScrollArea {"
        "   border: 2px solid #7c4dff;"
        "   border-radius: 8px;"
        "   background: transparent;"
        "}"
        "QScrollArea > QWidget > QWidget {"
        "   background: transparent;"
        "}"
        "QScrollBar {"
        "   background: #2d2d2d;"
        "}"
        "QScrollBar:vertical {"
        "   width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #4a148c;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0;"
        "}"
    );

    // Заголовок
    QLabel* titleLabel = new QLabel("ORGANIZER", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   border-radius: 8px;"
        "   border: 2px solid #7c4dff;"
        "}"
    );
    m_mainLayout->addWidget(titleLabel);

    // Настраиваем область прокрутки
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_scrollContent);
    m_scrollContent->setLayout(m_buildsLayout);
    m_buildsLayout->setSpacing(10);
    m_buildsLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->addWidget(m_scrollArea);

    // Создаем нижнюю панель с кнопками
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    // Добавляем кнопки
    buttonLayout->addWidget(m_banButton);
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_settingsButton);

    m_mainLayout->addLayout(buttonLayout);

    // Подключаем сигналы
    connect(m_addButton, &QPushButton::clicked, this, &OrganizerWidget::onAddClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &OrganizerWidget::onRemoveClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &OrganizerWidget::onExportClicked);
    connect(m_banButton, &QPushButton::clicked, this, &OrganizerWidget::onBanClicked);
    connect(m_settingsButton, &QPushButton::clicked, this, &OrganizerWidget::onSettingsClicked);
}

void OrganizerWidget::addBuild(const Build& build)
{
    BuildCard* card = new BuildCard(build);
    connect(card, &BuildCard::clicked, this, &OrganizerWidget::onBuildCardClicked);
    connect(card, &BuildCard::banStateChanged, this, &OrganizerWidget::onBuildCardBanChanged);
    m_buildCards.append(card);
    m_builds.append(build);
    updateBuildsDisplay(); // Обновляем весь дисплей для правильного расположения
    updateButtonStates();
    emit buildsUpdated();
}

QVector<Build> OrganizerWidget::getBuilds() const
{
    return m_builds;
}

void OrganizerWidget::onBuildCardClicked(BuildCard* card)
{
    if (!card) return;

    // Check if Control key is pressed for multi-selection
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
    bool isCtrlPressed = modifiers.testFlag(Qt::ControlModifier);

    // If Ctrl is not pressed, deselect all cards except the clicked one
    if (!isCtrlPressed) {
        for (BuildCard* otherCard : m_buildCards) {
            if (otherCard != card) {
                otherCard->setSelected(false);
            }
        }
    }

    // Toggle selection of the clicked card
    card->setSelected(!card->isSelected());
    updateButtonStates();
}

void OrganizerWidget::onBuildCardBanChanged(BuildCard* card)
{
    if (!card) return;
    
    int index = m_buildCards.indexOf(card);
    if (index >= 0 && index < m_builds.size()) {
        m_builds[index] = card->getBuild();
    }
    updateCardStyle(card);
    emit buildsUpdated();
}

void OrganizerWidget::updateCardStyle(BuildCard* card)
{
    if (!card) return;
    
    bool isSelected = card->isSelected();
    QString backgroundColor = card->isBanned() ? "rgba(61, 32, 32, 180)" : "rgba(30, 30, 30, 180)";
    QString borderColor = isSelected ? "#ffd700" : (card->isBanned() ? "#ff5252" : "#4a148c");
    
    card->setStyleSheet(QString(
        "BuildCard {"
        "   background-color: %1;"
        "   border: 2px solid %2;"
        "   border-radius: 8px;"
        "}"
    ).arg(backgroundColor, borderColor));
}

void OrganizerWidget::onBanClicked()
{
    QVector<BuildCard*> selectedCards = getSelectedCards();
    if (selectedCards.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите билд для бана/анбана");
        return;
    }
    
    for (BuildCard* card : selectedCards) {
        card->setBanned(!card->isBanned());
    }
}

void OrganizerWidget::onAddClicked()
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

    AddBuildDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<Build> builds = dialog.getBuilds();
        
        // Проверяем, не превысит ли добавление новых билдов максимальный лимит
        if (m_builds.size() + builds.size() > m_settings->getMaxBuildsTotal()) {
            QMessageBox::warning(
                this,
                "Предупреждение",
                QString("Невозможно добавить %1 билдов. Это превысит установленный лимит в %2 билдов.\nТекущее количество: %3")
                    .arg(builds.size())
                    .arg(m_settings->getMaxBuildsTotal())
                    .arg(m_builds.size())
            );
            return;
        }
        
        for (const Build& build : builds) {
            addBuild(build);
        }
    }
}

void OrganizerWidget::onRemoveClicked()
{
    QVector<BuildCard*> selectedCards = getSelectedCards();
    if (selectedCards.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите билд для удаления");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Подтверждение",
        "Вы уверены, что хотите удалить выбранные билды?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        for (BuildCard* card : selectedCards) {
            int index = m_buildCards.indexOf(card);
            if (index >= 0) {
                m_buildCards.removeAt(index);
                m_builds.removeAt(index);
                card->deleteLater();
            }
        }
        updateButtonStates();
        emit buildsUpdated();
    }
}

void OrganizerWidget::onExportClicked()
{
    if (m_builds.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Нет билдов для экспорта");
        return;
    }
    
    QString hash = m_buildPool->generatePoolHash(m_builds);
    QApplication::clipboard()->setText(hash);
    
    // Count total and banned builds
    int totalBuilds = m_builds.size();
    int bannedBuilds = 0;
    for (const Build& build : m_builds) {
        if (build.isBanned()) {
            bannedBuilds++;
        }
    }
    
    QMessageBox::information(
        this,
        "Экспорт завершен",
        QString("Хэш скопирован в буфер обмена.\nВсего билдов: %1\nЗабанено: %2\nДоступно: %3")
            .arg(totalBuilds)
            .arg(bannedBuilds)
            .arg(totalBuilds - bannedBuilds)
    );
}

void OrganizerWidget::updateBuildsDisplay()
{
    // Clear layout
    QLayoutItem* item;
    while ((item = m_buildsLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    
    // Add cards in a grid layout
    int row = 0;
    int col = 0;
    const int CARDS_PER_ROW = 3;
    
    for (BuildCard* card : m_buildCards) {
        m_buildsLayout->addWidget(card, row, col);
        
        col++;
        if (col >= CARDS_PER_ROW) {
            col = 0;
            row++;
        }
    }
}

void OrganizerWidget::updateButtonStates()
{
    bool hasSelected = !getSelectedCards().isEmpty();
    bool hasBuilds = !m_builds.isEmpty();
    
    m_banButton->setEnabled(hasSelected);
    m_removeButton->setEnabled(hasSelected);
    m_exportButton->setEnabled(hasBuilds);
}

QVector<BuildCard*> OrganizerWidget::getSelectedCards() const
{
    QVector<BuildCard*> selectedCards;
    for (BuildCard* card : m_buildCards) {
        if (card->isSelected()) {
            selectedCards.append(card);
        }
    }
    return selectedCards;
}

void OrganizerWidget::onSettingsClicked()
{
    SettingsDialog dialog(m_settings, this);
    dialog.exec();
} 