#include "mainwindow.h"
#include "organizerwidget.h"
#include "rollbuildwidget.h"
#include "starscalculatorwidget.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QMenuBar>
#include <QClipboard>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include "roledialog.h"
#include "settingsdialog.h"
#include <QGraphicsDropShadowEffect>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_buildConstructor(nullptr)
    , m_organizerWidget(nullptr)
    , m_buildsButton(nullptr)
    , m_organizerButton(nullptr)
    , m_rollBuildButton(nullptr)
    , m_starsCalculatorButton(nullptr)
    , m_startButton(nullptr)
    , m_closeButton(nullptr)
    , m_rollBuildWidget(nullptr)
    , m_starsCalculatorWidget(nullptr)
    , m_stackedWidget(new QStackedWidget(this))
    , m_settings(new GameSettings(this))
    , m_isOrganizer(false)
    , m_timer(new QTimer(this))
    , m_remainingSeconds(0)
    , m_timerLabel(nullptr)
{
    setupUi();
    showRoleDialog();
    
    // Настраиваем таймер
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTimer);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // Устанавливаем размер окна
    resize(400, 600);
    setWindowTitle("Albion Zero To Hero");
    setStyleSheet("QMainWindow { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1a1a1a, stop:1 #0a0a0a); }");

    // Создаем центральный виджет
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Создаем главное меню
    createMainMenu();
}

void MainWindow::createMainMenu()
{
    // Создаем вертикальный layout
    QVBoxLayout* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Добавляем заголовок
    QLabel* titleLabel = new QLabel("ALBION\nZERO TO HERO", m_centralWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4a148c, stop:1 #311b92);"
        "   color: #ffffff;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "   border-radius: 10px;"
        "   border: 2px solid #7c4dff;"
        "}"
    );
    mainLayout->addWidget(titleLabel);

    // Добавляем таймер (только для участников)
    if (!m_isOrganizer) {
        m_timerLabel = new QLabel("00:00", m_centralWidget);
        m_timerLabel->setAlignment(Qt::AlignCenter);
        m_timerLabel->setStyleSheet(
            "QLabel {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #424242, stop:1 #212121);"
            "   color: #ffffff;"
            "   font-size: 48px;"
            "   font-weight: bold;"
            "   padding: 20px;"
            "   border-radius: 10px;"
            "   border: 2px solid #7c4dff;"
            "   min-height: 100px;"
            "}"
        );
        mainLayout->addWidget(m_timerLabel);
    }

    // Создаем стиль для кнопок
    QString buttonStyle = 
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #424242, stop:1 #212121);"
        "   color: white;"
        "   border: 2px solid #7c4dff;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "   font-size: 18px;"
        "   min-width: 200px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   border: 2px solid #b388ff;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #311b92, stop:1 #4a148c);"
        "   border: 2px solid #7c4dff;"
        "}"
        "QPushButton:disabled {"
        "   background: #2d2d2d;"
        "   border: 2px solid #424242;"
        "   color: #757575;"
        "}";

    // Создаем кнопки
    m_buildsButton = new QPushButton("БИЛДЫ", m_centralWidget);
    m_organizerButton = new QPushButton("ОРГАНИЗАТОР", m_centralWidget);
    m_rollBuildButton = new QPushButton("РОЛЛ БИЛДА", m_centralWidget);
    m_starsCalculatorButton = new QPushButton("КАЛЬКУЛЯТОР ЗВЁЗД", m_centralWidget);
    m_closeButton = new QPushButton("ЗАКРЫТЬ", m_centralWidget);

    // Создаем кнопку СТАРТ для участников
    if (!m_isOrganizer) {
        m_startButton = new QPushButton("СТАРТ", m_centralWidget);
    }

    // Применяем стиль ко всем кнопкам
    QList<QPushButton*> buttons = {
        m_buildsButton,
        m_organizerButton,
        m_rollBuildButton,
        m_starsCalculatorButton
    };

    if (!m_isOrganizer && m_startButton) {
        buttons.append(m_startButton);
    }

    buttons.append(m_closeButton);

    for (QPushButton* button : buttons) {
        button->setStyleSheet(buttonStyle);
        button->setFixedHeight(50);
        mainLayout->addWidget(button);
    }

    // Устанавливаем видимость кнопок в зависимости от роли
    m_organizerButton->setVisible(m_isOrganizer);

    // Добавляем растягивающийся элемент перед последней кнопкой
    mainLayout->addStretch();
    mainLayout->addWidget(m_closeButton);

    // Соединяем сигналы
    connect(m_buildsButton, &QPushButton::clicked, this, &MainWindow::onBuildsClicked);
    connect(m_organizerButton, &QPushButton::clicked, this, &MainWindow::onOrganizerClicked);
    connect(m_rollBuildButton, &QPushButton::clicked, this, &MainWindow::onRollClicked);
    connect(m_starsCalculatorButton, &QPushButton::clicked, this, &MainWindow::onStarsClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &MainWindow::onCloseClicked);

    if (!m_isOrganizer && m_startButton) {
        connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    }
}

void MainWindow::onBuildsClicked()
{
    if (!m_buildConstructor) {
        m_buildConstructor = new BuildConstructor(m_settings, nullptr); // Передаем настройки
        m_buildConstructor->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_buildConstructor, &QWidget::destroyed, this, [this]() {
            m_buildConstructor = nullptr;
        });
    }
    
    m_buildConstructor->show();
    m_buildConstructor->raise();
    m_buildConstructor->activateWindow();
}

void MainWindow::onOrganizerClicked()
{
    if (!m_organizerWidget) {
        BuildPool* buildPool = new BuildPool();
        m_organizerWidget = new OrganizerWidget(buildPool, m_settings);
        m_organizerWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_organizerWidget, &QWidget::destroyed, this, [this]() {
            m_organizerWidget = nullptr;
        });
    }
    
    m_organizerWidget->show();
    m_organizerWidget->raise();
    m_organizerWidget->activateWindow();
}

void MainWindow::onRollClicked()
{
    if (!m_rollBuildWidget) {
        m_rollBuildWidget = new RollBuildWidget(m_settings, nullptr);
        m_rollBuildWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_rollBuildWidget, &QWidget::destroyed, this, [this]() {
            m_rollBuildWidget = nullptr;
        });
    }

    m_rollBuildWidget->show();
    m_rollBuildWidget->raise();
    m_rollBuildWidget->activateWindow();
}

void MainWindow::onStarsClicked()
{
    if (!m_starsCalculatorWidget) {
        m_starsCalculatorWidget = new StarsCalculatorWidget();
        m_starsCalculatorWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_starsCalculatorWidget, &QWidget::destroyed, this, [this]() {
            m_starsCalculatorWidget = nullptr;
        });
    }
    
    m_starsCalculatorWidget->show();
    m_starsCalculatorWidget->raise();
    m_starsCalculatorWidget->activateWindow();
}

void MainWindow::onCloseClicked()
{
    close();
}

void MainWindow::onStartClicked()
{
    if (!m_timer->isActive()) {
        // Запускаем таймер
        m_remainingSeconds = m_settings->getGameTimeMinutes() * 60;
        m_timer->start(1000); // Обновляем каждую секунду
        m_startButton->setText("СТОП");
        
        // Блокируем кнопки во время игры
        m_buildsButton->setEnabled(false);
        m_rollBuildButton->setEnabled(false);
        m_starsCalculatorButton->setEnabled(false);
    } else {
        // Останавливаем таймер
        m_timer->stop();
        m_startButton->setText("СТАРТ");
        
        // Разблокируем кнопки
        m_buildsButton->setEnabled(true);
        m_rollBuildButton->setEnabled(true);
        m_starsCalculatorButton->setEnabled(true);
    }
}

void MainWindow::updateTimer()
{
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        QString timeStr;
        formatTime(m_remainingSeconds, timeStr);
        m_timerLabel->setText(timeStr);
        
        // Мигаем красным когда осталось мало времени
        if (m_remainingSeconds <= 60) {
            m_timerLabel->setStyleSheet(
                "QLabel {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #b71c1c, stop:1 #d32f2f);"
                "   color: #ffffff;"
                "   font-size: 48px;"
                "   font-weight: bold;"
                "   padding: 20px;"
                "   border-radius: 10px;"
                "   border: 2px solid #ff5252;"
                "   min-height: 100px;"
                "}"
            );
        }
    } else {
        // Время вышло
        m_timer->stop();
        m_startButton->setText("СТАРТ");
        
        // Разблокируем кнопки
        m_buildsButton->setEnabled(true);
        m_rollBuildButton->setEnabled(true);
        m_starsCalculatorButton->setEnabled(true);
        
        QMessageBox::information(this, "Время вышло", "Время игры закончилось!");
    }
}

void MainWindow::formatTime(int seconds, QString& result)
{
    int minutes = seconds / 60;
    seconds = seconds % 60;
    result = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void MainWindow::showRoleDialog()
{
    RoleDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_isOrganizer = (dialog.getSelectedRole() == RoleDialog::Role::Organizer);
        
        if (!m_isOrganizer) {
            // Player needs to import settings
            if (!importSettings(dialog.getSettingsHash())) {
                QMessageBox::critical(this, "Ошибка",
                                    "Неверный формат хэша настроек.\n"
                                    "Пожалуйста, получите корректный хэш у организатора.");
                QApplication::quit();
                return;
            }
        }
        
        // Update UI based on role
        m_organizerButton->setVisible(m_isOrganizer);
    } else {
        // User cancelled role selection
        QApplication::quit();
    }
}

bool MainWindow::importSettings(const QString& hash)
{
    return m_settings->importFromHash(hash);
}

void MainWindow::showRoleSelection()
{
    // Скрываем все кнопки
    m_organizerButton->hide();
    m_playerButton->hide();
    m_leaderboardButton->hide();
    m_settingsButton->hide();
    m_timerLabel->hide();
    m_startButton->hide();
    
    // Показываем диалог выбора роли
    RoleDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        RoleDialog::Role role = dialog.getSelectedRole();
        if (role == RoleDialog::Role::Organizer) {
            // Скрываем таймер и кнопку старта для организатора
            m_timerLabel->hide();
            m_startButton->hide();
            m_organizerWidget->show();
            m_playerWidget->hide();
            m_leaderboardWidget->hide();
        } else {
            // Показываем таймер и кнопку старта для игрока
            m_timerLabel->show();
            m_startButton->show();
            m_organizerWidget->hide();
            m_playerWidget->show();
            m_leaderboardWidget->hide();
        }
    } else {
        // Если диалог был отменен, показываем кнопки снова
        m_organizerButton->show();
        m_playerButton->show();
        m_leaderboardButton->show();
        m_settingsButton->show();
    }
} 