#include "CameraPreview.h"

CameraPreview::CameraPreview(QWidget *parent, QString cameraDetails, QPushButton *buttonIsEnabledFromParent, QPushButton *buttonRecognationFromParent, QLabel *numberOfEnabledCameras, OnvifClientDevice *onvifDevice, int camID)
	: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);
	this->buttonIsEnabledFromParent = buttonIsEnabledFromParent;
	this->buttonRecognationFromParent = buttonRecognationFromParent;
	this->numberOfEnabledCameras = numberOfEnabledCameras;
	this->onvifDevice = onvifDevice;

	ui.LCameraDetails->setText(cameraDetails);

	this->capThread = new CapturingFrame(this);
	ptz = nullptr;

	connect(ui.PBCameraOnOff, SIGNAL(clicked()), this, SLOT(TurnOnOffCamera()));
	connect(ui.PBRecognize, SIGNAL(clicked()), this, SLOT(TurnOnOffRecognizeMode()));

	connect(ui.PBBack, SIGNAL(clicked()), this, SLOT(BackButtonClicked()));

	connect(ui.PBEdit, &QPushButton::clicked, this, [this, camID] {emit openCameraEdit(camID); });

	connect(ui.PBLeft, &QPushButton::pressed, this, [this] {MoveCamera(-0.2, 0.0); });
	connect(ui.PBRight, &QPushButton::pressed, this, [this] {MoveCamera(0.2, 0.0); });
	connect(ui.PBUp, &QPushButton::pressed, this, [this] {MoveCamera(0.0, 0.2); });
	connect(ui.PBDown, &QPushButton::pressed, this, [this] {MoveCamera(0.0, -0.2); });

	connect(ui.PBLeft, SIGNAL(released()), this, SLOT(StopCamera()));
	connect(ui.PBRight, SIGNAL(released()), this, SLOT(StopCamera()));
	connect(ui.PBUp, SIGNAL(released()), this, SLOT(StopCamera()));
	connect(ui.PBDown, SIGNAL(released()), this, SLOT(StopCamera()));

	connect(ui.PBHome, SIGNAL(clicked()), this, SLOT(GoHomeCamera()));

	connect(capThread, SIGNAL(updatePixmap(const QPixmap&)), this, SLOT(UpdatePixmap(const QPixmap&)));
	connect(parent, SIGNAL(closeCameraEdit(const QString&)), this, SLOT(CloseCameraEdit(const QString&)));

	/*_tptz__SetPresetResponse *res2 = new _tptz__SetPresetResponse();
	ptz->SetPreset(*res2, profileToken);

	_tptz__GetPresetsResponse *res = new _tptz__GetPresetsResponse();
	ptz->GetPresets(*res, profileToken);*/

	if (buttonIsEnabledFromParent->text() == "On")
	{
		StartShowingPreview();
	}
	else
	{
		StopShowingPreview();
	}

	if (buttonRecognationFromParent->text() == "On")
	{
		ui.PBRecognize->setText("On");
		ui.PBRecognize->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOn.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOnHover.png);}");
	}
	else
	{
		ui.PBRecognize->setText("Off");
		ui.PBRecognize->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOff.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOffHover.png);}");
	}

}

CameraPreview::~CameraPreview()
{
	delete capThread;
	if (ptz != nullptr)
	{
		delete ptz;
	}
}

bool CameraPreview::StartShowingPreview()
{
	if (onvifDevice->GetCapabilities() == 0)
	{
		OnvifClientMedia media(*onvifDevice);
		_trt__GetProfilesResponse profiles;
		media.GetProfiles(profiles);

		if (profiles.Profiles.size() > 0)
		{
			this->profileToken = profiles.Profiles[0]->token;
			_trt__GetStreamUriResponse link;
			media.GetStreamUrl(this->profileToken, link);

			if (&link.MediaUri != NULL)
			{
				this->ptz = new OnvifClientPTZ(*onvifDevice);

				std::string login = "";
				std::string pass = "";
				onvifDevice->GetUserPasswd(login, pass);
				std::string streamURI = link.MediaUri->Uri.insert(link.MediaUri->Uri.find("//") + 2, login + ":" + pass + "@");

				capThread->SetStreamURI(streamURI);
				capThread->start();

				ui.PBCameraOnOff->setText("On");
				ui.PBCameraOnOff->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(36, 118, 59);}QPushButton:hover{background-color: rgb(39, 129, 63);}");
				ui.PBUp->setEnabled(true);
				ui.PBDown->setEnabled(true);
				ui.PBLeft->setEnabled(true);
				ui.PBRight->setEnabled(true);
				ui.PBHome->setEnabled(true);
				ui.PBPatrol->setEnabled(true);
				return true;
			}
		}
	}
	return false;
}
void CameraPreview::StopShowingPreview()
{
	ui.PBCameraOnOff->setText("Off");
	ui.PBCameraOnOff->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(255, 77, 61);}QPushButton:hover{background-color: rgb(255, 87, 58);}");
	ui.PBUp->setEnabled(false);
	ui.PBDown->setEnabled(false);
	ui.PBLeft->setEnabled(false);
	ui.PBRight->setEnabled(false);
	ui.PBHome->setEnabled(false);
	ui.PBPatrol->setEnabled(false);
}


void CameraPreview::UpdatePixmap(const QPixmap& pixmap) 
{
	ui.LPreviewScreen->setPixmap(pixmap);
}

void CameraPreview::CloseCameraEdit(const QString& cameraDetails)
{
	ui.LCameraDetails->setText(cameraDetails);
	//TODO zamyka� i uruchamia� w�tek je�eli si� zmieni�y dane 
}

void CameraPreview::BackButtonClicked()
{
	this->close();
}

void CameraPreview::closeEvent(QCloseEvent *event)
{
	if (capThread->isRunning() == true)
	{
		capThread->StopThread();
		capThread->wait();
	}
	event->accept();
}

void CameraPreview::TurnOnOffCamera()
{
	int number = numberOfEnabledCameras->text().split(" ").last().toInt();

	if (ui.PBCameraOnOff->text() == "Off")
	{
		if (StartShowingPreview())
		{
			buttonIsEnabledFromParent->setText("On");
			buttonIsEnabledFromParent->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(36, 118, 59);}QPushButton:hover{background-color: rgb(39, 129, 63);}");
			number += 1;
		}
	}
	else
	{
		capThread->StopThread();
		capThread->wait();

		StopShowingPreview();
		buttonIsEnabledFromParent->setText("Off");
		buttonIsEnabledFromParent->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(255, 77, 61);}QPushButton:hover{background-color: rgb(255, 87, 58);}");
		number -= 1;

		ui.LPreviewScreen->clear();
	}

	numberOfEnabledCameras->setText("Number of enabled cameras: " + QVariant(number).toString());
}

void CameraPreview::TurnOnOffRecognizeMode()
{
	if (buttonRecognationFromParent->text() == "Off")
	{
		buttonRecognationFromParent->setText("On");
		buttonRecognationFromParent->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOn.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOnHover.png);}");
		ui.PBRecognize->setText("On");
		ui.PBRecognize->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOn.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOnHover.png);}");
	}
	else
	{
		buttonRecognationFromParent->setText("Off");
		buttonRecognationFromParent->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOff.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOffHover.png);}");
		ui.PBRecognize->setText("Off");
		ui.PBRecognize->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOff.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOffHover.png);}");
	}
}
void CameraPreview::MoveCamera(float panSpeed, float tiltSpeed)
{
	_tptz__ContinuousMoveResponse *res = new _tptz__ContinuousMoveResponse();
	LONG64 timeout = 1;

	tt__PTZSpeed *ptzSpeed = new tt__PTZSpeed();
	ptzSpeed->PanTilt = new tt__Vector2D();
	ptzSpeed->PanTilt->x = panSpeed;
	ptzSpeed->PanTilt->y = tiltSpeed;

	ptzSpeed->Zoom = new tt__Vector1D();
	ptzSpeed->Zoom->x = 0.0;

	for (int i = 0; i < MAXCONNECTIONTRIES; i++)
	{
		ptz->ContinuousMove(*res, *ptzSpeed, timeout, profileToken);
		if (res->soap != nullptr)
		{
			if (res->soap->status == 200)
			{
				break;
			}
		}
	}

	delete ptzSpeed->Zoom;
	delete ptzSpeed;
	delete res;
}
void CameraPreview::StopCamera()
{
	_tptz__StopResponse *res = new _tptz__StopResponse();
	for (int i = 0; i < MAXCONNECTIONTRIES; i++)
	{
		ptz->Stop(*res, 1, 1, profileToken);
		if (res->soap != nullptr)
		{
			if (res->soap->status == 200)
			{
				break;
			}
		}
	}

	delete res;
}
void CameraPreview::GoHomeCamera()
{
	_tptz__GotoHomePositionResponse *res = new _tptz__GotoHomePositionResponse();
	for (int i = 0; i < MAXCONNECTIONTRIES; i++)
	{
		ptz->GoToHomePosition(*res, profileToken);
		if (res->soap != nullptr)
		{
			if (res->soap->status == 200)
			{
				break;
			}
		}
	}

	delete res;
}
