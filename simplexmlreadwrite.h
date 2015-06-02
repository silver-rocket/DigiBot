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
    QString fname;

public:
    SimpleXMLReadWrite(const QString& _fname);
    ~SimpleXMLReadWrite();

    bool writeFile(const QMap<QString, QString>& map);
    QMap<QString, QString> readFile();

};

#endif // SIMPLEXMLREADWRITE_H
