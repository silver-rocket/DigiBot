#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QMap>

class Settings
{
    QMap<QString, QString> common_settings;
    QMap<QString, QMap<QString, QString> > commands_settings;
    QMap<QString, QMap<QString, QString> > custom_commands_settings;

public:
    Settings();

    static Settings & instance() {
        static Settings inst;
        return inst;
    }

    QMap<QString, QString> & get_common_settings_ref() {
        return common_settings;
    }

    QMap<QString, QMap<QString, QString> > & get_commands_settings_ref() {
        return commands_settings;
    }

    QMap<QString, QMap<QString, QString> > & get_custom_commands_settings_ref() {
        return custom_commands_settings;
    }
};

#define COMMON_SETTINGS Settings::instance().get_common_settings_ref()
#define COMMANDS_SETTINGS Settings::instance().get_commands_settings_ref()
#define CUSTOM_COMMANDS_SETTINGS Settings::instance().get_custom_commands_settings_ref()

#endif // SETTINGS_H
