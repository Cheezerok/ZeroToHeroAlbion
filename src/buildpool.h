#ifndef BUILDPOOL_H
#define BUILDPOOL_H

#include "build.h"
#include <QVector>
#include <QString>
#include <QSet>
#include <QJsonObject>
#include <random>

class BuildPool {
public:
    BuildPool();
    
    void addBuild(const Build& build);
    void removeBuild(const Build& build);
    bool containsBuild(const Build& build) const;
    void banBuild(const Build& build);
    
    const QVector<Build>& getBuilds() const { return m_builds; }
    const QVector<Build>& getSelectedBuilds() const { return m_selectedBuilds; }
    
    QString generatePoolHash() const;
    QString generatePoolHash(const QVector<Build>& builds) const;
    bool importPool(const QString& hash);
    void generateRandomBuilds(int count);
    
    QJsonObject toJson() const;
    static BuildPool fromJson(const QJsonObject& json);

private:
    QVector<Build> m_builds;
    QVector<Build> m_selectedBuilds;
    QSet<QString> m_bannedBuilds;
    
    static std::random_device rd;
    static std::mt19937 gen;
};

#endif // BUILDPOOL_H 