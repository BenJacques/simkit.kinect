#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <QMessageBox>
#include <QTimer>
#include <QWidget>
#include <unistd.h>
#include <vector>

#include "common_macros.h"
#include "date.h"
#include "utils.h"
#include "kinect.h"
#include "buttons.h"
#include "settings.h"
#include <loguru.hpp>

std::atomic <bool> temparature_logging_enabled (false);
QTimer *timer;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeUi()
{
    show_color_image = true;
    root_dir = "/media/FarmData01/Datasets/";
    
    // Set up GPIO pins on the Nano
    button_mapping.SetupPins();
    button_thread = button_mapping.PollInputsThread(); 

    // Create the directory structure for saving data
    std::string s = date::format("%m_%d_%Y-%H_%M_%S", std::chrono::system_clock::now());
    LOG_F(INFO, "Time: %s\n", s.c_str());

    if (dirExists(root_dir) ==false){
        QMessageBox msgBox;
        QFont font = QFont("Ubuntu", 20);
        msgBox.setFont(font);
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
    LOG_F(INFO, "Creating data save directories at %s\n", base_dir.c_str());
    if (curr_settings.CreateDataCaptureDirectories(base_dir.c_str()) == false){
        return;
    }

    LOG_F(INFO, "Loading settings...\n");
    if (curr_settings.LoadSettings("config.json") == false){
        return;
    }


    // Connect to the Kinect 
    if (kinect_device.Connect(-1, curr_settings) == false){
        LOG_F(ERROR, "Failed to connect to Kinect.\n");
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
    connect(timer, &QTimer::timeout, this, &MainWindow::on_updateImage);
    
}

void MainWindow::on_pushButton_ToggleView_clicked()
{
    LOG_F(INFO, "Toggle View button clicked");
    show_color_image = !show_color_image;
    if (show_color_image){
        ui->pushButton_ToggleView->setText("Show Depth \nImage");
    }
    else{
        ui->pushButton_ToggleView->setText("Show Color \nImage");
    }
}

void MainWindow::on_pushButton_Back_clicked()
{
    LOG_F(INFO, "Back button clicked");
    ui->tabWidget->setCurrentWidget(ui->tabWidget->findChild<QWidget *>("tab_Main"));
}

void MainWindow::on_updateImage()
{
    char* image_to_show;
    int image_num = kinect_device.imageCount -25;
    if (show_color_image){
        int len = asprintf(&image_to_show, "%s%d.jpeg", curr_settings.data_dirs.colorFileDirectory.c_str(),image_num);
        UNUSED_PARAM(len);
        printf("Opening image %s\n", image_to_show);

        QPixmap pm_color(image_to_show); // <- path to image file
        ui->label_Image->setPixmap(pm_color);
        ui->label_Image->setScaledContents(true);
    }
    else{
        int len = asprintf(&image_to_show, "%s%d.bin", curr_settings.data_dirs.depthFileDirectory.c_str(), image_num);
        UNUSED_PARAM(len);
        QImage img = readDepthImageAsQImage(image_to_show);
        QPixmap pm_depth = QPixmap::fromImage(img);
        ui->label_Image->setPixmap(pm_depth);
        ui->label_Image->setScaledContents(true);
    }

}

void MainWindow::on_pushButton_Start_clicked()
{
    LOG_F(INFO, "Start button clicked");
    if (kinect_stream_thread.joinable() == false) {
        handleKinectStart();
    }
    else {
        handleKinectStop();
    }
}

void MainWindow::on_pushButton_Config_clicked()
{
    LOG_F(INFO, "Config button clicked");
    ui->tabWidget->setCurrentWidget(ui->tabWidget->findChild<QWidget *>("tab_cameraViewer"));
}

void MainWindow::on_pushButton_Exit_clicked()
{
    LOG_F(INFO, "Exit button clicked");
    handleExit();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0){
        timer->stop();
    }
    else{
        if (kinect_stream_thread.joinable() == false) {
            handleKinectStart();
        }
        timer->start(500);
    }
    
}

void MainWindow::handleKinectStart()
{
    LOG_F(INFO, "Entered handleKinectStart");
    if (kinect_stream_thread.joinable() == false){
        ui->pushButton_Start->setText("Stop Streaming");
        kinect_stream_thread = kinect_device.RunThread();
        ui->statusBar->showMessage("Streaming images...");
    }
}

void MainWindow::handleKinectStop()
{
    LOG_F(INFO, "Entered handleKinectStop");
    kinect_device.Stop();
    if (kinect_stream_thread.joinable()){
        kinect_stream_thread.join();
    }
    ui->pushButton_Start->setText("Start Streaming");
    ui->statusBar->showMessage("Streaming stopped.");
}

void MainWindow::handleExit()
{
    LOG_F(INFO, "Entered handleExit");
    QMessageBox msgBox;
    QFont font = QFont("Ubuntu", 20);
    msgBox.setFont(font);
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

    LOG_F(INFO, "**KINECT THREAD DONE****\n");; 
    temparature_logging_enabled = false;
    button_mapping.Close();
    button_thread.join();
    LOG_F(INFO, "**BUTTON THREAD DONE****\n");
    log_temps_thread.join();
    LOG_F(INFO, "**LOG TEMPS THREAD DONE****\n");
    QCoreApplication::exit();
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

QImage MainWindow::readDepthImageAsQImage(std::string file_name)
{
    std::streampos size;
    std::vector<uint16_t> data_block;
    int image_size;
    

    std::ifstream file_object(file_name, std::ios::in | std::ios::binary | std::ios::ate);
    if (file_object.is_open()){
        size = file_object.tellg();
        image_size = (int)((int)size/2.0);
        data_block.resize(image_size);
        file_object.seekg(0, std::ios::beg);
        file_object.read((char*)data_block.data(), size);
        file_object.close();
    }
    else{
        printf("Could not open file.\n");
        return QImage();
    }

    uint8_t *image_data;
    image_data = new uint8_t[image_size];
    for (int i = 0; i < image_size; i++)
    {
        image_data[i] = (uint8_t)(data_block[i]/2.0);
    }
    
    QImage img = QImage((const uchar*)image_data, 512, 512, 512*sizeof(uint8_t), QImage::Format_Grayscale8);
    return img;
}