#include "PBNodeDataModel.hpp"

using QtNodes::PortType;

PBNodeDataModel::PBNodeDataModel(QString modelName, bool enable)
    : QtNodes::NodeDataModel(),
      msModelName(modelName),
      mOrgNodeStyle(nodeStyle())
{
    setCaption(name());
    mOrgNodeStyle.NormalBoundaryColor = Qt::darkGreen;
    mOrgNodeStyle.SelectedBoundaryColor = Qt::green;
    setNodeStyle(mOrgNodeStyle);
    enabled(enable);

    QString propId = "caption";
    auto propCaption = std::make_shared< TypedProperty< QString > >( "Caption", propId, QVariant::String, caption() ) ;
    mvProperty.push_back( propCaption );
    mMapIdToProperty[ propId ] = propCaption;

    propId = "enable";
    auto propEnable = std::make_shared< TypedProperty< bool > >( "Enable", propId, QVariant::Bool, isEnable() );
    mvProperty.push_back( propEnable );
    mMapIdToProperty[ propId ] = propEnable;

    propId = "minimize";
    auto propMinimize = std::make_shared< TypedProperty< bool > >( "Minimize", propId, QVariant::Bool, isMinimize() );
    mvProperty.push_back( propMinimize );
    mMapIdToProperty[ propId ] = propMinimize;

    connect( this, SIGNAL( enable_changed_signal( bool ) ), this, SLOT( enable_changed( bool ) ) );
    connect( this, SIGNAL( minimize_changed_signal( bool ) ), this, SLOT( minimize_changed( bool ) ) );
}

QJsonObject
PBNodeDataModel::
save() const
{
    QJsonObject modelJson = NodeDataModel::save();

    QJsonObject params = modelJson["params"].toObject();
    params["caption"] = caption();

    modelJson["params"] = params;

    return modelJson;
}

void
PBNodeDataModel::
restore(QJsonObject const &p)
{
    NodeDataModel::restore(p);

    QJsonObject paramsObj = p["params"].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj["caption"];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty["caption"];
            auto typedProp = std::static_pointer_cast<TypedProperty<QString>>(prop);
            typedProp->getData() = v.toString();

            setCaption( v.toString() );
        }

        v = paramsObj[ "enable" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "enable" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            enabled( v.toBool() );
        }

        v = paramsObj[ "minimize" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "minimize" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            minimized( v.toBool() );
        }
    }

}

void
PBNodeDataModel::
setModelProperty(QString & id, const QVariant & value)
{
    if(!mMapIdToProperty.contains(id))
        return;

    auto prop = mMapIdToProperty[id];
    if( id == "caption" )
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<QString>>(prop);
        typedProp->getData() = value.toString();

        setCaption( value.toString() );
    }
    else if( id == "enable" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        enabled( value.toBool() );
        Q_EMIT enable_changed_signal( value.toBool() );
    }
    else if( id == "minimize" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        minimized( value.toBool() );
        Q_EMIT minimize_changed_signal( value.toBool() );
    }
}

void
PBNodeDataModel::
setEnable( bool enable )
{
    enabled( enable );

    auto prop = mMapIdToProperty[ "enable" ];
    auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
    typedProp->getData() = enable;
    Q_EMIT property_changed_signal( prop );
}

void
PBNodeDataModel::
setMinimize( bool minimize )
{
    minimized( minimize );

    auto prop = mMapIdToProperty[ "minimize" ];
    auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
    typedProp->getData() = minimize;
    Q_EMIT property_changed_signal( prop );
}

void
PBNodeDataModel::
enabled( bool enable )
{
    NodeDataModel::setEnable( enable );

    if( enable )
        setNodeStyle(mOrgNodeStyle);
    else
    {
        auto style = mOrgNodeStyle;
        style.NormalBoundaryColor = Qt::darkRed;
        style.SelectedBoundaryColor = Qt::red;
        setNodeStyle(style);
    }
}

void
PBNodeDataModel::
minimized( bool minimize )
{
    NodeDataModel::setMinimize( minimize );
}

void
PBNodeDataModel::
updateAllOutputPorts()
{
    auto no_output_ports = nPorts( PortType::Out );
    for( unsigned int i = 0; i < no_output_ports; i++ )
        Q_EMIT dataUpdated( i );
}
