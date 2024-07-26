#ifndef NOTES_HPP
#define NOTES_HPP


#include <qt6/QtCore/QPair>
#include <qt6/QtCore/QMap>
#include <qt6/QtCore/QString>

class Notes
{
public:
    Notes();
    ~Notes() {}
    qreal getFrequency(QString key);

private:
    QMap<QString, qreal>frequencies;

};
#endif
