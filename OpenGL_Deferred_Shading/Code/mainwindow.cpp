#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- Functions that listen for widget events
// forewards to the mainview

void MainWindow::on_ResetRotationButton_clicked(bool checked)
{
    Q_UNUSED(checked);
    ui->RotationDialX->setValue(0);
    ui->RotationDialY->setValue(0);
    ui->RotationDialZ->setValue(0);
    ui->mainView->setRotation(0, 0, 0);
}

void MainWindow::on_RotationDialX_sliderMoved(int value)
{
    ui->mainView->setRotation(value,
                              ui->RotationDialY->value(),
                              ui->RotationDialZ->value());
}

void MainWindow::on_RotationDialY_sliderMoved(int value)
{
    ui->mainView->setRotation(ui->RotationDialX->value(),
                              value,
                              ui->RotationDialZ->value());
}

void MainWindow::on_RotationDialZ_sliderMoved(int value)
{
    ui->mainView->setRotation(ui->RotationDialX->value(),
                              ui->RotationDialY->value(),
                              value);
}

void MainWindow::on_ResetScaleButton_clicked(bool checked)
{
    Q_UNUSED(checked);
    ui->ScaleSlider->setValue(100);
    ui->mainView->setScale(100);
}

void MainWindow::on_ScaleSlider_sliderMoved(int value)
{
    ui->mainView->setScale(value);
}

void MainWindow::on_AllButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setCurrentTexture(MainView::ALL);
        ui->mainView->update();
    }
}

void MainWindow::on_ColorButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setCurrentTexture(MainView::COLOR);
        ui->mainView->update();
    }
}

void MainWindow::on_NormalsButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setCurrentTexture(MainView::NORMALS);
        ui->mainView->update();
    }
}

void MainWindow::on_PositionButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setCurrentTexture(MainView::POSITION);
        ui->mainView->update();
    }
}

void MainWindow::on_DepthButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setCurrentTexture(MainView::DEPTH);
        ui->mainView->update();
    }
}

void MainWindow::on_enableSun_toggled(bool checked)
{
    ui->mainView->toggleSun(checked);
    ui->mainView->update();
}

void MainWindow::on_enableLights_toggled(bool checked)
{
    ui->mainView->toggleLights(checked);
    ui->mainView->update();
}
