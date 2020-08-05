#include <QPluginLoader>
#include <QDir>
#include <QFileDialog>
#include "MainWindow.hpp"
#include <nodes/Node>
#include "PluginInterface.hpp"
#include "ui_MainWindow.h"
#include <QGraphicsScene>
#include <QVector>
#include "qtpropertybrowser.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include <QDebug>

using QtNodes::DataModelRegistry;

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow )
    , mpFlowScene( new PBFlowScene( this ) )
{
    auto model_regs = std::make_shared<DataModelRegistry>();
    load_plugins( model_regs );
    mpFlowScene->setRegistry(model_regs);

    ui->setupUi( this );
    QStringList headers = { "Caption", "ID" };
    ui->mpNodeListTreeView->setHeaderLabels( headers );
    ui->mpFlowView->setScene( mpFlowScene );

    setupPropertyBrowserDockingWidget();
    setupNodeCategoriesDockingWidget();
    setupNodeListDockingWidget();

    connect( ui->mpActionLoad, &QAction::triggered, this, &MainWindow::load );
    connect( ui->mpActionSave, &QAction::triggered, this, &MainWindow::save );
    connect( ui->mpActionSaveAs, &QAction::triggered, this, &MainWindow::saveAs );
    connect( ui->mpActionQuit, &QAction::triggered, this, &QMainWindow::close );
    connect( mpFlowScene, &PBFlowScene::nodeCreated, this, &MainWindow::nodeCreated );
    connect( mpFlowScene, &PBFlowScene::nodeDeleted, this, &MainWindow::nodeDeleted );
    connect( mpFlowScene, &QGraphicsScene::selectionChanged, this, &MainWindow::nodeInSceneSelectionChanged );
    connect( ui->mpNodeListTreeView, &QTreeWidget::itemClicked, this, &MainWindow::nodeListClicked );
    connect( ui->mpNodeListTreeView, &QTreeWidget::itemDoubleClicked, this, &MainWindow::nodeListDoubleClicked );

    setWindowTitle( msProgramName + msFlowFilename );
    showMaximized();
}

MainWindow::
~MainWindow()
{
    delete mpFlowScene;
    delete mpVariantManager;
    delete mpPropertyEditor;

    QMap< QString, QTreeWidgetItem * >::ConstIterator it = mMapModelCategoryToNodeTreeWidgetItem.constBegin();
    while( it != mMapModelCategoryToNodeTreeWidgetItem.constEnd() )
    {
        delete it.value();
        it++;
    }

    delete ui;
}

void
MainWindow::
nodeInSceneSelectionChanged()
{
    auto nodes = mpFlowScene->selectedNodes();
    if( nodes.size() == 1 )
    {
        clearPropertyBrowser();

        mpSelectedNode = nodes[ 0 ];
        mpSelectedNodeDataModel = static_cast< PBNodeDataModel * >( mpSelectedNode->nodeDataModel() );
        if( mpSelectedNodeDataModel->embeddedWidget() )
            mpSelectedNodeDataModel->embeddedWidget()->setEnabled( true );
        connect( mpSelectedNodeDataModel, SIGNAL( property_changed_signal( std::shared_ptr< Property > ) ),
                this, SLOT( nodePropertyChanged( std::shared_ptr< Property > ) ) );
        connect( mpSelectedNodeDataModel, SIGNAL( property_structure_changed_signal() ),
                this, SLOT( nodeInSceneSelectionChanged() ) );

        auto propertyVector = mpSelectedNodeDataModel->getProperty();

        auto nodeID = mpSelectedNode->id().toString();
        auto nodeTreeWidgetItem = mMapNodeIDToNodeTreeWidgetItem[ nodeID ];
        ui->mpNodeListTreeView->clearSelection();
        nodeTreeWidgetItem->setSelected( true );

        QtVariantProperty *property;
        property = mpVariantManager->addProperty( QVariant::String, "Node ID" );
        property->setAttribute( "readOnly", true);
        property->setValue( nodeID );
        addProperty( property, "id", "" );

        auto it = propertyVector.begin();
        while( it != propertyVector.end() )
        {
            auto type = ( *it )->getType();
            if( type == QVariant::String )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< QString > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setValue( typedProp->getData() );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::Int )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                IntPropertyType intPropType = typedProp->getData();
                property->setAttribute( QLatin1String( "minimum" ), intPropType.miMin );
                property->setAttribute( QLatin1String( "maximum" ), intPropType.miMax );
                property->setValue( intPropType.miValue );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::Double )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                DoublePropertyType doublePropType = typedProp->getData();
                property->setAttribute( QLatin1String( "minimum" ), doublePropType.mdMin );
                property->setAttribute( QLatin1String( "maximum" ), doublePropType.mdMax );
                property->setValue( doublePropType.mdValue );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QtVariantPropertyManager::enumTypeId() )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setAttribute( QLatin1String( "enumNames" ), typedProp->getData().mslEnumNames );
                property->setValue( typedProp->getData().miCurrentIndex );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::Bool )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< bool >>( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setAttribute( QLatin1String( "textVisible" ), false );
                property->setValue( typedProp->getData() );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QtVariantPropertyManager::filePathTypeId() )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< FilePathPropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setAttribute( QLatin1String( "filter" ), typedProp->getData().msFilter );
                property->setAttribute( QLatin1String( "mode" ), typedProp->getData().msMode );
                property->setValue( typedProp->getData().msFilename );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::Size )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setValue( QSize( typedProp->getData().miWidth, typedProp->getData().miHeight ) );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::SizeF )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< SizeFPropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setValue( QSizeF( typedProp->getData().mfWidth, typedProp->getData().mfHeight ) );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::Point )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setValue( QPoint( typedProp->getData().miXPosition, typedProp->getData().miYPosition ) );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            else if( type == QVariant::PointF )
            {
                auto typedProp = std::static_pointer_cast< TypedProperty< PointFPropertyType > >( *it );
                property = mpVariantManager->addProperty( type, typedProp->getName() );
                property->setValue( QPointF( typedProp->getData().mfXPosition, typedProp->getData().mfYPosition ) );
                addProperty( property, typedProp->getID(), typedProp->getSubPropertyText() );
            }
            it++;
        }
    }
    else
    {
        if( mpSelectedNodeDataModel != nullptr )
        {
            clearPropertyBrowser();
            if( mpSelectedNodeDataModel->embeddedWidget() )
                mpSelectedNodeDataModel->embeddedWidget()->setEnabled( false );
            disconnect(mpSelectedNodeDataModel, nullptr, this, nullptr);
            mpSelectedNodeDataModel = nullptr;
            mpSelectedNode = nullptr;

            ui->mpNodeListTreeView->clearSelection();
        }
    }
}

void
MainWindow::
setupPropertyBrowserDockingWidget()
{
    mpVariantManager = new QtVariantPropertyManager(this);
    connect(mpVariantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(editorPropertyChanged(QtProperty *, const QVariant &)));

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
    mpPropertyEditor = new QtTreePropertyBrowser(ui->mpPropertyBrowserDockWidget);
    mpPropertyEditor->setResizeMode(QtTreePropertyBrowser::Interactive);
    mpPropertyEditor->setFactoryForManager(mpVariantManager, variantFactory);
    ui->mpPropertyBrowserDockWidget->setWidget(mpPropertyEditor);
}

void
MainWindow::
setupNodeCategoriesDockingWidget()
{
    auto skipText = QStringLiteral( "skip me" );
    // Add filterbox to the menu.
    ui->mpAvailableNodeCategoryFilterLineEdit->setPlaceholderText( QStringLiteral( "Filter" ) );
    ui->mpAvailableNodeCategoryFilterLineEdit->setClearButtonEnabled( true );
    // Add models to the view.
    for ( auto const &cat : mpFlowScene->registry().categories() )
    {
        auto item = new QTreeWidgetItem( ui->mpAvailableNodeCategoryTreeView );
        item->setText( 0, cat );
        item->setData( 0, Qt::UserRole, skipText );
        mMapModelCategoryToNodeTreeWidgetItem[ cat ] = item;
    }

    for ( auto const &assoc : mpFlowScene->registry().registeredModelsCategoryAssociation() )
    {
        auto parent = mMapModelCategoryToNodeTreeWidgetItem[assoc.second];
        auto item = new QTreeWidgetItem( parent );
        item->setText( 0, assoc.first );
        item->setData( 0, Qt::UserRole, assoc.first );

        auto type = mpFlowScene->registry().create( assoc.first );
        item->setIcon( 0, QIcon( type->minPixmap() ) );
    }
    ui->mpAvailableNodeCategoryTreeView->expandAll();
    //Setup filtering
    connect( ui->mpAvailableNodeCategoryFilterLineEdit, &QLineEdit::textChanged, [&]( const QString &text )
    {
        for( auto& item : mMapModelCategoryToNodeTreeWidgetItem )
        {
            for (int i = 0; i < item->childCount(); ++i)
            {
                auto child = item->child( i );
                auto modelName = child->data( 0, Qt::UserRole ).toString();
                const bool match = ( modelName.contains( text, Qt::CaseInsensitive ) );
                child->setHidden( !match );
            }
        }
    });
}

void
MainWindow::
setupNodeListDockingWidget()
{
    auto skipText = QStringLiteral( "skip me" );
    // Add filterbox to the menu.
    ui->mpNodeListFilterLineEdit->setPlaceholderText( QStringLiteral( "Filter" ) );
    ui->mpNodeListFilterLineEdit->setClearButtonEnabled( true );

    ui->mpNodeListTreeView->expandAll();
    connect( ui->mpNodeListFilterLineEdit, &QLineEdit::textChanged, [&]( const QString &text )
    {
        for( auto& item : mMapModelNameToNodeTreeWidgetItem )
        {
            for( int i = 0; i < item->childCount(); ++i )
            {
                auto child = item->child( i );
                auto nodeName = child->data( 0, Qt::UserRole ).toString();
                const bool match = ( nodeName.contains( text, Qt::CaseInsensitive ) );
                child->setHidden( !match );
            }
        }
    });
}

void
MainWindow::
updatePropertyExpandState()
{
    QVector<QtBrowserItem *> vec = mpPropertyEditor->topLevelItems();
    QVectorIterator<QtBrowserItem *> it( vec );
    while( it.hasNext() )
    {
        QtBrowserItem * item = it.next();
        QtProperty * prop = item->property();
        mMapPropertyIdToExpanded[ mMapQtPropertyToPropertyId[ prop ] ] = mpPropertyEditor->isExpanded( item );
    }
}

// Set node's property when its property changed by the property browser.
void
MainWindow::
editorPropertyChanged(QtProperty * property, const QVariant & value)
{
    if( !mMapQtPropertyToPropertyId.contains( property ) )
        return;
    if( !mpSelectedNode )
        return;

    QString propId = mMapQtPropertyToPropertyId[ property ];
    //Update node's property
    mpSelectedNodeDataModel->setModelProperty( propId, value);
    mpSelectedNode->nodeGraphicsObject().setGeometryChanged();
    if( propId == "caption" )
    {
        auto nodeId = mpSelectedNode->id().toString();
        auto child = mMapNodeIDToNodeTreeWidgetItem[ nodeId ];
        child->setText( 0, value.toString() );
    }
    else if( propId == "minimize" )
    {
        mpSelectedNode->nodeGeometry().recalculateSize();
        mpSelectedNode->nodeGraphicsObject().moveConnections();
    }
    //Update node's gui
    mpSelectedNode->nodeGraphicsObject().update();
}

// Set node's property browser when its propery changed from within node itself.
void
MainWindow::
nodePropertyChanged( std::shared_ptr< Property > prop)
{
    QString id = prop->getID();
    auto property = static_cast< QtVariantProperty * >( mMapPropertyIdToQtProperty[ id ] );
    auto type = prop->getType();

    if( type == QVariant::String )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< QString > >( prop );
        property->setValue( typedProp->getData() );
    }
    else if( type == QVariant::Int )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        property->setValue( typedProp->getData().miValue );
    }
    else if( type == QtVariantPropertyManager::enumTypeId() )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        property->setValue( typedProp->getData().miCurrentIndex );
    }
    else if( type == QVariant::Bool )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        property->setValue( typedProp->getData() );
    }
    else if( type == QtVariantPropertyManager::filePathTypeId() )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< FilePathPropertyType > >( prop );
        property->setValue( typedProp->getData().msFilename );
    }
    else if( type == QVariant::Size )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( prop );
        auto size = QSize( typedProp->getData().miWidth, typedProp->getData().miHeight );
        property->setValue( size );
    }
    else if( type == QVariant::SizeF )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< SizeFPropertyType > >( prop );
        auto sizef = QSizeF( typedProp->getData().mfWidth, typedProp->getData().mfHeight );
        property->setValue( sizef );
    }
    else if( type == QVariant::Point )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
        auto point = QPoint( typedProp->getData().miXPosition, typedProp->getData().miYPosition );
        property->setValue( point );
    }
    else if( type == QVariant::PointF )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointFPropertyType > >( prop );
        auto pointf = QPointF( typedProp->getData().mfXPosition, typedProp->getData().mfYPosition );
        property->setValue( pointf );
    }
}

void
MainWindow::
save()
{
    if( !msFlowFilename.isEmpty() )
        mpFlowScene->save( msFlowFilename );
    else
        saveAs();
}

void
MainWindow::
saveAs()
{
    QString filename = QFileDialog::getSaveFileName(nullptr,
                                   tr( "Open Flow Scene" ),
                                   QDir::homePath(),
                                   tr( "Flow Scene Files (*.flow)" ));

    if( !filename.isEmpty() )
    {
        if( !filename.endsWith( "flow", Qt::CaseInsensitive ) )
            filename += ".flow";
        if( mpFlowScene->save(filename) )
        {
            msFlowFilename = filename;
            setWindowTitle( msProgramName + msFlowFilename );
        }
    }
}

void
MainWindow::
load()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                                   tr( "Open Flow Scene" ),
                                   QDir::homePath(),
                                   tr( "Flow Scene Files (*.flow)" ));
    if( mpFlowScene->load( filename ) )
    {
        msFlowFilename = filename;
        setWindowTitle( msProgramName + msFlowFilename );
    }
}

void
MainWindow::
clearPropertyBrowser()
{
    updatePropertyExpandState();

    QMap< QtProperty *, QString >::ConstIterator itProp = mMapQtPropertyToPropertyId.constBegin();
    while( itProp != mMapQtPropertyToPropertyId.constEnd() )
    {
        delete itProp.key();
        itProp++;
    }
    while( !mListGroupPropertyManager.isEmpty() )
    {
        delete mListGroupPropertyManager.last();
        mListGroupPropertyManager.removeLast();
    }

    mMapQtPropertyToPropertyId.clear();
    mMapPropertyIdToQtProperty.clear();
    mListGroupPropertyManager.clear();
}

void
MainWindow::
nodeCreated( Node & node )
{
    auto skipText = QStringLiteral( "skip me" );
    auto modelName = node.nodeDataModel()->name();
    auto caption = node.nodeDataModel()->caption();
    auto nodeID = node.id().toString();
    if( !mMapModelNameToNodeTreeWidgetItem.contains( modelName ) )
    {
        auto item = new QTreeWidgetItem( ui->mpNodeListTreeView );
        item->setText( 0, modelName );
        item->setData( 0, Qt::UserRole, skipText );
        auto type = mpFlowScene->registry().create( modelName );
        item->setIcon( 0, QIcon( type->minPixmap() ) );
        mMapModelNameToNodeTreeWidgetItem[ modelName ] = item;
    }

    auto item = mMapModelNameToNodeTreeWidgetItem[ modelName ];
    auto child = new QTreeWidgetItem( item );
    child->setText( 0, caption);
    child->setData( 0, Qt::UserRole, caption );
    child->setText( 1, nodeID );
    child->setData( 1, Qt::UserRole, nodeID );

    mMapNodeIDToNode[ nodeID ] = & node;
    mMapNodeIDToNodeTreeWidgetItem[ nodeID ] = child;

    ui->mpNodeListTreeView->expandItem( item );

    mpFlowScene->clearSelection();
    node.nodeGraphicsObject().setSelected( true );
}

void
MainWindow::
nodeDeleted( Node & n )
{
    auto nodeID = n.id().toString();
    if( mMapNodeIDToNode.contains( nodeID ) )
        mMapNodeIDToNode.remove( nodeID );
    auto child = mMapNodeIDToNodeTreeWidgetItem[ nodeID ];
    if( child )
    {
        mMapNodeIDToNodeTreeWidgetItem.remove( nodeID );
        auto parent = child->parent();
        delete child;
        if( parent->childCount() == 0 )
        {
            mMapModelNameToNodeTreeWidgetItem.remove( parent->text( 0 ) );
            delete parent;
        }
        ui->mpNodeListTreeView->clearSelection();
    }
}

void
MainWindow::
nodeListClicked( QTreeWidgetItem * item, int )
{
    if( item->columnCount() == 2 )
    {
        mpFlowScene->clearSelection();

        auto node = mMapNodeIDToNode[ item->text( 1 ) ];
        node->nodeGraphicsObject().setSelected( true );
    }
}

void
MainWindow::
nodeListDoubleClicked( QTreeWidgetItem * item, int )
{
    if( item->columnCount() == 2 )
    {
        auto node = mMapNodeIDToNode[ item->text( 1 ) ];
        ui->mpFlowView->center_on( node );
    }
}

void
MainWindow::
addProperty( QtVariantProperty *property, const QString & prop_id, const QString & sub_text )
{
    mMapQtPropertyToPropertyId[ property ] = prop_id;
    mMapPropertyIdToQtProperty[ prop_id ] = property;

    if( sub_text == "" )
    {
        QtBrowserItem * item = mpPropertyEditor->addProperty( property );
        if( mMapPropertyIdToExpanded.contains( prop_id ) )
            mpPropertyEditor->setExpanded( item, mMapPropertyIdToExpanded[ prop_id ] );
    }
    else if( mMapPropertyIdToQtProperty.contains( sub_text ) )
    {
        auto main_prop = mMapPropertyIdToQtProperty[ sub_text ];
        main_prop->addSubProperty( property );
    }
    else
    {
        QtGroupPropertyManager * new_group = new QtGroupPropertyManager( this );
        QtProperty * main_prop = new_group->addProperty( sub_text );
        main_prop->addSubProperty( property );

        mMapQtPropertyToPropertyId[ main_prop ] = sub_text;
        mMapPropertyIdToQtProperty[ sub_text ] = main_prop;
        mListGroupPropertyManager.push_back( new_group );
        QtBrowserItem * item = mpPropertyEditor->addProperty( main_prop );
        if( mMapPropertyIdToExpanded.contains( sub_text ) )
            mpPropertyEditor->setExpanded( item, mMapPropertyIdToExpanded[ sub_text ] );
    }
}
