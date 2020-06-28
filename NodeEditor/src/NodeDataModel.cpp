#include "NodeDataModel.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeDataModel;
using QtNodes::NodeStyle;

#include <QDebug>

NodeDataModel::
NodeDataModel()
  : _minimize(false), _enable(false),
    _nodeStyle(StyleCollection::nodeStyle()), _minPixmap(":NodeEditor.png")
{
  // Derived classes can initialize specific style here
}


QJsonObject
NodeDataModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  QJsonObject params;
  params["minimize"] = isMinimize();
  params["enable"] = isEnable();

  modelJson["params"] = params;
  return modelJson;
}


void
NodeDataModel::
restore(QJsonObject const &p)
{
  QJsonObject params = p["params"].toObject();
  QJsonValue v = params["minimize"];
  if( !v.isUndefined() )
  {
    _minimize = v.toBool();
  }

  v = params["enable"];
  if( !v.isUndefined() )
  {
    _enable = v.toBool();
  }
}


NodeStyle const&
NodeDataModel::
nodeStyle() const
{
  return _nodeStyle;
}


void
NodeDataModel::
setNodeStyle(NodeStyle const& style)
{
  _nodeStyle = style;
}


void
NodeDataModel::
setMinimize(bool minimize)
{
    _minimize = minimize;
}


void
NodeDataModel::
setEnable(bool enable)
{
    _enable = enable;
}
