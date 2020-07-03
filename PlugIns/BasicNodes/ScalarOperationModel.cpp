#include "ScalarOperationModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include "qtvariantproperty.h"

ScalarOperationModel::
ScalarOperationModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":ScalarOperation.png" )
{
    mpInformationData = std::make_shared< InformationData >();

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"+","-",">",">=","<","<=","*","/","MAX","MIN"});
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "operator";
    auto propOperator = std::make_shared< TypedProperty< EnumPropertyType > >( "Operator", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propOperator );
    mMapIdToProperty[ propId ] = propOperator;
}

unsigned int
ScalarOperationModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 2;
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
ScalarOperationModel::
dataType(PortType portType, PortIndex portIndex) const
{
    NodeDataType nodeDataType = NodeDataType();
    if(portType == PortType::In)
    {
        return mapInformationInData[portIndex]?
               mapInformationInData[portIndex]->type() : InformationData().type() ;
    }
    else if(portType == PortType::Out)
    {
        return mpInformationData->type();
    }
    return nodeDataType;
}


std::shared_ptr<NodeData>
ScalarOperationModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpInformationData;
    else
        return nullptr;
}

void
ScalarOperationModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<InformationData>(nodeData);
        if (d)
        {
            mapInformationInData[portIndex] = d;
            if(mapInformationInData[0]&&mapInformationInData[1])
            {
                processData( mapInformationInData, mpInformationData, mParams );
            }
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
ScalarOperationModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["operator"] = mParams.miOperator;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ScalarOperationModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "operator" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "operator" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miOperator = v.toInt();
        }
    }
}

void
ScalarOperationModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "operator" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty <EnumPropertyType>>(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

        mParams.miOperator = value.toInt();
    }

    if( mapInformationInData[0] && mapInformationInData[1] )
    {
        processData( mapInformationInData, mpInformationData, mParams );

        Q_EMIT dataUpdated(0);
    }
}

void
ScalarOperationModel::
processData(std::shared_ptr< InformationData > (&in)[2], std::shared_ptr<InformationData> & out,
            const ScalarOperationParameters & params )
{
    for(std::shared_ptr<InformationData>& info : in)
    {
        info->set_information();
    }
    const double& in0 = in[0]->info().toDouble();
    const double& in1 = in[1]->info().toDouble();

    if( params.miOperator == SclOps::PLUS )
    {
        auto result = (in0 + in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::MINUS )
    {
        auto result = (in0 - in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::GREATER_THAN)
    {
        bool result = (in0 > in1);
        info_pointer_cast(result, out);
    }else if( params.miOperator == SclOps::GREATER_THAN)
    {
        bool result = (in0 > in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::GREATER_THAN_OR_EQUAL)
    {
        bool result = (in0 > in1 || in0 == in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::LESSER_THAN)
    {
        bool result = (in0 < in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::LESSER_THAN_OR_EQUAL)
    {
        bool result = (in0 < in1 || in0 == in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::MULTIPLY)
    {
        auto result = (in0 * in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::DIVIDE)
    {
        auto result = (in0 / in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::MAXIMUM)
    {
        auto result = std::max(in0 , in1);
        info_pointer_cast(result, out);
    }
    else if( params.miOperator == SclOps::MINIMUM)
    {
        auto result = std::min(in0 , in1);
        info_pointer_cast(result, out);
    }
}

const QString ScalarOperationModel::_category = QString( "Number Operation" );

const QString ScalarOperationModel::_model_name = QString( "Scalar Operation" );
