#include "BasicNodePlugin.hpp"
#include "CannyEdgeModel.hpp"
#include "RGBsetValueModel.hpp"
#include "CVImageDisplayModel.hpp"
#include "InformationDisplayModel.hpp"
#include "CVImageLoaderModel.hpp"
#include "CVVDOLoaderModel.hpp"
#include "RGBtoGrayModel.hpp"
#include "ColorSpaceModel.hpp"
#include "DrawContourModel.hpp"
#include "Test_SharpenModel.hpp"
#include "CVCameraModel.hpp"
#include "GaussianBlurModel.hpp"
#include "TemplateModel.hpp"
#include "SobelAndScharrModel.hpp"
#include "CreateHistogramModel.hpp"
#include "ErodeAndDilateModel.hpp"
#include "InvertGrayModel.hpp"
#include "ThresholdingModel.hpp"
#include "BlendImagesModel.hpp"
#include "FloodFillModel.hpp"
#include "MakeBorderModel.hpp"
#include "FaceDetectionModel.hpp"
#include "BitwiseOperationModel.hpp"
#include "ImageROIModel.hpp"
#include "CVImagePropertiesModel.hpp"
#include "MorphologicalTransformationModel.hpp"
#include "HoughCircleTransfromModel.hpp"
#include "DistanceTransformModel.hpp"
#include "Filter2DModel.hpp"
#include "SplitImageModel.hpp"
#include "TemplateMatchingModel.hpp"

QStringList BasicNodePlugin::registerDataModel( std::shared_ptr< DataModelRegistry > model_regs )
{
    QStringList duplicate_model_names;

    registerModel< CVImageDisplayModel >( model_regs, duplicate_model_names );
    registerModel< CVImagePropertiesModel >( model_regs, duplicate_model_names );
    registerModel< InformationDisplayModel > ( model_regs, duplicate_model_names );
    registerModel< CVImageLoaderModel >( model_regs, duplicate_model_names );
    registerModel< CVVDOLoaderModel >( model_regs, duplicate_model_names );
    registerModel< RGBtoGrayModel >( model_regs, duplicate_model_names );
    registerModel< ColorSpaceModel >( model_regs, duplicate_model_names );
    registerModel< DrawContourModel >( model_regs, duplicate_model_names );
    registerModel< Test_SharpenModel >( model_regs, duplicate_model_names );
    registerModel< CannyEdgeModel >( model_regs, duplicate_model_names );
    registerModel< RGBsetValueModel >(model_regs, duplicate_model_names);
    registerModel< SobelAndScharrModel >( model_regs, duplicate_model_names );
    registerModel< CreateHistogramModel >( model_regs, duplicate_model_names );
    registerModel< ErodeAndDilateModel >( model_regs, duplicate_model_names );
    registerModel< InvertGrayModel >( model_regs, duplicate_model_names );
    registerModel< GaussianBlurModel >( model_regs, duplicate_model_names );
    registerModel< FaceDetectionModel >( model_regs, duplicate_model_names );
    registerModel< ThresholdingModel >( model_regs, duplicate_model_names );
    registerModel< BlendImagesModel >( model_regs, duplicate_model_names );
    registerModel< FloodFillModel >( model_regs, duplicate_model_names );
    registerModel< MakeBorderModel >( model_regs, duplicate_model_names );
    registerModel< BitwiseOperationModel >( model_regs, duplicate_model_names );
    registerModel< ImageROIModel >( model_regs, duplicate_model_names );
    registerModel< MorphologicalTransformationModel >( model_regs, duplicate_model_names );
    registerModel< HoughCircleTransformModel >( model_regs, duplicate_model_names );
    registerModel< DistanceTransformModel >( model_regs, duplicate_model_names );
    registerModel< SplitImageModel >( model_regs, duplicate_model_names );
    registerModel< Filter2DModel >( model_regs, duplicate_model_names );
    registerModel< TemplateMatchingModel >( model_regs, duplicate_model_names );
    registerModel< CVCameraModel >( model_regs, duplicate_model_names );
    registerModel< TemplateModel >( model_regs, duplicate_model_names );


    return duplicate_model_names;
}
