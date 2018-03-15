#ifndef CAMERA_H
#define CAMERA_H
#include <QString>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>

class QImage;
class QPixmap;

class Camera
{
public:
    Camera(QString path = NULL);
private:
    QString videoPath = QString::null;
    QImage qimage;//q开头表示Qt的图像格式
    cv::VideoCapture capture;
    cv::Mat cvframe;//cv开头表示是OpenCv的图像格式
    long frameNum;
public:
    bool openCamera();
    QString getPath();
    void setPath(QString);
    bool readFrame(QPixmap &);
    void closeCamera();
    long getFrameNum();
    bool cameraState();
};

#endif // CAMERA_H
