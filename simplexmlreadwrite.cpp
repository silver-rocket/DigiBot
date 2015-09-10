#include "simplexmlreadwrite.h"

QMap<QString, QString> SimpleXMLReadWrite::readNonIerarchy(const QString& fname)
{
    QFile file(fname);

    bool ret = file.open(QIODevice::ReadOnly);

    if (!ret)
        return QMap<QString, QString>();

    QXmlStreamReader stream(&file);

    QMap<QString, QString> out_map;

    stream.readNextStartElement(); // Skip "Settings" tag
    while (stream.readNextStartElement()) // Read single element
    {
        out_map[stream.name().toString()] = stream.readElementText();
    }

    file.close();

    return out_map;
}

bool SimpleXMLReadWrite::writeNonIerarchy(const QMap<QString, QString>& map, const QString& fname)
{
    QFile file(fname);

    bool ret = file.open(QIODevice::WriteOnly);

    if (!ret)
        return false;

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement("Settings");

    for (auto it = map.begin(); it!= map.end(); it++) {
        stream.writeTextElement(it.key(), it.value());
    }

    stream.writeEndElement();

    stream.writeEndDocument();

    file.close();

    return true;
}

QMap<QString, QMap<QString, QString> > SimpleXMLReadWrite::readIerarchy(const QString& fname)
{
    QFile file(fname);

    bool ret = file.open(QIODevice::ReadOnly);

    if (!ret)
        return QMap<QString, QMap<QString, QString> >();

    QXmlStreamReader stream(&file);

    QMap<QString, QMap<QString, QString> > out_map;

    stream.readNextStartElement(); // Skip "Settings" tag
    while (stream.readNextStartElement()) { // Read next cmd

        QString cmd_name = stream.name().toString();
        QMap<QString, QString> single_cmd_map;

        while (stream.readNextStartElement()) // Read single cmd info
        {
            single_cmd_map[stream.name().toString()] = stream.readElementText();
        }
        out_map[cmd_name] = single_cmd_map;
    }

    file.close();

    return out_map;
}

bool SimpleXMLReadWrite::writeIerarchy(const QMap<QString, QMap<QString, QString> >& map, const QString& fname)
{
    QFile file(fname);

    bool ret = file.open(QIODevice::WriteOnly);

    if (!ret)
        return false;

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement("Settings");

    for (auto it = map.begin(); it!= map.end(); it++) {

        QString key = it.key();
        QMap<QString, QString> value = it.value();

        stream.writeStartElement(key);

        for (auto it2 = value.begin(); it2!= value.end(); it2++) {
            stream.writeTextElement(it2.key(), it2.value());
        }

        stream.writeEndElement();
    }

    stream.writeEndElement();

    stream.writeEndDocument();

    file.close();
}

