#include "TemplateModel.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>
#include <QEvent>
#include <QDir>
#include <QVariant>
#include "qtvariantproperty.h"

TemplateModel::
TemplateModel()
    : PBNodeDataModel( _model_name ),
      mpEmbeddedWidget( new TemplateEmbeddedWidget() )
{
    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &TemplateEmbeddedWidget::button_clicked_signal, this, &TemplateModel::em_button_clicked );
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = mpEmbeddedWidget->get_combobox_string_list();
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "combobox_id";
    auto propComboBox = std::make_shared< TypedProperty< EnumPropertyType > >("ComboBox", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mvProperty.push_back( propComboBox );
    mMapIdToProperty[ propId ] = propComboBox;

    IntPropertyType intPropertyType;
    intPropertyType.miMax = mpEmbeddedWidget->get_spinbox()->maximum();
    intPropertyType.miMin = mpEmbeddedWidget->get_spinbox()->minimum();
    intPropertyType.miValue = mpEmbeddedWidget->get_spinbox()->value();
    propId = "spinbox_id";
    auto propSpinBox = std::make_shared< TypedProperty< IntPropertyType > >("SpinBox", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propSpinBox );
    mMapIdToProperty[ propId ] = propSpinBox;
}

unsigned int
TemplateModel::
nPorts( PortType portType ) const
{
    switch( portType )
    {
    case PortType::In:
        return( 1 );
    case PortType::Out:
        return( 1 );
    default:
        return( -1 );
    }
}

NodeDataType
TemplateModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if( portType == PortType::Out && portIndex == 0 )
        return CVImageData().type();
    else if( portType == PortType::In && portIndex == 0 )
        return CVImageData().type();
    else
        return NodeDataType();
}

std::shared_ptr<NodeData>
TemplateModel::
outData(PortIndex)
{
    if( mbEnable && mpCVImageData->image().data != nullptr )
        return mpCVImageData;
    else
        return nullptr;
}

void
TemplateModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !mbEnable )
        return;

    if( nodeData )
    {
        /*
         * Do something with incoming data.
         */
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
        if( d )
        {
            mpCVImageData->set_image( d->image() );
        }
    }

    /*
     * Emit dataUpdated( _data_out_channel_no_ ) to tell other models who link with the model's output channel
     * that there is data ready to read out.
     */
    Q_EMIT dataUpdated( 0 );
}

QJsonObject
TemplateModel::
save() const
{
    /*
     * If save() was overrided, PBNodeDataModel::save() must be called explicitely.
     */
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams[ "combobox_text" ] = mpEmbeddedWidget->get_combobox_text();
    cParams[ "spinbox_value" ] = mpEmbeddedWidget->get_spinbox()->value();
    modelJson[ "cParams" ] = cParams;

    return modelJson;
}

void
TemplateModel::
restore(const QJsonObject &p)
{
    /*
     * If restore() was overrided, PBNodeDataModel::restore() must be called explicitely.
     */
    PBNodeDataModel::restore(p);
    late_constructor();

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "combobox_text" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "combobox_id" ];
            /* Restore internal property */
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( v.toString() );
            /* Restore mpEmbeddedWidget */
            mpEmbeddedWidget->set_combobox_value( v.toString() );
        }
        v = paramsObj[ "spinbox_value" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "spinbox_id" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mpEmbeddedWidget->set_spinbox_value( v.toInt() );
        }
    }


}

void
TemplateModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "combobox_id" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( value.toString() );

        mpEmbeddedWidget->set_combobox_value( value.toString() );
    }
    else if( id == "spinbox_id" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mpEmbeddedWidget->set_spinbox_value( value.toInt() );
    }
}

void
TemplateModel::
enable_changed(bool enable)
{
    if( enable )
    {
        qDebug() << "Enable";
    }
    else
        qDebug() << "Disable";
}

void
TemplateModel::
late_constructor()
{
    qDebug() << "Automatically call this function only after creating this model";
}

void
TemplateModel::
em_button_clicked( int button )
{
    if( button == 0 ) //Start
    {
        auto prop = mMapIdToProperty[ "enable" ];
        /*
         * Update internal property.
         */
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = true;

        /*
         * Emiting property_changed_signal will send a signal to QtPropertyBrowser
         * and it will update its parameters accordingly.
         */
        Q_EMIT property_changed_signal( prop );
    }
    else if( button == 1 ) //Stop
    {
        auto prop = mMapIdToProperty[ "enable" ];
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = false;
        Q_EMIT property_changed_signal( prop );
    }
    else if( button == 2 )
    {
        auto prop = mMapIdToProperty[ "spinbox_id" ];
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = mpEmbeddedWidget->get_spinbox()->value();
        Q_EMIT property_changed_signal( prop );
    }
    else if( button == 3 )
    {
        auto prop = mMapIdToProperty[ "combobox_id" ];
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( mpEmbeddedWidget->get_combobox_text() );
        Q_EMIT property_changed_signal( prop );
    }
}

const QString TemplateModel::_category = QString( "Template Category" );

const QString TemplateModel::_model_name = QString( "Template Model" );
