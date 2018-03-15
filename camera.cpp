#include "camera.h"
#include <QPixmap>
#include <QImage>

using namespace cv;


Camera::Camera(QString path)
{
    this->videoPath = path;
}

QString Camera::getPath()
{
    return this->videoPath;
}

bool Camera::openCamera()
{
    capture = cv::VideoCapture(videoPath.toStdString());
    if(!capture.isOpened())
        return false;
    else
    {
        frameNum = capture.get(CV_CAP_PROP_FRAME_COUNT);
        capture.set( CV_CAP_PROP_POS_FRAMES, 0);
        return true;
    }
}
void Camera::setPath(QString path)
{
    this->videoPath = path;
}

long Camera::getFrameNum()
{
    return frameNum;
}

bool Camera::readFrame(QPixmap & pixmap)
{
    if(!capture.read(cvframe))
        return false;
    cv::cvtColor(cvframe, cvframe, CV_BGR2RGB);
    qimage = QImage((const uchar *)(cvframe.data), cvframe.cols, cvframe.rows, QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(qimage);
    return true;
}

bool Camera::cameraState()
{
    return capture.isOpened();
}

void Camera::closeCamera()
{
    capture.release();
}
