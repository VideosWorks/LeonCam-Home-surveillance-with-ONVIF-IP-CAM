#pragma once

#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <qimage.h>
#include <qpixmap.h>
#include "ImgProc.h"
#include "Utilities.h"
#include "qtimer.h"
#include <iterator>
#include <QCoreApplication>
#include <qrunnable.h>

#define thumbnailWidth 216
#define thumbnailHeight 123
class MainAppCamera : public QThread//, public QRunnable
{
	Q_OBJECT

public:
	MainAppCamera(ImgProc *imgProc,int cameraID, QObject *parent);
	~MainAppCamera();
	void SetStreamURI(std::string streamURI);
	void StopThread();
	void ChangeFaceRecoState(bool state);
	void SetCameraID(int cameraID) { this->cameraID = cameraID; }
	void SetSendBigPicture(bool setting);
	QTimer *greenTimer;
	QTimer *redTimer;
signals:
	void updateThumbnail(const QPixmap& pixmap, int cameraID);
	void updatePixmap(const QPixmap& pixmap);

//private slots:
//	void UpdateGreenAlerts();
//	void UpdateRedAlerts();

private:
	struct GreenAlert
	{
		int faceID;
		int cameraID;
		QString startDate;
		QString stopDate;
		int greenAlertID;
	};
	struct RedAlert
	{
		QString startDate;
		QString stopDate;
		int redAlertID;
	};
	int cameraID = -1;
	ImgProc *imgProc = nullptr;
	std::string streamURI;
	bool isWorking;
	bool sendBigPicture;
	bool faceRecognitionState = false;
	cv::VideoCapture vcap;
	cv::Mat img;
	std::list<GreenAlert> *greenAlertList;
	RedAlert *redAlert = nullptr;
	void UpdateDBAfterPrediction(int predictionLabel);
	void run() override;
public slots:
	void UpdateGreenAlerts();
	void UpdateRedAlerts();
	void Process();
signals:
	void starkWorking();
	void insertGreenAlert(int greenAlertID, int faceID, int cameraID, QString dateTimeNow);
	void insertRedAlert(int redAlertID, int cameraID, QString dateTimeNow);
};
