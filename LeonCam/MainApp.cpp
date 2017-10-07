﻿#include "MainApp.h"

//TODO: Delete buttons from Faces Base

MainApp::MainApp(QWidget *parent, QString username)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->username = username;
	this->setWindowTitle(username + " - LeonCam");

	vectorCameraLayoutsPages = new std::vector<std::vector<QLayout*>*>();
	vectorQGridLayouts = new std::vector<QGridLayout*>();

	vectorIsEnabledButtonToRowIndex = new std::vector<QPushButton*>();
	vectorPatrolButtonToRowIndex = new std::vector<QPushButton*>();
	vectorRecognationButtonToRowIndex = new std::vector<QPushButton*>();
	vectorEditButtonToRowIndex = new std::vector<QPushButton*>();
	vectorRemoveButtonToRowIndex = new std::vector<QPushButton*>();

	FillFacesBaseTW();

	activeCameraPage = 0;

	addTab();
	ui.TWCameraPages->setTabText(0, "");

	connect(ui.PBAddCamera, SIGNAL(clicked()), this, SLOT(AddCamera()));
	//logout: on close (logout and close), by clicking logout icon (only logout and switch to LogIn window)
	connect(this, SIGNAL(closed()), this, SLOT(LogOut()));
	connect(ui.PBLogout, SIGNAL(clicked()), this, SLOT(LogOut()));
	connect(ui.LESearch, SIGNAL(textChanged(const QString&)), this, SLOT(LESearchChanged()));
	connect(ui.TWCameraPages, SIGNAL(currentChanged(int)), this, SLOT(TWCameraPagesChanged(int)));
	//Faces base
	connect(ui.TWFacesBase, SIGNAL(CurentCellChanged(int, int)), this, SLOT(UpdateDBAfterCellChanged(int, int)));
	connect(ui.LESearchFB, SIGNAL(textChanged(const QString&)), this, SLOT(LESearchFBChanged()));
	connect(ui.PBAddPerson, SIGNAL(clicked()), this, SLOT(AddPerson()));
}

MainApp::~MainApp()
{
}

void MainApp::AddCamera()
{
	if (vectorCameraLayoutsPages->size() == 20)
	{
		if (vectorCameraLayoutsPages->at(19)->size()==6)
		{
			QMessageBox msgBox;
			msgBox.setText("Error: You can't assign more cameras to your profile");
			msgBox.exec();
			return;
		}
	}
	UserCamera *UserCam = new UserCamera(this);
	bool result = UserCam->exec();
	if (result == QDialog::Accepted)
	{
		//vector includes values from QDialog
		std::vector<QString>* controlsValues = UserCam->GetValuesFromControls();

		QGridLayout *layout = new QGridLayout();

		QPushButton* btn = new QPushButton();
		btn->setStyleSheet("background-image: url(:/Resources/Images/unavailablePreview.png);");
		btn->setFixedSize(216, 123);
		connect(btn, &QPushButton::clicked, this, [this, layout] {CameraSelected(layout); });
		layout->addWidget(btn, 0, 0, 1, 5);

		QLabel *label = new QLabel(controlsValues->at(1) + " (" + controlsValues->at(2) + ")");
		label->setStyleSheet("color:rgb(255, 255, 255);");
		label->setFixedSize(216, 23);
		label->setAlignment(Qt::AlignCenter);
		layout->addWidget(label, 1, 0, 1, 5);

		btn = new QPushButton();
		btn->setText("Off");
		btn->setFixedSize(40, 40);
		btn->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(255, 77, 61);}QPushButton:hover{background-color: rgb(255, 87, 58);}");
		connect(btn, &QPushButton::clicked, this, [this, btn] {TurnOnOffCamera(btn); });
		vectorIsEnabledButtonToRowIndex->push_back(btn);
		layout->addWidget(btn, 2, 0);

		btn = new QPushButton();
		btn->setFixedSize(40, 40);
		btn->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/patrol.png); border: none; margin: 0px; padding: 0px;} QPushButton:hover{background-image: url(:/Resources/Images/patrolHover.png);}");
		btn->setToolTip("Turn on camera patrol");
		connect(btn, &QPushButton::clicked, this, [this, btn] {PatrolCamera(btn); });
		vectorPatrolButtonToRowIndex->push_back(btn);
		layout->addWidget(btn, 2, 1);

		btn = new QPushButton();
		btn->setFixedSize(40, 40);
		btn->setText("On");
		btn->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOn.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOnHover.png);}");
		btn->setToolTip("Recognation mode: On");
		connect(btn, &QPushButton::clicked, this, [this, btn] {RecognationCamera(btn); });
		vectorRecognationButtonToRowIndex->push_back(btn);
		layout->addWidget(btn, 2, 2);

		btn = new QPushButton();
		btn->setFixedSize(40, 40);
		btn->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/edit.png);border: none; margin: 0px; padding: 0px;} QPushButton:hover{background-image: url(:/Resources/Images/editHover.png);}");
		btn->setToolTip("Edit camera");
		connect(btn, &QPushButton::clicked, this, [this, btn] {EditCamera(btn); });
		vectorEditButtonToRowIndex->push_back(btn);
		layout->addWidget(btn, 2, 3);

		btn = new QPushButton();
		btn->setFixedSize(40, 40);
		btn->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/remove.png); border: none; margin: 0px; padding: 0px;} QPushButton:hover{background-image: url(:/Resources/Images/removeHover.png);}");
		btn->setToolTip("Remove camera");
		connect(btn, &QPushButton::clicked, this, [this, layout] { RemoveCamera(layout); });
		vectorRemoveButtonToRowIndex->push_back(btn);
		layout->addWidget(btn, 2, 4);

		layout->setHorizontalSpacing(4);
		layout->setVerticalSpacing(2);

		//layout->itemAtPosition(2,0)->widget()->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOn.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOnHover.png);}");


		if (vectorCameraLayoutsPages->at(vectorCameraLayoutsPages->size()-1)->size() == 6)
		{
			addTab();
		}
		else
		{
			ui.TWCameraPages->setCurrentIndex(vectorQGridLayouts->size()-1);
		}

		vectorQGridLayouts->at(vectorQGridLayouts->size()-1)->addLayout(layout, vectorCameraLayoutsPages->at(vectorCameraLayoutsPages->size() - 1)->size() / 3, vectorCameraLayoutsPages->at(vectorCameraLayoutsPages->size() - 1)->size() % 3);
		vectorCameraLayoutsPages->at(vectorCameraLayoutsPages->size() - 1)->push_back(layout);

		ui.LTotalNumber->setText("Total number of cameras: " + QString::number((vectorQGridLayouts->size()-1) * 6 + vectorCameraLayoutsPages->at(vectorCameraLayoutsPages->size() - 1) ->size()));
	}
}
void MainApp::addTab()
{
	QGridLayout *newLayout = new QGridLayout();
	QWidget *newTab = new QWidget(ui.TWCameraPages);
	newTab->setLayout(newLayout);
	ui.TWCameraPages->addTab(newTab, "");
	ui.TWCameraPages->setCurrentIndex(vectorQGridLayouts->size());
	TWCameraPagesChanged(vectorQGridLayouts->size());

	vectorCameraLayoutsPages->push_back(new std::vector<QLayout*>());
	vectorQGridLayouts->push_back(newLayout);

	ui.TWCameraPages->setStyleSheet("QTabWidget::pane {color: rgb(213, 235, 255);border: 0px;}QTabWidget::tab-bar {left: " + QString::number(360 - 18 * vectorQGridLayouts->size()) + "px;}QTabBar::tab {background-color: transparent;color: rgb(133, 196, 255);height: 18px;width: 36px;}QTabBar::tab:hover{color: rgb(160, 209, 255);}QTabBar::tab:selected{margin-top: -1px;color:rgb(219, 235, 255);}");

}
void MainApp::CameraSelected(QGridLayout* layout)
{
	CameraPreview *cameraPreview = new CameraPreview(this, ((QLabel *)layout->itemAtPosition(1, 0)->widget())->text(), (QPushButton *)layout->itemAtPosition(2, 0)->widget(), (QPushButton *)layout->itemAtPosition(2, 2)->widget(), ui.LEnabledNumber);
	cameraPreview->exec();
}
void MainApp::TurnOnOffCamera(QPushButton* button)
{
	int number = ui.LEnabledNumber->text().split(" ").last().toInt();

	if (button->text() == "Off")
	{
		button->setText("On");
		button->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(36, 118, 59);}QPushButton:hover{background-color: rgb(39, 129, 63);}");
		number += 1;
	}
	else
	{
		button->setText("Off");
		button->setStyleSheet("QPushButton{color:rgb(255, 255, 255);background-color: rgb(255, 77, 61);}QPushButton:hover{background-color: rgb(255, 87, 58);}");
		number -= 1;
	}

	ui.LEnabledNumber->setText("Number of enabled cameras: " + QVariant(number).toString());

}
void MainApp::PatrolCamera(QPushButton* button)
{
	int index = 0;

	for (const auto& item : *vectorPatrolButtonToRowIndex)
	{
		if (item == button)
		{
			if (vectorIsEnabledButtonToRowIndex->at(index)->text() == "On")
			{

			}
			return;
		}
		else
		{
			index += 1;
		}
	}
}
void MainApp::RecognationCamera(QPushButton* button)
{
	if (button->text() == "Off")
	{
		button->setText("On");
		button->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOn.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOnHover.png);}");
	}
	else
	{
		button->setText("Off");
		button->setStyleSheet("QPushButton{background-image: url(:/Resources/Images/recognizeOff.png); border: none; margin: 0px; padding: 0px; color: transparent;} QPushButton:hover{background-image: url(:/Resources/Images/recognizeOffHover.png);}");
	}
}
void MainApp::EditCamera(QPushButton* button)
{
	int index = 0;

	for (const auto& item : *vectorEditButtonToRowIndex)
	{
		if (item == button)
		{
			CameraEdition *cameraEdition = new CameraEdition(this);
			//TODO
			cameraEdition->exec();
			delete cameraEdition;
			return;
		}
		else
		{
			index += 1;
		}
	}
}
void MainApp::RemoveCamera(QGridLayout* layout)
{
	int pageIndex = 0;
	for (const auto& item : *vectorCameraLayoutsPages)
	{
		if (std::find(item->begin(), item->end(), layout) != item->end()) 
		{
			int indexOfLayoutInPage = 0;
			for (indexOfLayoutInPage = 0; indexOfLayoutInPage < item->size(); indexOfLayoutInPage++)
			{
				if (item->at(indexOfLayoutInPage) == layout)
				{
					break;
				}
			}

			if (((QPushButton*)(layout->itemAt(2)->widget()))->text() == "On")
			{
				int numberEnabled = ui.LEnabledNumber->text().split(" ").last().toInt();
				ui.LEnabledNumber->setText("Number of enabled cameras: " + QVariant(numberEnabled - 1).toString());
			}
			int number = ui.LTotalNumber->text().split(" ").last().toInt();
			ui.LTotalNumber->setText("Total number of cameras: " + QVariant(number - 1).toString());

			//Removing all item in layout
			QLayoutItem * itemLayout;
			QWidget * widget;
			while ((itemLayout = layout->takeAt(0)))
			{
				if ((widget = itemLayout->widget()) != 0) 
				{
					widget->hide(); 
					delete widget; 
				}
				else 
				{ 
					delete itemLayout; 
				}
			}
			delete layout;

			item->erase(std::remove(item->begin(), item->end(), layout), item->end());

			while (pageIndex < vectorCameraLayoutsPages->size())
			{
				while (indexOfLayoutInPage < vectorCameraLayoutsPages->at(pageIndex)->size())
				{
					QLayoutItem* layoutToMove = vectorQGridLayouts->at(pageIndex)->itemAtPosition((indexOfLayoutInPage + 1) / 3, (indexOfLayoutInPage + 1) % 3);
					vectorQGridLayouts->at(pageIndex)->removeItem(layoutToMove);

					vectorQGridLayouts->at(pageIndex)->addLayout((QGridLayout *)layoutToMove, indexOfLayoutInPage / 3, indexOfLayoutInPage % 3);
					indexOfLayoutInPage += 1;
				}

				if ((pageIndex + 1) < vectorCameraLayoutsPages->size())
				{
					if (vectorCameraLayoutsPages->at(pageIndex + 1)->size() > 0)
					{
						QLayoutItem* layoutToMove = vectorQGridLayouts->at(pageIndex + 1)->itemAtPosition(0, 0);
						vectorQGridLayouts->at(pageIndex + 1)->removeItem(layoutToMove);

						vectorQGridLayouts->at(pageIndex)->addLayout((QGridLayout *)layoutToMove, 1, 2);

						vectorCameraLayoutsPages->at(pageIndex + 1)->erase(vectorCameraLayoutsPages->at(pageIndex + 1)->begin(), vectorCameraLayoutsPages->at(pageIndex + 1)->begin() + 1);
						vectorCameraLayoutsPages->at(pageIndex)->push_back((QGridLayout *)layoutToMove);

					}
					indexOfLayoutInPage = 0;
				}
				pageIndex += 1;
			}
			if (vectorCameraLayoutsPages->at(vectorCameraLayoutsPages->size() - 1)->size() == 0 && vectorQGridLayouts->size() > 1)
			{
				QGridLayout *qgridlayout = vectorQGridLayouts->at(vectorQGridLayouts->size() - 1);
				delete qgridlayout;
				vectorQGridLayouts->pop_back();

				std::vector<QLayout*> *vectorqlayout = vectorCameraLayoutsPages->at(vectorQGridLayouts->size());
				delete vectorqlayout;
				vectorCameraLayoutsPages->pop_back();

				if (activeCameraPage == vectorQGridLayouts->size())
				{
					ui.TWCameraPages->setCurrentIndex(vectorQGridLayouts->size()-1);
					TWCameraPagesChanged(vectorQGridLayouts->size()-1);
				}

				ui.TWCameraPages->removeTab(vectorQGridLayouts->size());
				ui.TWCameraPages->setStyleSheet("QTabWidget::pane {color: rgb(213, 235, 255);border: 0px;}QTabWidget::tab-bar {left: " + QString::number(360 - 18 * vectorQGridLayouts->size()) + "px;}QTabBar::tab {background-color: transparent;color: rgb(133, 196, 255);height: 18px;width: 36px;}QTabBar::tab:hover{color: rgb(160, 209, 255);}QTabBar::tab:selected{margin-top: -1px;color:rgb(219, 235, 255);}");

			}
			if (vectorQGridLayouts->size()==1)
			{
				ui.TWCameraPages->setTabText(0, "");
			}
			break;
		}
		else
		{
			pageIndex+=1;
		}
	}

}
void MainApp::LESearchChanged()
{
	/*for (int i = 0; i < ui.TLWCameras->rowCount(); i++)
	{
		if (ui.TLWCameras->item(i, 2)->text().startsWith(ui.LESearch->text(), Qt::CaseInsensitive))
		{
			ui.TLWCameras->showRow(i);
		}
		else
		{
			ui.TLWCameras->hideRow(i);
		}
	}*/
}
void MainApp::TWCameraPagesChanged(int newIndex)
{
	ui.TWCameraPages->setTabText(activeCameraPage, QString::fromStdWString(L"\u25CB"));
	ui.TWCameraPages->setTabText(newIndex, QString::fromStdWString(L"\u25CF"));
	activeCameraPage = newIndex;
}
void MainApp::LogOut()
{
	//Get proper user from DB
	QSqlQuery query;
	query.prepare("SELECT COUNT (*) FROM Users WHERE Username = ?");
	query.bindValue(0, username);
	bool result = query.exec() == true ? true : false;
	if (result == true)
	{
		query.next();
		int result = query.value(0).toInt();
		if (result == 1)
		{
			//set LastLogoutDate
			query.clear();
			query.exec("BEGIN IMMEDIATE TRANSACTION");
			query.prepare("UPDATE Users SET LastLogoutDate = ?  WHERE Username = ?");
			query.bindValue(0, Utilities::GetCurrentDateTime());
			query.bindValue(1, username);
			bool result = query.exec() == true ? true : false;
			query.exec("COMMIT");
			if (result == false)
			{
				Utilities::MBAlarm("User has not been logged out properly. Please, restart application.", false);
			}
		}
	}

	LogIn *login = new LogIn(nullptr);
	login->show();
	this->close();
}
void MainApp::closeEvent(QCloseEvent *event)
{
	//Get proper user from DB
	QSqlQuery query;
	query.prepare("SELECT COUNT (*) FROM Users WHERE Username = ?");
	query.bindValue(0, username);
	bool result = query.exec() == true ? true : false;
	if (result == true)
	{
		query.next();
		int result = query.value(0).toInt();
		if (result == 1)
		{
			//set LastLogoutDate
			query.clear();
			query.exec("BEGIN IMMEDIATE TRANSACTION");
			query.prepare("UPDATE Users SET LastLogoutDate = ? WHERE Username = ?");
			query.bindValue(0, Utilities::GetCurrentDateTime());
			query.bindValue(1, username);
			bool result = query.exec() == true ? true : false;
			query.exec("COMMIT");
			if (result == false)
			{
				Utilities::MBAlarm("User has not been logged out properly. Please, restart application.", false);
			}

		}
	}

	event->accept();
}
//Faces base
void MainApp::AdjustFaceBaseTW()
{
	//ui.TWFacesBase->horizontalHeader()->setSortIndicator(0, Qt::DescendingOrder);
	//Set adjusted column width
	ui.TWFacesBase->setColumnWidth(0, 80);
	ui.TWFacesBase->setColumnWidth(1, 140);
	ui.TWFacesBase->setColumnWidth(2, 140);
	ui.TWFacesBase->setColumnWidth(3, 100);
	ui.TWFacesBase->setColumnWidth(4, 80);
	ui.TWFacesBase->setColumnWidth(5, 70);
	ui.TWFacesBase->setColumnWidth(6, 70);
	ui.TWFacesBase->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	//Disable dotted border
	ui.TWFacesBase->setFocusPolicy(Qt::NoFocus);
}
void MainApp::AddRowToFB(int ID, QString name, QString surname)
{
	ui.TWFacesBase->setSortingEnabled(false);
	QWidget *widget;
	QPushButton *button;
	QHBoxLayout *layout;
	QTableWidgetItem *item;
	int rowCount = ui.TWFacesBase->rowCount();

	//Inserts an empty row into the table at row
	ui.TWFacesBase->insertRow(ui.TWFacesBase->rowCount());

	//Set the widget in the cell
	item = new QTableWidgetItem(QVariant(ID).toString());
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	ui.TWFacesBase->setItem(rowCount, 0, item);

	item = new QTableWidgetItem(name);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	ui.TWFacesBase->setItem(rowCount, 1, item);

	item = new QTableWidgetItem(surname);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	ui.TWFacesBase->setItem(rowCount, 2, item);

	//New widget
	widget = new QWidget();
	//Button to go to the folder (in the cell)
	button = new QPushButton(ui.TWFacesBase);
	button->setFixedSize(25, 25);
	button->setStyleSheet("QPushButton{border-image: url(:/Resources/Images/folder.png) 0 0 0 0 stretch stretch; border: none; margin: 0px; padding: 0px;} QPushButton:hover{border-image: url(:/Resources/Images/folderHover.png) 0 0 0 0 stretch stretch;}");
	button->setFocusPolicy(Qt::NoFocus);
	//Layout
	layout = new QHBoxLayout(ui.TWFacesBase);
	layout->addWidget(button);
	layout->setAlignment(Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
	//Set the layout on the widget
	widget->setLayout(layout);
	ui.TWFacesBase->setCellWidget(rowCount, 3, widget);
	connect(button, &QPushButton::clicked, this, [this, ID] {OpenFileExplorer(ID); });

	//New widget
	widget = new QWidget();
	//Button to take picture (in the cell)
	button = new QPushButton(ui.TWFacesBase);
	button->setFixedSize(25, 25);
	button->setStyleSheet("QPushButton{border-image: url(:/Resources/Images/snapshot.png) 0 0 0 0 stretch stretch; border: none; margin: 0px; padding: 0px;} QPushButton:hover{border-image: url(:/Resources/Images/snapshotHover.png) 0 0 0 0 stretch stretch;}");
	button->setFocusPolicy(Qt::NoFocus);
	//Layout
	layout = new QHBoxLayout(ui.TWFacesBase);
	layout->addWidget(button);
	layout->setAlignment(Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
	//Set the layout on the widget
	widget->setLayout(layout);
	//Set the widget in the cell
	ui.TWFacesBase->setCellWidget(rowCount, 4, widget);
	connect(button, &QPushButton::clicked, this, [this, ID] {TakePicture(ID); });

	//New widget
	widget = new QWidget();
	//Button to edit person (in the cell)
	button = new QPushButton(ui.TWFacesBase);
	button->setFixedSize(25, 25);
	button->setStyleSheet("QPushButton{border-image: url(:/Resources/Images/edit.png) 0 0 0 0 stretch stretch; border: none; margin: 0px; padding: 0px;} QPushButton:hover{border-image: url(:/Resources/Images/editHover.png) 0 0 0 0 stretch stretch;}");
	button->setFocusPolicy(Qt::NoFocus);
	//Layout
	layout = new QHBoxLayout(ui.TWFacesBase);
	layout->addWidget(button);
	layout->setAlignment(Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
	//Set the layout on the widget
	widget->setLayout(layout);
	//Set the widget in the cell
	ui.TWFacesBase->setCellWidget(rowCount, 5, widget);
	connect(button, &QPushButton::clicked, this, [this, ID] {EditPerson(ID); });

	//New widget
	widget = new QWidget();
	//Button to remove person (in the cell)
	button = new QPushButton(ui.TWFacesBase);
	button->setFixedSize(25, 25);
	button->setStyleSheet("QPushButton{border-image: url(:/Resources/Images/remove.png) 0 0 0 0 stretch stretch; border: none; margin: 0px; padding: 0px;} QPushButton:hover{border-image: url(:/Resources/Images/removeHover.png) 0 0 0 0 stretch stretch;}");
	button->setFocusPolicy(Qt::NoFocus);
	//Layout
	layout = new QHBoxLayout(ui.TWFacesBase);
	layout->addWidget(button);
	layout->setAlignment(Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
	//Set the layout on the widget
	widget->setLayout(layout);
	//Set the widget in the cell
	ui.TWFacesBase->setCellWidget(rowCount, 6, widget);
	connect(button, &QPushButton::clicked, this, [this, ID] {RemovePerson(ID); });
	ui.TWFacesBase->setSortingEnabled(true);
}
void MainApp::FillFacesBaseTW()
{
	AdjustFaceBaseTW();
	//TODO: Get data from DB
	//http://doc.qt.io/qt-5/qhboxlayout.html
	//http://www.qtcentre.org/threads/3416-Center-a-widget-in-a-cell-on-a-QTableWidget
	//https://stackoverflow.com/a/14715980


	for (int i = helpVar; helpVar < 20; helpVar++)
	{
		AddRowToFB(helpVar, "Kazimierz", "Aleksandrowicz");
	}
}
void MainApp::UpdateDBAfterCellChanged(int row, int column)
{
	//TODO
	Utilities::MBAlarm("DB Update " + QVariant(row).toString() + " " + QVariant(column).toString(), true);
}
void MainApp::OpenFileExplorer(int ID)
{
	QString path = ".\\FaceBase\\" + QVariant(ID).toString();
	//https://stackoverflow.com/a/11517874
	QDir folder(path);
	if (!folder.exists()) 
	{
		folder.mkpath(".");
	}
	//https://stackoverflow.com/q/3490336
	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
void MainApp::TakePicture(int ID)
{
	//TODO
	Utilities::MBAlarm("TakePicture " + QVariant(ID).toString(), true);
}
void MainApp::LESearchFBChanged()
{
	QString typedSurname = ui.LESearchFB->text();
	for (int i = 0; i < ui.TWFacesBase->rowCount(); i++)
	{
		if (ui.TWFacesBase->item(i, 2)->text().startsWith(typedSurname, Qt::CaseInsensitive))
		{
			ui.TWFacesBase->showRow(i);
		}
		else
		{
			ui.TWFacesBase->hideRow(i);
		}
	}
}
void MainApp::AddPerson()
{
	QString name = ui.LEUsername->text();
	QString surname = ui.LESurname->text();

	//TODO: Add to DB
	if (name != "" && surname != "") 
	{
		AddRowToFB(helpVar, name, surname);
		helpVar++;
	}
	else
	{
		Utilities::MBAlarm("At least one field is incomplete", false);
	}

}
void MainApp::EditPerson(int ID)
{
	//TODO
	for (size_t i = 0; i < ui.TWFacesBase->rowCount(); i++)
	{
		if (ID == ui.TWFacesBase->item(i, 0)->text().toInt())
		{
			//Create Qdialog
			QDialog *qDialog = new QDialog(0, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
			QPixmap pixmap(iconPath);
			QIcon ButtonIcon(pixmap);
			qDialog->setWindowIcon(ButtonIcon);

			QDialogButtonBox *dialogBB = new QDialogButtonBox();
			dialogBB->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

			QFormLayout *formLayout = new QFormLayout();
			QLabel *nameL = new QLabel();
			nameL->setText("Name:");
			QLineEdit *nameLE = new QLineEdit();
			//Set name form table
			nameLE->setText(ui.TWFacesBase->item(i, 1)->text());

			QLabel *surnameL = new QLabel();
			surnameL->setText("Surname:");
			QLineEdit *surnameLE = new QLineEdit();
			//Set surname from table
			surnameLE->setText(ui.TWFacesBase->item(i, 2)->text());

			//Check if fields are empty
			connect(nameLE, &QLineEdit::textEdited, this, [this, nameLE, surnameLE, dialogBB]
			{
				if (nameLE->text() == "" || surnameLE->text() == "")
				{
					dialogBB->button(QDialogButtonBox::Ok)->setDisabled(true);
				}
				else
				{
					dialogBB->button(QDialogButtonBox::Ok)->setEnabled(true);
				}
			});
			connect(surnameLE, &QLineEdit::textEdited, this, [this, nameLE, surnameLE, dialogBB]
			{
				if (surnameLE->text() == "" || nameLE->text() == "")
				{
					dialogBB->button(QDialogButtonBox::Ok)->setDisabled(true);
				}
				else
				{
					dialogBB->button(QDialogButtonBox::Ok)->setEnabled(true);
				}
			});

			formLayout->addRow(nameL, nameLE);
			formLayout->addRow(surnameL, surnameLE);

			QGridLayout *gridLayout = new QGridLayout();
			gridLayout->addLayout(formLayout, 0, 0, 1, 2, Qt::AlignCenter);

			qDialog->setLayout(gridLayout);

			gridLayout->addWidget(dialogBB, 3, 0, 1, 2, Qt::AlignRight);
			qDialog->adjustSize();
			qDialog->setMinimumSize(qDialog->width(), qDialog->height());
			qDialog->setMaximumSize(qDialog->width(), qDialog->height());

			connect(dialogBB, SIGNAL(accepted()), qDialog, SLOT(accept()));
			connect(dialogBB, SIGNAL(rejected()), qDialog, SLOT(reject()));

			int result = qDialog->exec();
			if (result == QDialog::Accepted)
			{
				if (nameLE->text() != "" && surnameLE->text() != "") 
				{
					ui.TWFacesBase->item(i, 1)->setText(nameLE->text());
					ui.TWFacesBase->item(i, 2)->setText(surnameLE->text());
				}
			}
			break;
		}
	}
}
void MainApp::RemovePerson(int ID)
{
	if (Utilities::MBQuestion("<b>Warning</b>: Are you sure, you want to <b>remove</b> person with ID: " + ((QVariant)ID).toString() + "?"))
	{
		//Remove row from table
		for (size_t i = 0; i < ui.TWFacesBase->rowCount(); i++)
		{
			if (ID == ui.TWFacesBase->item(i, 0)->text().toInt())
			{
				ui.TWFacesBase->removeRow(i);
				break;
			}
		}
	}
		//TODO::
		//Remove from DB
		//Remove folder
}