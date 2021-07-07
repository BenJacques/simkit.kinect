#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "date.h"
#include "utils.h"
#include "kinect.h"
#include "buttons.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Start_clicked();

    void on_pushButton_Config_clicked();

    void on_pushButton_Exit_clicked();

    void on_updateImage();

    void on_pushButton_ToggleView_clicked();

    void on_pushButton_Back_clicked();

    void on_tabWidget_currentChanged(int index);

protected:
    void showEvent(QShowEvent *ev);

private:
    const char* root_dir;
    std::string base_dir;
    kinect::Kinect kinect_device;
    buttons::Buttons button_mapping;
    settings::Settings curr_settings;
    std::thread kinect_stream_thread;
    std::thread log_temps_thread;
    std::thread button_thread;
    bool show_color_image;

    Ui::MainWindow *ui;
    void initializeUi();
    void logTemparatures();
    void handleExit();
    void handleKinectStart();
    void handleKinectStop();
    QImage readDepthImageAsQImage(std::string file_name);
};

#endif // MAINWINDOW_H
