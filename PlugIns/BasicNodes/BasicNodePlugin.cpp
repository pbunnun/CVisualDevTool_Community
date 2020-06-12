#include "BasicNodePlugin.hpp"
#include "CannyEdgeModel.hpp"
#include "RGBsetValueModel.hpp"
#include "CVImageDisplayModel.hpp"
#include "PixmapDisplayModel.hpp"
#include "CVImageLoaderModel.hpp"
#include "CVVDOLoaderModel.hpp"
#include "RGBtoGrayModel.hpp"
#include "Test_SharpenModel.hpp"
#include "CVCameraModel.hpp"
#include "GaussianBlurModel.hpp"
#include "TemplateModel.hpp"

QStringList BasicNodePlugin::registerDataModel( std::shared_ptr< DataModelRegistry > model_regs )
{
    QStringList duplicate_model_names;

    registerModel< CVImageDisplayModel >( model_regs, duplicate_model_names );
    registerModel< PixmapDisplayModel >( model_regs, duplicate_model_names );
    registerModel< CVImageLoaderModel >( model_regs, duplicate_model_names );
    registerModel< CVVDOLoaderModel >( model_regs, duplicate_model_names );
    registerModel< RGBtoGrayModel >( model_regs, duplicate_model_names );
    registerModel< Test_SharpenModel >( model_regs, duplicate_model_names );
    registerModel< CannyEdgeModel >( model_regs, duplicate_model_names );
    registerModel< RGBsetValueModel >(model_regs, duplicate_model_names);
    registerModel< GaussianBlurModel >( model_regs, duplicate_model_names );
    registerModel< CVCameraModel >( model_regs, duplicate_model_names );
    registerModel< TemplateModel >( model_regs, duplicate_model_names );

    return duplicate_model_names;
}
