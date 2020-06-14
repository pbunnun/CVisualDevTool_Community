#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <FlowScene>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include "PBNodeDataModel.hpp"
#include "PBFlowScene.hpp"
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QtVariantProperty;
class QtProperty;
using QtNodes::Node;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = nullptr );
    ~MainWindow();

private slots:
    void editorPropertyChanged( QtProperty *property, const QVariant &value );
    void nodePropertyChanged( std::shared_ptr<Property> );
    void nodeCreated( Node & );
    void nodeDeleted( Node & );
    void nodeInSceneSelectionChanged();
    void nodeListClicked( QTreeWidgetItem *, int );
    void nodeListDoubleClicked( QTreeWidgetItem *, int );
    void save();
    void saveAs();
    void load();

private:
    void setupPropertyBrowserDockingWidget();
    void setupNodeCategoriesDockingWidget();
    void setupNodeListDockingWidget();

    Ui::MainWindow *ui;

    PBFlowScene * mpFlowScene;
    QtNodes::Node * mpSelectedNode{nullptr};
    PBNodeDataModel * mpSelectedNodeDataModel{nullptr};

    QMap<QString, QTreeWidgetItem*> mMapModelCategoryToNodeTreeWidgetItem;

    QMap<QString, QTreeWidgetItem*> mMapModelNameToNodeTreeWidgetItem;

    QMap<QString, QTreeWidgetItem*> mMapNodeIDToNodeTreeWidgetItem;
    QMap<QString, Node*> mMapNodeIDToNode;
    //////////////////////////////////////////////////////////
    /// \brief mpVariantManager for Node Property Editor
    ///
    class QtVariantPropertyManager * mpVariantManager;
    class QtTreePropertyBrowser * mpPropertyEditor;
    QMap<QtProperty *, QString> mMapPropertyToPropertyId;
    QMap<QString, QtVariantProperty *> mMapPropertyIdToQtProperty;
    QMap<QString, bool> mMapPropertyIdToExpanded;

    QString msFlowFilename{"Untitle.flow"};
    const QString msProgramName{"CVisualDev : "};

    void updatePropertyExpandState();
    void addProperty(QtVariantProperty *property, const QString &id);
    void clearPropertyBrowser();
};
#endif // MAINWINDOW_H
