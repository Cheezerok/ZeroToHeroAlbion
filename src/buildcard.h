#ifndef BUILDCARD_H
#define BUILDCARD_H

#include <QWidget>
#include "build.h"

class QLabel;

class BuildCard : public QWidget
{
    Q_OBJECT

public:
    explicit BuildCard(const Build& build, bool isBanned = false, QWidget* parent = nullptr);
    
    bool isBanned() const;
    void setBanned(bool banned);
    Build getBuild() const;
    
    bool isSelected() const;
    void setSelected(bool selected);

signals:
    void clicked(BuildCard* card);
    void banStateChanged(BuildCard* card);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();
    void updateStyle();

    Build m_build;
    bool m_isBanned;
    bool m_isHovered;
    bool m_isSelected;

    // UI elements
    QLabel* m_nameLabel;
    QLabel* m_weaponLabel;
    QLabel* m_armorLabel;
    QLabel* m_bannedLabel;
};

#endif // BUILDCARD_H 