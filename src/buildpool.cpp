#include "buildpool.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <algorithm>
#include <QRandomGenerator>

std::random_device BuildPool::rd;
std::mt19937 BuildPool::gen(rd());

BuildPool::BuildPool()
{
}

void BuildPool::addBuild(const Build& build)
{
    m_builds.append(build);
}

void BuildPool::removeBuild(const Build& build)
{
    m_builds.removeOne(build);
}

bool BuildPool::containsBuild(const Build& build) const
{
    return m_builds.contains(build);
}

QString BuildPool::generatePoolHash() const
{
    QJsonArray buildsArray;
    for (const Build& build : m_builds) {
        buildsArray.append(build.toJson());
    }
    
    QJsonDocument doc(buildsArray);
    return QString::fromLatin1(doc.toJson().toBase64());
}

QString BuildPool::generatePoolHash(const QVector<Build>& builds) const
{
    QJsonArray buildsArray;
    for (const Build& build : builds) {
        buildsArray.append(build.toJson());
    }
    
    QJsonDocument doc(buildsArray);
    return QString::fromLatin1(doc.toJson().toBase64());
}

bool BuildPool::importPool(const QString& hash)
{
    QByteArray jsonData = QByteArray::fromBase64(hash.toLatin1());
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    
    if (doc.isNull() || !doc.isArray()) {
        return false;
    }
    
    QJsonArray buildsArray = doc.array();
    m_builds.clear();
    
    for (const QJsonValue& value : buildsArray) {
        Build build;
        if (build.fromJson(value.toObject())) {
            m_builds.append(build);
        }
    }
    
    return !m_builds.isEmpty();
}

void BuildPool::generateRandomBuilds(int count)
{
    QVector<Build> availableBuilds;
    
    // Collect all unbanned builds
    for (const Build& build : m_builds) {
        if (!build.isBanned()) {
            availableBuilds.append(build);
        }
    }
    
    m_selectedBuilds.clear();
    
    // Generate random builds
    if (!availableBuilds.isEmpty()) {
        while (m_selectedBuilds.size() < count && !availableBuilds.isEmpty()) {
            int index = QRandomGenerator::global()->bounded(availableBuilds.size());
            m_selectedBuilds.append(availableBuilds.takeAt(index));
        }
    }
}

void BuildPool::banBuild(const Build& build) {
    m_bannedBuilds.insert(build.getName());
}

QJsonObject BuildPool::toJson() const {
    QJsonObject json;
    QJsonArray buildsArray;
    
    for (const Build& build : m_builds) {
        buildsArray.append(build.toJson());
    }
    
    QJsonArray bannedArray;
    for (const QString& banned : m_bannedBuilds) {
        bannedArray.append(banned);
    }
    
    json["builds"] = buildsArray;
    json["banned"] = bannedArray;
    return json;
}

BuildPool BuildPool::fromJson(const QJsonObject& json) {
    BuildPool pool;
    
    QJsonArray buildsArray = json["builds"].toArray();
    for (const QJsonValue& value : buildsArray) {
        Build build;
        if (build.fromJson(value.toObject())) {
            pool.addBuild(build);
        }
    }
    
    QJsonArray bannedArray = json["banned"].toArray();
    for (const QJsonValue& value : bannedArray) {
        pool.m_bannedBuilds.insert(value.toString());
    }
    
    return pool;
} 