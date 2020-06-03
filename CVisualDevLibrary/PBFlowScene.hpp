#ifndef PBFLOWSCENE_HPP
#define PBFLOWSCENE_HPP

#pragma once

#include "CVisualDevLibrary.hpp"

#include "nodes/internal/Export.hpp"
#include <nodes/FlowScene>

class CVISUALDEVSHAREDLIB_EXPORT PBFlowScene : public QtNodes::FlowScene
{
    Q_OBJECT
public:
    explicit PBFlowScene(QWidget *parent = nullptr);

    bool save(QString & sFilename) const;

    bool load(QString & sFilename);
};

#endif // PBFLOWSCENE_HPP
