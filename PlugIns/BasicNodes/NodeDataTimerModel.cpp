#include "NodeDataTimerModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include "qtvariantproperty.h"
#include "InformationData.hpp"

NodeDataTimerModel::
NodeDataTimerModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget(new NodeDataTimerEmbeddedWidget),
      _minPixmap( ":NodeDataTimer.png" )
{
    connect(mpEmbeddedWidget,&NodeDataTimerEmbeddedWidget::timeout_signal,this,&NodeDataTimerModel::em_timeout);

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mpEmbeddedWidget->get_second_spinbox();
    QString propId = "second_value";
    auto propSecond = std::make_shared< TypedProperty< IntPropertyType > >( "", propId, QVariant::Int, intPropertyType);
    mMapIdToProperty[ propId ] = propSecond;

    intPropertyType.miValue = mpEmbeddedWidget->get_millisecond_spinbox();
    propId = "millisecond_value";
    auto propMillisecond = std::make_shared< TypedProperty< IntPropertyType > >( "", propId, QVariant::Int, intPropertyType);
    mMapIdToProperty[ propId ] = propMillisecond;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"Period","Frequency"});
    intPropertyType.miValue = mpEmbeddedWidget->get_millisecond_spinbox();
    propId = "pf_value";
    auto propPF = std::make_shared< TypedProperty< EnumPropertyType > >( "", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mMapIdToProperty[ propId ] = propPF;

    propId = "start_enabled";
    auto propStart = std::make_shared< TypedProperty < bool > > ("", propId, QVariant::Bool, mpEmbeddedWidget->get_start_button());
    mMapIdToProperty[ propId ] = propStart;

    propId = "stop_enabled";
    auto propStop = std::make_shared< TypedProperty < bool > > ("", propId, QVariant::Bool, mpEmbeddedWidget->get_stop_button());
    mMapIdToProperty[ propId ] = propStop;
}

unsigned int
NodeDataTimerModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 1;
        break;

    default:
        break;
    }

    return result;
}


NodeDataType
NodeDataTimerModel::
dataType(PortType, PortIndex) const
{
    return InformationData().type();
}


std::shared_ptr<NodeData>
NodeDataTimerModel::
outData(PortIndex)
{
    if( isEnable() )
    {
        return mpNodeData;
    }
    else
        return nullptr;
}

void
NodeDataTimerModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        mpNodeData = nodeData;
    }
}

QJsonObject
NodeDataTimerModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["second"] = mpEmbeddedWidget->get_second_spinbox();
    cParams["millisecond"] = mpEmbeddedWidget->get_millisecond_spinbox();
    cParams["PF"] = mpEmbeddedWidget->get_pf_combobox();
    cParams["start"] = mpEmbeddedWidget->get_start_button();
    modelJson["stop"] = mpEmbeddedWidget->get_stop_button();

    return modelJson;
}

void
NodeDataTimerModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "second" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "second" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mpEmbeddedWidget->set_second_spinbox(v.toInt());
        }
        v = paramsObj[ "millisecond" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "millisecond" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mpEmbeddedWidget->set_millisecond_spinbox(v.toInt());
        }
        v = paramsObj[ "PF" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "pf" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mpEmbeddedWidget->set_pf_combobox(v.toInt());
        }
        v = paramsObj[ "start" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "start" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            mpEmbeddedWidget->set_start_button(v.toBool());
        }
        v = paramsObj[ "stop" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "stop" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            mpEmbeddedWidget->set_stop_button(v.toBool());
            mpEmbeddedWidget->set_widget_bundle(v.toBool());
        }
    }
}

void NodeDataTimerModel::em_timeout()
{
    Q_EMIT dataUpdated(0);
}


const QString NodeDataTimerModel::_category = QString( "Source" );

const QString NodeDataTimerModel::_model_name = QString( "NodeData Timer" );
