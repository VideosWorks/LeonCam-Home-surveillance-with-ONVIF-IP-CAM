#include "NewProfile.h"


NewProfile::NewProfile(QWidget *parent)
	: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);
	//Create DesignB instance
	designB = new DesignBase(this);
	//Set gif in Lloading label
	designB->SetGifInLabel(ui.Lloading);
	//Signals and slots
	connect(ui.PBCreate, SIGNAL(clicked()), this, SLOT(CreateClicked()));
	connect(ui.PBBack, SIGNAL(clicked()), this, SLOT(BackClicked()));
}

NewProfile::~NewProfile()
{
}
void NewProfile::CreateClicked()
{
	//Get values from ui and prepare it
	designB->gif->start();
	QString username = "";
	username = ui.LEUsername->text();
	QString password = "";
	password = ui.LEPassword->text();
	QString confirmedPassword = "";
	confirmedPassword = ui.LEConfPass->text();
	QString securityQuestion = "";
	securityQuestion = ui.LESecQuest->text();
	QString answer = "";
	answer = ui.LEAnswer->text();

	if (username == "" || password == "" || confirmedPassword == "" || securityQuestion == "" || answer == "")
	{
		designB->gif->stop();
		Utilities::MBAlarm("At least one field is incomplete", false);
		return;
	}
	if (password != confirmedPassword)
	{
		designB->gif->stop();
		Utilities::MBAlarm("Passwords are not the same", false);
		return;
	}

	/*Requirements:
		minimum 8 characters
		minimum 1 digit
		minimum 1 capital letter
		minimum lowercase*/
	std::regex passwordPattern("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])[a-zA-Z0-9]{8,}$");
	if (std::regex_match(password.toStdString(), passwordPattern) == false)
	{
		designB->gif->stop();
		Utilities::MBAlarm("Password incompatible format", false);
		ui.LEPassword->setText("");
		ui.LEConfPass->setText("");

		return;
	}

	std::string concatHelp = "";
	//password abbreviation -> password + username
	concatHelp = password.toStdString() + username.toStdString();
	QString passwordAbbreviation = QString::fromStdString(Utilities::sha256HEX(concatHelp));

	//answer abbreviation -> username + answer
	concatHelp = username.toStdString() + answer.toStdString();

	QString answerAbbreviation = QString::fromStdString(Utilities::sha256HEX(concatHelp));

	QString currentDateTimeS = Utilities::GetCurrentDateTime();

	QSqlQuery query;
	query.exec("BEGIN IMMEDIATE TRANSACTION");
	query.prepare("INSERT INTO Users (Username, Password, SecurityQuestion, Answer, RedAlertDeleteSettingID, GreenAlertDeleteSettingID, LastLogoutDate, LastLoginAttemptDate, LoginAttemptCounter, RegistrationDate) "
		"VALUES (:Username, :Password, :SecurityQuestion, :Answer, :RedAlertDeleteSettingID,:GreenAlertDeleteSettingID, :LastLogoutDate, :LastLoginAttemptDate, :LoginAttemptCounter, :RegistrationDate)");
	query.bindValue(":Username", username);
	query.bindValue(":Password", passwordAbbreviation);
	query.bindValue(":SecurityQuestion", securityQuestion);
	query.bindValue(":Answer", answerAbbreviation);
	query.bindValue(":RedAlertDeleteSettingID", 1);
	query.bindValue(":GreenAlertDeleteSettingID", 1);
	query.bindValue(":LastLogoutDate", currentDateTimeS);
	query.bindValue(":LastLoginAttemptDate", currentDateTimeS);
	query.bindValue(":LoginAttemptCounter", 0);
	query.bindValue(":RegistrationDate", currentDateTimeS);
	bool result = query.exec() == true ? true : false;
	
	designB->gif->stop();
	if (result == true)
	{
		query.exec("COMMIT");
		Utilities::MBAlarm("Account has been created", true);
		this->close();
	}
	else
	{
		Utilities::MBAlarm("Account has not been created. Your login is occupied", false);
	}
}
void NewProfile::BackClicked()
{
	this->close();
}