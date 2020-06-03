#ifndef PBTREEWIDGET_H
#define PBTREEWIDGET_H

#pragma once

#include "CVisualDevLibrary.hpp"

#include "nodes/internal/Export.hpp"
#include <QTreeWidget>

class CVISUALDEVSHAREDLIB_EXPORT PBTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit PBTreeWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
};

#endif // PBTREEWIDGET_H
