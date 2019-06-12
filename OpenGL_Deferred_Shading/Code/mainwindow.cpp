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
    QVector3D rotation_default = ui->mainView->rotation_default;
    ui->RotationDialX->setValue(static_cast<int>(rotation_default.x()));
    ui->RotationDialY->setValue(static_cast<int>(rotation_default.y()));
    ui->RotationDialZ->setValue(static_cast<int>(rotation_default.z()));
    ui->mainView->setRotation(ui->mainView->rotation_default);
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

void MainWindow::on_perfAnalysisButton_toggled(bool checked)
{
    // first, reset projection
    if (checked) {
        on_ResetScaleButton_clicked(true);
        on_ResetRotationButton_clicked(true);
        ui->mainView->setRotation(ui->mainView->rotation_default);
        ui->perfAnalysisButton->setText("Stop performance test");
    } else {

        ui->perfAnalysisButton->setText("Start performance test");
    }

    // run performance analysis
    ui->mainView->perfAnalysis(checked);
}
