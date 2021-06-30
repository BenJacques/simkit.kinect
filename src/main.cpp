#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <k4a/k4a.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <thread>
#include <string>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#include "common_macros.h"
#include "date.h"
#include "utils.h"
#include "kinect.h"
#include "buttons.h"
#include "settings.h"

#include "mainwindow.h"
#include <QApplication>



int main(int argc, char* argv[]) {
    UNUSED_PARAMS(argc, argv);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

    /*

    
    
    printf("Waiting for button press to start...\n");
    while (true){
        if (button_mapping.start_button_clicked){
            printf("Start button clicked.\n");
            button_mapping.start_button_clicked = false;
            if (kinect_stream_thread.joinable() == false){
                kinect_stream_thread = kinect_device.RunThread();
            }
        }
        if (button_mapping.stop_button_clicked)
        {
            printf("Stop button clicked.\n");
            button_mapping.stop_button_clicked = false;
            kinect_device.Stop();
            if (kinect_stream_thread.joinable()){
                kinect_stream_thread.join();
            }
        }

        if (button_mapping.exit_button_clicked){
            printf("Exit button clicked.\n");
            button_mapping.exit_button_clicked = false;
            //TODO: This button does not seem to be working on the board I have.
            //It always reads high and therefore exits immediately.
            //Uncomment out the break to make exit functionality work.
            break;
        }
        

    }

    kinect_device.Stop();
    if (kinect_stream_thread.joinable()){
        kinect_stream_thread.join();
    }

    printf("**KINECT THREAD DONE****\n");; 
    temparature_logging_enabled = false;
    button_mapping.Close();
    button_thread.join();
    printf("**BUTTON THREAD DONE****\n");
    log_temps_thread.join();
    printf("**LOG TEMPS THREAD DONE****\n");

    kinect_device.Stop();
 */

    return 0;
}
