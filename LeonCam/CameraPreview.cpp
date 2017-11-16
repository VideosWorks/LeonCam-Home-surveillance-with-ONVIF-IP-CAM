#include "CameraPreview.h"

CameraPreview::CameraPreview(QWidget *parent, QString cameraDetails, QPushButton *buttonIsEnabledFromParent, QPushButton *buttonRecognationFromParent, QPushButton *buttonTakePhotoFromParent, OnvifClientDevice *onvifDevice, int camID, MainAppCamera *thread, std::string passHash)
	: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);
	this->buttonIsEnabledFromParent = buttonIsEnabledFromParent;
	this->buttonRecognationFromParent = buttonRecognationFromParent;
	this->buttonTakePhotoFromParent = buttonTakePhotoFromParent;
	this->camID = camID;
	this->passHash = passHash;
	designB = new DesignBase(this);
	ui.Lloading->setVisible(false);
	designB->SetGifInLabel(ui.Lloading);

	ui.LCameraDetails->setText(cameraDetails);

	this->capThread = thread;
	ptz = nullptr;
	ctrl = nullptr;

	connect(ui.PBCameraOnOff, SIGNAL(clicked()), this, SLOT(TurnOnOffCamera()));
	connect(ui.PBRecognize, SIGNAL(clicked()), this, SLOT(TurnOnOffRecognizeMode()));

	connect(ui.PBBack, &QPushButton::clicked, this, [this] {this->close();	});

	connect(ui.PBEdit, &QPushButton::clicked, this, [this, camID] {emit openCameraEdit(camID); });

	connect(ui.PBLeft, &QPushButton::pressed, this, [this] {ctrl->MoveCamera(-0.2, 0.0); });
	connect(ui.PBRight, &QPushButton::pressed, this, [this] {ctrl->MoveCamera(0.2, 0.0); });
	connect(ui.PBUp, &QPushButton::pressed, this, [this] {ctrl->MoveCamera(0.0, 0.2); });
	connect(ui.PBDown, &QPushButton::pressed, this, [this] {ctrl->MoveCamera(0.0, -0.2); });

	connect(ui.PBLeft, &QPushButton::released, this, [this] {ctrl->StopCamera(); });
	connect(ui.PBRight, &QPushButton::released, this, [this] {ctrl->StopCamera(); });
	connect(ui.PBUp, &QPushButton::released, this, [this] {ctrl->StopCamera(); });
	connect(ui.PBDown, &QPushButton::released, this, [this] {ctrl->StopCamera(); });

	connect(ui.PBHome, &QPushButton::clicked, this, [this] {ctrl->GoHomeCamera(); });

	connect(ui.PBSnapshot, &QPushButton::clicked, this, [this, buttonTakePhotoFromParent] {buttonTakePhotoFromParent->click(); });

	connect(capThread, SIGNAL(updatePixmap(const QPixmap&)), this, SLOT(UpdatePixmap(const QPixmap&)));
	connect(parent, SIGNAL(closeCameraEdit(const QString&)), this, SLOT(CloseCameraEdit(const QString&)));

	/*_tptz__SetPresetResponse *res2 = new _tptz__SetPresetResponse();
	ptz->SetPreset(*res2, profileToken);

	_tptz__GetPresetsResponse *res = new _tptz__GetPresetsResponse();
	ptz->GetPresets(*res, profileToken);*/

	ui.PBCameraOnOff->setText(buttonIsEnabledFromParent->text());
	ui.PBCameraOnOff->setStyleSheet(buttonIsEnabledFromParent->styleSheet());

	ui.PBRecognize->setText(buttonRecognationFromParent->text());
	ui.PBRecognize->setStyleSheet(buttonRecognationFromParent->styleSheet());
	
	ui.PBSnapshot->setText(buttonTakePhotoFromParent->text());

	if (buttonIsEnabledFromParent->text() == "On")
	{
		TurnOnLabels();
		if (!SetProfileTokenAndPTZ(onvifDevice))
		{
			TurnOnOffCamera();
		}
	}
	else
	{
		TurnOffLabels();
	}
}

CameraPreview::~CameraPreview()
{
	if (ptz != nullptr)
	{
		delete ptz;
	}
	if (ctrl != nullptr)
	{
		delete ctrl;
	}
}

void CameraPreview::TurnOnLabels()
{
	ui.PBCameraOnOff->setText("On");
	ui.PBCameraOnOff->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(36, 118, 59);}QPushButton:hover{background-color: rgb(39, 129, 63);}");
	ui.PBUp->setEnabled(true);
	ui.PBDown->setEnabled(true);
	ui.PBLeft->setEnabled(true);
	ui.PBRight->setEnabled(true);
	ui.PBHome->setEnabled(true);
	ui.PBSnapshot->setText(buttonTakePhotoFromParent->text());
}

bool CameraPreview::SetProfileTokenAndPTZ(OnvifClientDevice *onvifDevice)
{
	if (onvifDevice->GetCapabilities() == 0)
	{
		OnvifClientMedia *media = new OnvifClientMedia(*onvifDevice);
		_trt__GetProfilesResponse *profiles = new _trt__GetProfilesResponse();
		media->GetProfiles(*profiles);
		if (profiles->Profiles.size() > 0)
		{
			this->profileToken = profiles->Profiles[0]->token;
			this->ptz = new OnvifClientPTZ(*onvifDevice);
			ctrl = new CameraControl(ptz, profileToken);
			delete media;
			delete profiles;
			return true;
		}
		delete media;
		delete profiles;
	}
	return false;
}

void CameraPreview::TurnOffLabels()
{
	ui.PBCameraOnOff->setText("Off");
	ui.PBCameraOnOff->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(255, 77, 61);}QPushButton:hover{background-color: rgb(255, 87, 58);}");
	ui.PBUp->setEnabled(false);
	ui.PBDown->setEnabled(false);
	ui.PBLeft->setEnabled(false);
	ui.PBRight->setEnabled(false);
	ui.PBHome->setEnabled(false);
	ui.PBSnapshot->setText(buttonTakePhotoFromParent->text());
}

void CameraPreview::UpdatePixmap(const QPixmap& pixmap) 
{
	ui.LPreviewScreen->setPixmap(pixmap);
}

void CameraPreview::CloseCameraEdit(const QString& cameraDetails)
{
	bool wasTurnedOn = false;
	if (ui.PBCameraOnOff->text()=="On")
	{
		TurnOnOffCamera();
		wasTurnedOn = true;
	}
	ui.LCameraDetails->setText(cameraDetails);

	QSqlQuery *query = new QSqlQuery();
	query->prepare("SELECT IPAddress, Login, Password FROM Cameras WHERE CameraID=?");
	query->bindValue(0, camID);
	if (query->exec() == true)
	{
		query->next();
		string url = "http://" + query->value(0).toString().toStdString() + "/onvif/device_service";
		string user = query->value(1).toString().toStdString();
		string pass = Utilities::GetDecrypted(passHash, query->value(2).toString().toStdString());

		OnvifClientDevice *onvifDevice = new OnvifClientDevice(url, user, pass);
		if (!SetProfileTokenAndPTZ(onvifDevice))
		{
			delete onvifDevice;
			return;
		}
		delete onvifDevice;
	}

	if (buttonIsEnabledFromParent->text() == "On")
	{
		TurnOnOffCamera();
	}
	delete query;
}

void CameraPreview::closeEvent(QCloseEvent *event)
{
	event->accept();
}

void CameraPreview::TurnOnOffCamera()
{
	if (ptz == nullptr)
	{
		CloseCameraEdit(ui.LCameraDetails->text());
	}
	if (ui.PBCameraOnOff->text() == "Off")
	{
		buttonIsEnabledFromParent->click();
		TurnOnLabels();
	}
	else
	{
		buttonIsEnabledFromParent->click();
		TurnOffLabels();
		ui.LPreviewScreen->clear();
	}
}

void CameraPreview::TurnOnOffRecognizeMode()
{
	buttonRecognationFromParent->click();
	ui.PBRecognize->setText(buttonRecognationFromParent->text());
	ui.PBRecognize->setStyleSheet(buttonRecognationFromParent->styleSheet());
}