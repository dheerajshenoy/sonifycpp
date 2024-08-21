#pragma once

#include <QPair>
#include <QMap>
#include <QString>

class Notes
{
public:
    Notes() noexcept;
    qreal getFrequency(const QString& key) noexcept;

private:
    QMap<QString, qreal>frequencies;

};
