#include "UserCamera.h"

UserCamera::UserCamera(QWidget *parent, int userID)
	: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);
	ui.CBAvailableCameras->lineEdit()->setPlaceholderText("IPv4 address : port number");
	this->setResult(QDialog::Rejected);
	//Create DesignB instance
	designB = new DesignBase(this);
	//Set gif in Lloading label
	designB->SetGifInLabel(ui.Lloading);
	//Signals and slots
	connect(ui.PBAdd, SIGNAL(clicked()), this, SLOT(AddClicked()));
	connect(ui.PBBack, SIGNAL(clicked()), this, SLOT(BackClicked()));
	this->userID = userID;
	SearchForCameraIPs();
}

UserCamera::~UserCamera()
{
	delete designB;
}

std::string generateUuid()
{
	const int kGUIDSize = 39;

	GUID guid;
	HRESULT guid_result = CoCreateGuid(&guid);
	if (!SUCCEEDED(guid_result))
		return std::string();

	wchar_t buffer[48] = { 0 };
	::StringFromGUID2(guid, buffer, 48);
	std::wstring guid_string = buffer;

	guid_string = guid_string.substr(1, guid_string.length() - 2);

	return std::string(guid_string.begin(), guid_string.end());
}
void UserCamera::SearchForCameraIPs()
{
	DiscoveryLookupBindingProxy *proxy = new DiscoveryLookupBindingProxy();
	std::string tmpuuid = "uuid:" + generateUuid();
	proxy->soap_endpoint = "soap.udp://239.255.255.250:3702/";
	proxy->header = new SOAP_ENV__Header();
	proxy->header->wsa__Action = (char*)"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
	proxy->header->wsa__MessageID = (char*)tmpuuid.c_str();
	//proxy.header->wsa__ReplyTo = new wsa__EndpointReferenceType();
	//proxy.header->wsa__ReplyTo->Address = (char*)"http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
	proxy->header->wsa__To = (char*)"urn:schemas-xmlsoap-org:ws:2005:04:discovery";
	
	proxy->recv_timeout = 2;
	ns1__ProbeType *probe = new ns1__ProbeType();
	probe->Types = new std::string("tdn:NetworkVideoTransmitter");
	ns1__ProbeMatchesType *probeMatches = new ns1__ProbeMatchesType();

	for (int i = 0; i < 5; i++)
	{
		if (proxy->Probe(probe, probeMatches) == SOAP_OK)
		{
			break;
		}
	}
	for each (ns1__ProbeMatchType* device in probeMatches->ProbeMatch)
	{
		ui.CBAvailableCameras->addItem(QString::fromStdString(device->XAddrs->c_str()).split('/')[2]);
	}

	delete proxy->header;
	delete proxy;
	delete probe->Types;
	delete probe;
	delete probeMatches;
}

std::vector<QString>* UserCamera::GetValuesFromControls()
{
	std::vector<QString>* controlsValues = new std::vector<QString>();
	controlsValues->push_back(ui.LEDescripton->text());
	controlsValues->push_back(ui.CBAvailableCameras->currentText());
	controlsValues->push_back(ui.LELogin->text());
	controlsValues->push_back(ui.LEPassword->text());

	return controlsValues;
}
void UserCamera::AddClicked() 
{	
	designB->gif->start();

	if (ui.LEDescripton->text() == "" || ui.CBAvailableCameras->currentText() == "" || ui.LELogin->text() == "" || ui.LEPassword->text() == "")
	{
		designB->gif->stop();
		Utilities::MBAlarm("At least one field is incomplete", false);
		return;
	}

	std::regex IPv4AddressPattern("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])(:[0-9]{1,5})?$");
	if (std::regex_match(ui.CBAvailableCameras->currentText().toStdString(), IPv4AddressPattern) == false)
	{
		designB->gif->stop();
		Utilities::MBAlarm("IPv4 address incompatible format", false);
		return;
	}

	QSqlQuery query;
	query.prepare("SELECT COUNT (*) FROM Cameras WHERE UserID = ? AND Name = ?");
	query.bindValue(0, userID);
	query.bindValue(1, ui.LEDescripton->text());
	bool result = query.exec() == true ? true : false;
	if (result == true)
	{
		query.next();
		int counter = query.value(0).toInt();
		if (counter > 0)
		{
			designB->gif->stop();
			Utilities::MBAlarm("This name is occupied by your another camera. Please type another one", false);
			return;
		}
	}

	designB->gif->stop();
	this->done(QDialog::Accepted);
}
void UserCamera::BackClicked() 
{
	this->close();
}