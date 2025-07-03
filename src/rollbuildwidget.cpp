#include "rollbuildwidget.h"
#include "buildcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLabel>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QScrollBar>
#include <QPushButton>
#include <QTimer>
#include <QLineEdit>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>

RollResultDialog::RollResultDialog(const QVector<Build>& builds, QWidget* parent)
    : QDialog(parent)
    , m_builds(builds)
{
    setWindowTitle("Результаты ролла");
    setMinimumSize(800, 600);
    setupUi();
}

void RollResultDialog::setupUi()
{
    setStyleSheet(
        "RollResultDialog {"
        "   background: #1a1a1a;"
        "}"
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
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
        "BuildCard {"
        "   background-color: rgba(30, 30, 30, 0.9);"
        "   border: 2px solid #7c4dff;"
        "   border-radius: 8px;"
        "   min-height: 200px;"
        "}"
        "BuildCard QLabel {"
        "   color: white;"
        "}"
        "BuildCard QLabel[objectName='slotIcon'] {"
        "   background: transparent;"
        "   border: none;"
        "   min-width: 32px;"
        "   min-height: 32px;"
        "}"
        "BuildCard QLabel[objectName='slotIcon'] > QPixmap {"
        "   background: transparent;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel* titleLabel = new QLabel("Выпавшие билды:", this);
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
    mainLayout->addWidget(titleLabel);
    
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    
    QWidget* container = new QWidget(scrollArea);
    QGridLayout* containerLayout = new QGridLayout(container);
    containerLayout->setSpacing(20);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    
    int row = 0;
    int col = 0;
    const int CARDS_PER_ROW = 3;
    
    for (const Build& build : m_builds) {
        BuildCard* card = new BuildCard(build);
        card->setEnabled(false);
        card->setMinimumSize(200, 200);
        containerLayout->addWidget(card, row, col);
        
        col++;
        if (col >= CARDS_PER_ROW) {
            col = 0;
            row++;
        }
    }
    
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
    
    QPushButton* closeButton = new QPushButton("Закрыть", this);
    closeButton->setMinimumHeight(40);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeButton);
}

RollBuildWidget::RollBuildWidget(GameSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_buildPool(new BuildPool())
    , m_animationTimer(new QTimer(this))
    , m_rollDuration(new QTimer(this))
    , m_animationSpeed(0.0)
    , m_isRolling(false)
{
    setupUi();
    
    connect(m_animationTimer, &QTimer::timeout, this, &RollBuildWidget::updateAnimation);
    connect(m_rollDuration, &QTimer::timeout, this, &RollBuildWidget::stopRoll);
}

void RollBuildWidget::setupUi()
{
    setMinimumSize(800, 600);
    setStyleSheet(
        "RollBuildWidget {"
        "   background: #1a1a1a;"
        "}"
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
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
        "QLineEdit {"
        "   background: #2d2d2d;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   color: white;"
        "   padding: 4px 8px;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #7c4dff;"
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
        "BuildCard {"
        "   background-color: rgba(30, 30, 30, 0.9);"
        "   border: 2px solid #4a148c;"
        "   border-radius: 8px;"
        "}"
        "BuildCard QLabel {"
        "   color: white;"
        "}"
        "BuildCard QLabel[objectName='slotIcon'] {"
        "   background: transparent;"
        "   border: none;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    QLabel* titleLabel = new QLabel("ROLL BUILD", this);
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
    mainLayout->addWidget(titleLabel);

    // Hash input section
    QVBoxLayout* hashLayout = new QVBoxLayout();
    hashLayout->setSpacing(10);

    QLabel* hashLabel = new QLabel("Хэш билдов:", this);
    hashLabel->setStyleSheet(
        "QLabel {"
        "   color: #b388ff;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
    );
    hashLayout->addWidget(hashLabel);

    m_hashInput = new QLineEdit(this);
    m_hashInput->setPlaceholderText("Вставьте хэш билдов от организатора...");
    m_hashInput->setMinimumHeight(40);
    connect(m_hashInput, &QLineEdit::textChanged, this, &RollBuildWidget::onHashEntered);
    hashLayout->addWidget(m_hashInput);

    mainLayout->addLayout(hashLayout);

    // Roll button
    m_rollButton = new QPushButton("РОЛЛ", this);
    m_rollButton->setEnabled(false);
    m_rollButton->setMinimumHeight(40);
    m_rollButton->setStyleSheet(
        "QPushButton {"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
    );
    connect(m_rollButton, &QPushButton::clicked, this, &RollBuildWidget::onRollClicked);
    mainLayout->addWidget(m_rollButton, 0, Qt::AlignCenter);

    // Scroll area for builds
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollContent = new QWidget(m_scrollArea);
    m_buildsLayout = new QGridLayout(m_scrollContent);
    m_buildsLayout->setSpacing(10);
    m_buildsLayout->setContentsMargins(10, 10, 10, 10);
    m_scrollArea->setWidget(m_scrollContent);
    mainLayout->addWidget(m_scrollArea);

    // Setup timers
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(50);
    connect(m_animationTimer, &QTimer::timeout, this, &RollBuildWidget::updateAnimation);

    m_rollDuration = new QTimer(this);
    m_rollDuration->setSingleShot(true);
    connect(m_rollDuration, &QTimer::timeout, this, &RollBuildWidget::stopRoll);
}

void RollBuildWidget::updateBuildsDisplay()
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

void RollBuildWidget::setBuilds(const QVector<Build>& builds)
{
    m_builds = builds;
    // Очищаем старые карточки
    qDeleteAll(m_buildCards);
    m_buildCards.clear();
    
    // Создаем новые карточки
    for (const Build& build : m_builds) {
        BuildCard* card = new BuildCard(build);
        card->setEnabled(false); // Карточки неактивны в режиме ролла
        m_buildCards.append(card);
    }
    
    updateBuildsDisplay();
    m_rollButton->setEnabled(!m_builds.isEmpty());
}

void RollBuildWidget::onRollClicked()
{
    if (!m_isRolling) {
        if (m_builds.isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", "Нет доступных билдов для ролла");
            return;
        }
        startRoll();
    } else {
        stopRoll();
    }
}

void RollBuildWidget::startRoll()
{
    m_isRolling = true;
    m_rollButton->setText("СТОП");
    m_animationSpeed = 50.0;
    m_animationTimer->start(100); // Замедлили немного для лучшего визуального эффекта
    m_rollDuration->start(2000); // 2 секунды анимации
    m_rolledBuilds.clear();
}

void RollBuildWidget::stopRoll()
{
    m_isRolling = false;
    m_rollButton->setText("РОЛЛ");
    m_animationTimer->stop();
    m_rollDuration->stop();
    
    // Выбираем случайные билды
    int numBuilds = m_settings->getRollBuildsCount();
    m_rolledBuilds.clear();
    
    if (m_builds.size() <= numBuilds) {
        m_rolledBuilds = m_builds;
    } else {
        QVector<int> indices;
        for (int i = 0; i < m_builds.size(); ++i) {
            indices.append(i);
        }
        
        // Перемешиваем индексы
        for (int i = indices.size() - 1; i > 0; --i) {
            int j = QRandomGenerator::global()->bounded(i + 1);
            std::swap(indices[i], indices[j]);
        }
        
        // Берем первые numBuilds билдов
        for (int i = 0; i < numBuilds; ++i) {
            m_rolledBuilds.append(m_builds[indices[i]]);
        }
    }
    
    showResults();
    
    // Восстанавливаем отображение всех билдов
    setBuilds(m_builds);
}

void RollBuildWidget::updateAnimation()
{
    if (!m_builds.isEmpty()) {
        // Очищаем текущее отображение
        qDeleteAll(m_buildCards);
        m_buildCards.clear();
        
        // Создаем случайный набор карточек
        int numBuilds = m_settings->getRollBuildsCount();
        for (int i = 0; i < numBuilds; ++i) {
            int randomIndex = QRandomGenerator::global()->bounded(m_builds.size());
            BuildCard* card = new BuildCard(m_builds[randomIndex]);
            card->setEnabled(false);
            m_buildCards.append(card);
        }
        
        // Обновляем отображение
        updateBuildsDisplay();
    }
}

void RollBuildWidget::showResults()
{
    RollResultDialog* dialog = new RollResultDialog(m_rolledBuilds, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void RollBuildWidget::onHashEntered()
{
    QString hash = m_hashInput->text().trimmed();
    if (hash.isEmpty()) {
        m_rollButton->setEnabled(false);
        return;
    }

    if (m_buildPool->importPool(hash)) {
        setBuilds(m_buildPool->getBuilds());
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Успех");
        msgBox.setText(QString("Успешно загружено %1 билдов").arg(m_builds.size()));
        msgBox.setStyleSheet(
            "QMessageBox {"
            "   background-color: #1a1a1a;"
            "   color: white;"
            "}"
            "QLabel {"
            "   color: white;"
            "   font-size: 14px;"
            "   min-width: 300px;"
            "}"
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 4px;"
            "   padding: 8px 16px;"
            "   font-size: 14px;"
            "   min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6a1b9a, stop:1 #4a148c);"
            "}"
        );
        msgBox.exec();
    } else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Неверный формат хэша");
        msgBox.setStyleSheet(
            "QMessageBox {"
            "   background-color: #1a1a1a;"
            "   color: white;"
            "}"
            "QLabel {"
            "   color: white;"
            "   font-size: 14px;"
            "   min-width: 300px;"
            "}"
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 4px;"
            "   padding: 8px 16px;"
            "   font-size: 14px;"
            "   min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6a1b9a, stop:1 #4a148c);"
            "}"
        );
        msgBox.exec();
        m_rollButton->setEnabled(false);
    }
} 