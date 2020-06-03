#ifndef PBNODEDATAMODEL_HPP
#define PBNODEDATAMODEL_HPP

#pragma once

#include "CVisualDevLibrary.hpp"
#include "Property.hpp"
#include "NodeDataModel"

using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeStyle;

class CVISUALDEVSHAREDLIB_EXPORT PBNodeDataModel : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    explicit PBNodeDataModel(QString modelName, bool enable=false);

    QJsonObject
    save() const override;

    void
    restore(QJsonObject const & p) override;

    QString
    caption() const override { return msCaptionName; };

    void
    setCaption(QString caption) { msCaptionName = caption; };

    QString
    name() const override { return msModelName; };

    QString
    modelName() const { return msModelName; };

    PropertyVector
    getProperty() { return mvProperty; };

    bool
    isEnable() const { return mbEnable; };

    void
    setEnable(bool);

    virtual
    std::shared_ptr<NodeData>
    outData(PortIndex) override { return nullptr; };

    virtual
    void
    setModelProperty(QString & , const QVariant & );

Q_SIGNALS:
    void
    property_changed_signal( std::shared_ptr<Property> );

    void
    enable_changed_signal( bool );

    void
    property_structure_changed_signal( );

protected Q_SLOTS:
    virtual
    void enable_changed( bool ) {};

protected:
    PropertyVector mvProperty;
    QMap<QString, std::shared_ptr<Property>> mMapIdToProperty;
    bool mbEnable{false};

private:
    QString msCaptionName;
    QString msModelName;

    NodeStyle mOrgNodeStyle;
};

#endif // PBNODEDATAMODEL_HPP
