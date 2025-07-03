#include "addbuildialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QJsonDocument>
#include <QJsonArray>

AddBuildDialog::AddBuildDialog(QWidget* parent)
    : QDialog(parent)
    , m_hashInput(nullptr)
    , m_addButton(nullptr)
    , m_errorLabel(nullptr)
{
    setWindowTitle("Add Builds");
    setFixedSize(800, 300);
    setupUi();
}

void AddBuildDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Create description label
    QLabel* descLabel = new QLabel(
        "Enter a build hash to add builds to the organizer.\n"
        "You can paste a hash from your clipboard or enter it manually.", 
        this
    );
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet(
        "QLabel {"
        "   color: #e1bee7;"
        "   font-size: 14px;"
        "   background: transparent;"
        "}"
    );
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);

    // Create hash input
    QHBoxLayout* hashLayout = new QHBoxLayout;
    hashLayout->setSpacing(10);

    m_hashInput = new QLineEdit(this);
    m_hashInput->setPlaceholderText("Enter builds hash...");
    m_hashInput->setMinimumHeight(40);
    m_hashInput->setStyleSheet(
        "QLineEdit {"
        "   background: #2d2d2d;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   color: #e1bee7;"
        "   font-size: 14px;"
        "   padding: 0 10px;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #7c4dff;"
        "}"
    );

    // Allow all Base64 characters (A-Z, a-z, 0-9, +, /, =)
    QRegExp base64Regex("[A-Za-z0-9+/=]+");
    m_hashInput->setValidator(new QRegExpValidator(base64Regex, this));

    QPushButton* pasteButton = new QPushButton("Paste", this);
    pasteButton->setMinimumHeight(40);
    pasteButton->setCursor(Qt::PointingHandCursor);
    pasteButton->setStyleSheet(
        "QPushButton {"
        "   background: #4a148c;"
        "   border: none;"
        "   border-radius: 4px;"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 0 20px;"
        "}"
        "QPushButton:hover {"
        "   background: #7c4dff;"
        "}"
        "QPushButton:pressed {"
        "   background: #3f1dcb;"
        "}"
    );

    connect(pasteButton, &QPushButton::clicked, this, [this]() {
        QString clipboardText = QApplication::clipboard()->text();
        if (!clipboardText.isEmpty()) {
            m_hashInput->setText(clipboardText);
        }
    });

    hashLayout->addWidget(m_hashInput, 1);
    hashLayout->addWidget(pasteButton);
    mainLayout->addLayout(hashLayout);

    // Create buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);

    QPushButton* cancelButton = new QPushButton("Cancel", this);
    cancelButton->setMinimumHeight(40);
    cancelButton->setCursor(Qt::PointingHandCursor);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   border: 2px solid #4a148c;"
        "   border-radius: 4px;"
        "   color: #e1bee7;"
        "   font-size: 14px;"
        "   padding: 0 20px;"
        "}"
        "QPushButton:hover {"
        "   border-color: #7c4dff;"
        "   color: #7c4dff;"
        "}"
        "QPushButton:pressed {"
        "   border-color: #3f1dcb;"
        "   color: #3f1dcb;"
        "}"
    );

    QPushButton* addButton = new QPushButton("Add Builds", this);
    addButton->setMinimumHeight(40);
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setStyleSheet(
        "QPushButton {"
        "   background: #4a148c;"
        "   border: none;"
        "   border-radius: 4px;"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 0 20px;"
        "}"
        "QPushButton:hover {"
        "   background: #7c4dff;"
        "}"
        "QPushButton:pressed {"
        "   background: #3f1dcb;"
        "}"
    );

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(addButton, &QPushButton::clicked, this, &AddBuildDialog::onAddClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(addButton);

    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    setStyleSheet(
        "AddBuildDialog {"
        "   background: #1e1e1e;"
        "}"
    );
}

void AddBuildDialog::onAddClicked()
{
    QString hash = m_hashInput->text().trimmed();
    try {
        QByteArray jsonData = QByteArray::fromBase64(hash.toLatin1());
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        
        if (doc.isNull() || !doc.isArray()) {
            throw std::runtime_error("Неверный формат хэша");
        }

        QJsonArray buildsArray = doc.array();
        if (buildsArray.isEmpty()) {
            throw std::runtime_error("В хэше нет билдов");
        }

        m_builds.clear();
        for (const QJsonValue& value : buildsArray) {
            if (!value.isObject()) continue;

            Build build;
            if (build.fromJson(value.toObject())) {
                m_builds.append(build);
            }
        }

        if (m_builds.isEmpty()) {
            throw std::runtime_error("Не удалось загрузить ни одного билда");
        }

        accept();
    } catch (const std::exception& e) {
        m_errorLabel->setText(QString("Ошибка: %1").arg(e.what()));
        m_errorLabel->show();
    }
}

QVector<Build> AddBuildDialog::getBuilds() const
{
    return m_builds;
} 