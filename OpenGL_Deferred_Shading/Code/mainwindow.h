#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_ResetRotationButton_clicked(bool checked);
    void on_RotationDialX_sliderMoved(int value);
    void on_RotationDialY_sliderMoved(int value);
    void on_RotationDialZ_sliderMoved(int value);

    void on_ResetScaleButton_clicked(bool checked);
    void on_ScaleSlider_sliderMoved(int value);

    void on_ColorButton_toggled(bool checked);
    void on_NormalsButton_toggled(bool checked);
    void on_DepthButton_toggled(bool checked);
    void on_AllButton_toggled(bool checked);
    void on_PositionButton_toggled(bool checked);
    void on_enableSun_toggled(bool checked);
    void on_enableLights_toggled(bool checked);
    void on_perfAnalysisButton_toggled(bool checked);
};

#endif // MAINWINDOW_H
