#include "simplexmlreadwrite.h"

SimpleXMLReadWrite::SimpleXMLReadWrite(const QString& _fname):fname(_fname)
{

}

SimpleXMLReadWrite::~SimpleXMLReadWrite()
{

}

bool SimpleXMLReadWrite::writeFile(const QMap<QString, QString>& map)
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

QMap<QString, QString> SimpleXMLReadWrite::readFile()
{
    QFile file(fname);

    bool ret = file.open(QIODevice::ReadOnly);

    if (!ret)
        return QMap<QString, QString>();

    QXmlStreamReader stream(&file);

    QMap<QString, QString> out_map;

    stream.readNextStartElement(); // Skip "Settings" tag
    while (stream.readNextStartElement())
    {
        out_map[stream.name().toString()] = stream.readElementText();
    }

    file.close();

    return out_map;
}







