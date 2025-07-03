#ifndef ADDBUILDIALOG_H
#define ADDBUILDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "build.h"

class AddBuildDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddBuildDialog(QWidget *parent = nullptr);
    QVector<Build> getBuilds() const;

private slots:
    void onAddClicked();

private:
    void setupUi();

    QLineEdit* m_hashInput;
    QPushButton* m_addButton;
    QLabel* m_errorLabel;
    QVector<Build> m_builds;
};

#endif // ADDBUILDIALOG_H 