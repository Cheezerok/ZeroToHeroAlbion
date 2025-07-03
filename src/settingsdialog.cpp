#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>

SettingsDialog::SettingsDialog(GameSettings* settings, QWidget* parent)
    : QDialog(parent)
    , m_settings(settings)
    , m_maxBansSpinBox(new QSpinBox(this))
    , m_maxPlayersSpinBox(new QSpinBox(this))
    , m_rollBuildsCountSpinBox(new QSpinBox(this))
    , m_maxBuildsTotalSpinBox(new QSpinBox(this))
    , m_gameTimeSpinBox(new QSpinBox(this))
    , m_saveButton(new QPushButton("СОХРАНИТЬ", this))
    , m_cancelButton(new QPushButton("ОТМЕНА", this))
    , m_exportButton(new QPushButton("ЭКСПОРТ", this))
{
    setupUi();
    loadSettings();
}

void SettingsDialog::setupUi()
{
    setWindowTitle("Настройки");
    setFixedSize(400, 500);
    setStyleSheet(
        "QDialog {"
        "   background-color: #1a1a1a;"
        "   color: white;"
        "}"
        "QLabel {"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QSpinBox {"
        "   background-color: #2d2d2d;"
        "   color: white;"
        "   border: 2px solid #7c4dff;"
        "   border-radius: 4px;"
        "   padding: 5px;"
        "   min-width: 100px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #424242, stop:1 #212121);"
        "   color: white;"
        "   border: 2px solid #7c4dff;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   border: 2px solid #b388ff;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок
    QLabel* titleLabel = new QLabel("НАСТРОЙКИ ИГРЫ", this);
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
    mainLayout->addWidget(titleLabel);

    // Настройка спинбоксов
    m_maxBansSpinBox->setRange(0, 100);
    m_maxPlayersSpinBox->setRange(2, 100);
    m_rollBuildsCountSpinBox->setRange(1, 10);
    m_maxBuildsTotalSpinBox->setRange(1, 1000);
    m_gameTimeSpinBox->setRange(1, 180);

    // Добавляем поля ввода
    mainLayout->addWidget(createInputField("Количество банов:", m_maxBansSpinBox));
    mainLayout->addWidget(createInputField("Количество участников:", m_maxPlayersSpinBox));
    mainLayout->addWidget(createInputField("Количество билдов на ролл:", m_rollBuildsCountSpinBox));
    mainLayout->addWidget(createInputField("Максимум билдов:", m_maxBuildsTotalSpinBox));
    mainLayout->addWidget(createInputField("Время партии (мин):", m_gameTimeSpinBox));

    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Подключаем сигналы
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &SettingsDialog::onExportClicked);
}

QWidget* SettingsDialog::createInputField(const QString& labelText, QSpinBox* spinBox)
{
    QWidget* container = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel* label = new QLabel(labelText, container);
    label->setMinimumWidth(200);
    
    layout->addWidget(label);
    layout->addWidget(spinBox);
    layout->addStretch();

    return container;
}

void SettingsDialog::loadSettings()
{
    m_maxBansSpinBox->setValue(m_settings->getMaxBans());
    m_maxPlayersSpinBox->setValue(m_settings->getMaxPlayers());
    m_rollBuildsCountSpinBox->setValue(m_settings->getRollBuildsCount());
    m_maxBuildsTotalSpinBox->setValue(m_settings->getMaxBuildsTotal());
    m_gameTimeSpinBox->setValue(m_settings->getGameTimeMinutes());
}

void SettingsDialog::saveSettings()
{
    m_settings->setMaxBans(m_maxBansSpinBox->value());
    m_settings->setMaxPlayers(m_maxPlayersSpinBox->value());
    m_settings->setRollBuildsCount(m_rollBuildsCountSpinBox->value());
    m_settings->setMaxBuildsTotal(m_maxBuildsTotalSpinBox->value());
    m_settings->setGameTimeMinutes(m_gameTimeSpinBox->value());
}

void SettingsDialog::onSaveClicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

void SettingsDialog::onExportClicked()
{
    saveSettings();
    QString hash = m_settings->exportToHash();
    QApplication::clipboard()->setText(hash);
    
    QMessageBox::information(
        this,
        "Экспорт настроек",
        "Хэш настроек скопирован в буфер обмена.\n"
        "Отправьте его участникам для импорта настроек."
    );
} 