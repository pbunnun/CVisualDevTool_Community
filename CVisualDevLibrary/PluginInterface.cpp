#include "PluginInterface.hpp"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QPluginLoader>
#include <QMessageBox>

void load_plugins( std::shared_ptr< DataModelRegistry > model_regs )
{
    QDir pluginsDir = QDir(QCoreApplication::applicationDirPath());
    pluginsDir.cd( "plugins" );
    const auto entryList = pluginsDir.entryList(QDir::Files);
    for(const QString & filename : entryList )
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
        qDebug() << "Found Files : " << filename;
        QObject *plugin = loader.instance();
        if( plugin )
        {
            qDebug() << "Load Plugins : " << filename;
            auto plugin_interface = qobject_cast<PluginInterface *>( plugin );
            if( plugin_interface )
            {
                QStringList duplicate_model_names = plugin_interface->registerDataModel( model_regs );
                if( !duplicate_model_names.isEmpty() )
                {
                    QMessageBox msg;
                    QString msgText = "Please check " + filename + "\n Duplicate Model Names :";
                    for( const QString & model_name : duplicate_model_names )
                        msgText += " " + model_name;
                    msgText += ".";
                    msg.setIcon( QMessageBox::Warning );
                    msg.setText( msgText );
                    msg.exec();
                }
            }
        }
    }

}
