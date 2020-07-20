#include "VideoWriterModel.hpp"

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"


VideoWriterModel::
VideoWriterModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new VideoWriterEmbeddedWidget ),
      _minPixmap( ":VideoWriter.png" )
{
    mpInformationData = std::make_shared< InformationData >();
    mpSyncData = std::make_shared< SyncData >();

    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect(mpEmbeddedWidget, &VideoWriterEmbeddedWidget::button_clicked_signal, this, &VideoWriterModel::em_button_clicked);

    mQFSaveDialog.setAcceptMode(QFileDialog::AcceptSave);
    mQFSaveDialog.setFileMode(QFileDialog::AnyFile);
    mQFSaveDialog.setNameFilter(tr("Video Files (*.mp4)"));
    mQFSaveDialog.setViewMode(QFileDialog::Detail);
    mQFSaveDialog.setModal(true);
    mQFSaveDialog.setWindowFilePath(mQSFilePath);
}

unsigned int
VideoWriterModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 2;
        break;

    case PortType::Out:
        result = 2;
        break;

    default:
        break;
    }

    return result;
}

NodeDataType
VideoWriterModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portIndex == 0)
    {
        if(portType == PortType::In)
            return CVImageData().type();
        else if(portType == PortType::Out)
            return InformationData().type();
    }
    else if(portIndex == 1)
    {
        if(portType == PortType::In)
            return IntegerData().type();
        else if(portType == PortType::Out)
            return SyncData().type();
    }
    return NodeDataType();
}

std::shared_ptr<NodeData>
VideoWriterModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I==0)
            return mpInformationData;
        else if( I==1 )
            return mpSyncData;
    }
    return nullptr;
}

void
VideoWriterModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex portIndex)
{
    if( !isEnable() )
        return;

    if( nodeData )
    {
        mpSyncData->state() = false;
        Q_EMIT dataUpdated(1);
        if(portIndex == 0)
        {
            auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
            if( d )
            {
                mpCVImageInData = d;
            }
        }
        else if(portIndex == 1)
        {
            auto d = std::dynamic_pointer_cast< IntegerData >( nodeData );
            if( d )
            {
                mpIntegerInData = d;
            }
            mpEmbeddedWidget->enable_save_button(this->mzCurrentFrames!=0 && mpIntegerInData);
        }
        if(mpCVImageInData && !this->mbPaused)
        {
            InsertToBuffer(mpCVImageInData);
        }
        mpSyncData->state() = true;
        Q_EMIT dataUpdated(1);
    }

}

void
VideoWriterModel::
InsertToBuffer(const std::shared_ptr<CVImageData> &in)
{
    const cv::Mat& image = in->image();
    if(image.empty())
    {
        return;
    }
    if(this->mzCurrentFrames % this->muiReserveRate == 0)
    {
        this->mvCVImageBuffer.resize(this->mzCurrentFrames+this->muiReserveRate);
    }
    this->mvCVImageBuffer[this->mzCurrentFrames] = image.clone();
    this->mzCurrentFrames++;
}

void
VideoWriterModel::
WriteVideo(const std::shared_ptr<IntegerData>& in,
           std::shared_ptr<InformationData>& out)
{
    const int fps = 1000/in->number();
    if(fps <= 0 || !mQFSaveDialog.exec())
    {
        return;
    }
    mQSFilePath = mQFSaveDialog.selectedFiles()[0];
    cv::VideoWriter video(this->mQSFilePath.toStdString(),
                          cv::VideoWriter::fourcc('M','J','P','G'),
                          fps,
                          this->mvCVImageBuffer[0].size(),
                          this->mvCVImageBuffer[0].channels()!=1);
    for(const cv::Mat& image : this->mvCVImageBuffer)
    {
        video.write(image);
    }
    if(video.isOpened())
    {
        video.release();
        out->set_information(mQSFilePath);
    }
}

void
VideoWriterModel::
em_button_clicked(int button)
{
    if(button == 0)
    {
        this->mbPaused = false;
        mpEmbeddedWidget->enable_start_button(false);
        mpEmbeddedWidget->enable_pause_button(true);
        mpEmbeddedWidget->enable_stop_button(true);
        mpEmbeddedWidget->enable_save_button(false);
        mpEmbeddedWidget->enable_display_button(false);
    }
    else if(button == 1)
    {
        this->mbPaused = true;
        mpEmbeddedWidget->enable_start_button(true);
        mpEmbeddedWidget->enable_pause_button(false);
        mpEmbeddedWidget->enable_stop_button(true);
        mpEmbeddedWidget->enable_save_button(false);
        mpEmbeddedWidget->enable_display_button(false);
    }
    else if(button == 2)
    {
        this->mbPaused = true;
        mpEmbeddedWidget->enable_start_button(true);
        mpEmbeddedWidget->enable_pause_button(false);
        mpEmbeddedWidget->enable_stop_button(false);
        mpEmbeddedWidget->enable_save_button(this->mzCurrentFrames!=0 && mpIntegerInData);
        QFileInfo check(this->mQSFilePath);
        const bool exist = QFileInfo::exists(this->mQSFilePath) &&
                           check.isFile() &&
                           check.suffix() == ".mp4";
        mpEmbeddedWidget->enable_display_button(exist);
        mpEmbeddedWidget->set_frames_label(this->mzCurrentFrames);
    }
    else if(button == 3)
    {
        WriteVideo(mpIntegerInData,mpInformationData);
        this->mvCVImageBuffer.clear();
        this->mzCurrentFrames = 0;
        this->mQSFilePath = "";
        mpEmbeddedWidget->enable_display_button(true);
    }
    else if(button == 4)
    {
        Q_EMIT dataUpdated(0);
    }
}


const QString VideoWriterModel::_category = QString("Image Operation");

const QString VideoWriterModel::_model_name = QString( "Video Writer" );
