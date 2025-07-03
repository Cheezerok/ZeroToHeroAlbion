#ifndef ROLEDIALOG_H
#define ROLEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>

class RoleDialog : public QDialog {
    Q_OBJECT
public:
    enum class Role {
        Organizer,
        Player
    };

    explicit RoleDialog(QWidget* parent = nullptr);
    Role getSelectedRole() const { return m_selectedRole; }
    QString getSettingsHash() const { return m_hashEdit->text(); }

private slots:
    void onOrganizerClicked();
    void onPlayerClicked();
    void onHashEntered();
    void onCancelClicked();

private:
    void setupUi();
    void showHashInput();

    Role m_selectedRole;
    QLineEdit* m_hashEdit;
    QPushButton* m_continueButton;
};

#endif // ROLEDIALOG_H 