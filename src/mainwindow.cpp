#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <QMessageBox>
#include <unistd.h>

#include "common_macros.h"
#include "date.h"
#include "utils.h"
#include "kinect.h"
#include "buttons.h"
#include "settings.h"


std::atomic <bool> temparature_logging_enabled (false);



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


void MainWindow::logTemparatures(){
    const char *fileDirectory = base_dir.c_str();
    temparature_logging_enabled = true;
    std::string temparature_file_name = fileDirectory;
    temparature_file_name += "/temparature_readings.csv";
    std::ofstream file_object(temparature_file_name);
    std::string header = "Timestamp,CPU Temp (C),GPU Temp (C),Kinect Temp (C)\n";
    file_object.write(header.c_str(), header.length());

    while (temparature_logging_enabled){
        double cpu_temp;
        double gpu_temp;
        double kinect_temp;
        bool nanoTempsRead = readTemparatures(cpu_temp, gpu_temp);
        bool kinectTempRead = kinect_device.GetRecentTemparature(kinect_temp);
        if (nanoTempsRead && kinectTempRead){
            std::string s = date::format("%T", std::chrono::system_clock::now());
            char *output;
            int len = asprintf(&output, "%s,%f,%f,%f\n",s.c_str(),cpu_temp,gpu_temp,kinect_temp);
            file_object.write(output,len);
            free(output);
            file_object.flush();
        }

        usleep(1*1000*1000); // Log every 60 seconds
    }
    file_object.flush();
    file_object.close();
}

void MainWindow::initializeUi()
{
    root_dir = "/media/FarmData01/Datasets/";
    
    // Set up GPIO pins on the Nano
    button_mapping.SetupPins();
    button_thread = button_mapping.PollInputsThread(); 

    // Create the directory structure for saving data
    std::string s = date::format("%m_%d_%Y-%H_%M_%S", std::chrono::system_clock::now());
    printf("Time: %s\n", s.c_str());

    if (dirExists(root_dir) ==false){
        QMessageBox msgBox;
        msgBox.setText("No external hard drive detected.\nWould you like to store data locally instead?");
        msgBox.setInformativeText("Save locally?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int retVal = msgBox.exec();
        if (retVal == QMessageBox::Yes){
            root_dir = "~/Documents/Datasets/";
        }
        else {
            return;
        }
    }

    //Create the needed directories and load settings
    base_dir = root_dir;
    base_dir += s;
    printf("Creating data save directories at %s\n", base_dir.c_str());
    if (curr_settings.CreateDataCaptureDirectories(base_dir.c_str()) == false){
        return;
    }

    printf("Loading settings...\n");
    if (curr_settings.LoadSettings("config.json") == false){
        return;
    }


    // Connect to the Kinect 
    if (kinect_device.Connect(-1, curr_settings) == false){
        printf("Failed to connect to Kinect.\n");
        return;
    }

    // Try saving the calibration file if possible
    kinect_device.TrySaveCalibrationFile(base_dir.c_str());


    log_temps_thread = std::thread(&MainWindow::logTemparatures, this);

}

void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    initializeUi();
}


void MainWindow::on_pushButton_Start_clicked()
{
    if (kinect_stream_thread.joinable() == false) {
        handleKinectStart();
    }
    else {
        handleKinectStop();
    }
}

void MainWindow::on_actionStart_triggered()
{
    if (kinect_stream_thread.joinable() == false) {
        handleKinectStart();
    }
    else {
        handleKinectStop();
    }
}

void MainWindow::on_pushButton_Config_clicked()
{

}

void MainWindow::on_actionConfig_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    handleExit();
}

void MainWindow::on_pushButton_Exit_clicked()
{
    handleExit();
}

void MainWindow::on_actionVersion_triggered()
{

}

void MainWindow::handleKinectStart()
{
    if (kinect_stream_thread.joinable() == false){
        ui->pushButton_Start->setText("Stop Streaming");
        kinect_stream_thread = kinect_device.RunThread();
    }
}

void MainWindow::handleKinectStop()
{
    kinect_device.Stop();
    if (kinect_stream_thread.joinable()){
        kinect_stream_thread.join();
    }
    ui->pushButton_Start->setText("Start Streaming");
}

void MainWindow::handleExit()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to exit?");
    msgBox.setInformativeText("Exit?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int retVal = msgBox.exec();
    if (retVal == QMessageBox::No){
        return;
    }


    
    if (kinect_stream_thread.joinable()){
        kinect_device.Stop();
        kinect_stream_thread.join();
    }

    printf("**KINECT THREAD DONE****\n");; 
    temparature_logging_enabled = false;
    button_mapping.Close();
    button_thread.join();
    printf("**BUTTON THREAD DONE****\n");
    log_temps_thread.join();
    printf("**LOG TEMPS THREAD DONE****\n");
    QCoreApplication::exit();
}