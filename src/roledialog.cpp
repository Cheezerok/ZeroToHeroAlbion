#include "roledialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

RoleDialog::RoleDialog(QWidget* parent)
    : QDialog(parent)
    , m_hashEdit(nullptr)
    , m_continueButton(nullptr)
{
    setupUi();
}

void RoleDialog::setupUi()
{
    setWindowTitle("Выбор роли");
    setMinimumWidth(400);
    setStyleSheet(
        "RoleDialog {"
        "   background: #1a1a1a;"
        "}"
        "QLabel {"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QLineEdit {"
        "   background: #2d2d2d;"
        "   color: #b388ff;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #7c4dff;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4a148c, stop:1 #311b92);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 12px 24px;"
        "   font-size: 16px;"
        "   min-width: 200px;"
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
    QLabel* titleLabel = new QLabel("ВЫБЕРИТЕ РОЛЬ", this);
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

    // Role buttons
    QPushButton* organizerButton = new QPushButton("ОРГАНИЗАТОР", this);
    QPushButton* playerButton = new QPushButton("УЧАСТНИК", this);

    QGraphicsDropShadowEffect* organizerShadow = new QGraphicsDropShadowEffect(this);
    organizerShadow->setBlurRadius(10);
    organizerShadow->setColor(QColor(0, 0, 0, 100));
    organizerShadow->setOffset(0, 4);
    organizerButton->setGraphicsEffect(organizerShadow);

    QGraphicsDropShadowEffect* playerShadow = new QGraphicsDropShadowEffect(this);
    playerShadow->setBlurRadius(10);
    playerShadow->setColor(QColor(0, 0, 0, 100));
    playerShadow->setOffset(0, 4);
    playerButton->setGraphicsEffect(playerShadow);

    mainLayout->addWidget(organizerButton);
    mainLayout->addWidget(playerButton);

    // Hash input (initially hidden)
    QWidget* hashWidget = new QWidget(this);
    QVBoxLayout* hashLayout = new QVBoxLayout(hashWidget);
    hashLayout->setSpacing(10);

    QLabel* hashLabel = new QLabel("Введите хэш настроек:", this);
    m_hashEdit = new QLineEdit(this);
    m_hashEdit->setPlaceholderText("Вставьте хэш настроек от организатора...");
    
    hashLayout->addWidget(hashLabel);
    hashLayout->addWidget(m_hashEdit);
    
    hashWidget->hide();
    mainLayout->addWidget(hashWidget);

    // Continue button (initially hidden)
    m_continueButton = new QPushButton("ПРОДОЛЖИТЬ", this);
    m_continueButton->hide();
    mainLayout->addWidget(m_continueButton);

    // Cancel button
    QPushButton* cancelButton = new QPushButton("ОТМЕНА", this);
    mainLayout->addWidget(cancelButton);

    // Connect signals
    connect(organizerButton, &QPushButton::clicked, this, &RoleDialog::onOrganizerClicked);
    connect(playerButton, &QPushButton::clicked, this, &RoleDialog::onPlayerClicked);
    connect(m_hashEdit, &QLineEdit::textChanged, this, &RoleDialog::onHashEntered);
    connect(m_continueButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &RoleDialog::onCancelClicked);
}

void RoleDialog::onOrganizerClicked()
{
    m_selectedRole = Role::Organizer;
    accept();
}

void RoleDialog::onPlayerClicked()
{
    m_selectedRole = Role::Player;
    showHashInput();
}

void RoleDialog::showHashInput()
{
    // Show hash input widgets
    m_hashEdit->parentWidget()->show();
    m_continueButton->show();
    m_continueButton->setEnabled(false);
    
    // Adjust dialog size
    adjustSize();
}

void RoleDialog::onHashEntered()
{
    m_continueButton->setEnabled(!m_hashEdit->text().isEmpty());
}

void RoleDialog::onCancelClicked()
{
    reject();
} 