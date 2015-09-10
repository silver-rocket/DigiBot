#ifndef SIMPLEXMLREADWRITE_H
#define SIMPLEXMLREADWRITE_H

#include <QString>
#include <QMap>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QDebug>

class SimpleXMLReadWrite
{
public:

    static QMap<QString, QString> readNonIerarchy(const QString& fname);
    static bool writeNonIerarchy(const QMap<QString, QString>& map, const QString& fname);
    static QMap<QString, QMap<QString, QString> > readIerarchy(const QString& fname);
    static bool writeIerarchy(const QMap<QString, QMap<QString, QString> >& map, const QString& fname);
};

#endif // SIMPLEXMLREADWRITE_H
