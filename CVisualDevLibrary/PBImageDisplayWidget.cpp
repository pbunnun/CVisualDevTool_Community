#include <QtGui>
#include <QtOpenGL/QtOpenGL>
#include "PBImageDisplayWidget.hpp"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>

PBImageDisplayWidget::
PBImageDisplayWidget(QWidget *parent)
    : QOpenGLWidget( parent )
{
    setFixedSize( 160, 120 );
    setAutoFillBackground( false );
}

void
PBImageDisplayWidget::
Display( const cv::Mat &image )
{
    mCVImage = image.clone();
    muImageFormat = static_cast< quint8 >( mCVImage.channels() );
    repaint();
}

void
PBImageDisplayWidget::
paintEvent( QPaintEvent * )
{
    mPainter.begin( this );
    if( muImageFormat == 1 )
    {
        QImage image( static_cast< uchar* >( mCVImage.data ), mCVImage.cols, mCVImage.rows, static_cast< qint32 >( mCVImage.step ), QImage::Format_Grayscale8 );
        mrScale_x = static_cast< qreal >( this->width() )/static_cast< qreal >( mCVImage.cols );
        mrScale_y = static_cast< qreal >( this->height() )/static_cast< qreal >( mCVImage.rows );
        mPainter.scale( mrScale_x, mrScale_y );
        mPainter.drawImage( 0, 0, image, 0, 0, image.width(), image.height(), Qt::MonoOnly );
    }
    else
    {
        QImage image = QImage( static_cast<uchar*>(mCVImage.data), mCVImage.cols, mCVImage.rows, static_cast< qint32 >( mCVImage.step ), QImage::Format_BGR888 );
        mrScale_x = static_cast< qreal >( this->width() )/static_cast< qreal >( mCVImage.cols );
        mrScale_y = static_cast< qreal >( this->height() )/static_cast< qreal >( mCVImage.rows );
        mPainter.scale( mrScale_x, mrScale_y );
        mPainter.drawImage( 0, 0, image, 0, 0, image.width(), image.height(), Qt::ColorOnly );
    }
    mPainter.end();
}
