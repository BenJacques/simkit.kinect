# Nano Setup
- Flash nano SD using setup instructions here: https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#write

# Update Ubuntu Packages
This step sets up the basic needs for getting the operating system in a good state to use.

`sudo apt update`

`sudo apt upgrade`

`sudo apt-get install cmake ninja-build`

`sudo apt-get install curl zip unzip tar`

`sudo apt-get install qt5-default`

# Install vcpkg and associated packages

This step sets up the `vcpkg` tool that is currently used to install the needed `jsoncpp` package. It can be used for other packages in the future.

`cd ~/Documents/`

`mkdir repos`

`cd repos`

`git clone https://github.com/Microsoft/vcpkg.git`

`cd vcpkg`

`./bootstrap-vcpkg.sh`

`export VCPKG_FORCE_SYSTEM_BINARIES=1`

`./vcpkg integrate install`

`./vcpkg install jsoncpp`

`./vcpkg install opencv4`

`./vcpkg install loguru`


# Install Azure Kinect SDK

This step installs and sets up the required SDK for connecting to and interacting with the Azure Kinect device.

`curl -sSL https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -`

`sudo apt-add-repository https://packages.microsoft.com/ubuntu/18.04/multiarch/prod`

`sudo apt-get update`

`sudo apt-get install libk4a1.4-dev`


# Install JetsonGPIO

This step sets up the JetsonGPIO c++ libraray. This is used to tie the buttons on the box to the data collection app.

`git clone https://github.com/pjueon/JetsonGPIO`

`cd JetsonGPIO/build`

`make all`

`sudo make install`

`sudo groupadd -f -r gpio`

`sudo usermod -a -G gpio <your_user_name>`

`cd ..`

`sudo cp ./99-gpio.rules /etc/udev/rules.d/`

`sudo udevadm control --reload-rules && sudo udevadm trigger`

`sudo apt-get install libxtst-dev`

# Setup External Drive

- Open Disk Utility in Ubuntu Start
- Plug in portalble SSD
- Format to EXT4 if not already
- Find the Drive and unmount 
- Edit mount options
- Mount at start, MountPoint=/media/FarmData01, IdentifyAs=/dev/sda1

# Pull Simkit Repo and Setup

`git clone https://gitlab.msu.edu/animallab/simkit.kinect.git`

`cd simkit.kinect`

`sudo cp ./99-k4a.rules /etc/udev/rule.d/`

`sudo udevadm control --reload-rules && sudo udevadm trigger`

`./build.sh`

# Set up everything for autostarting the application

`sudo mkdir ~/.config/autostart`

`sudo cp ./sh.desktop ~/.config/autostart/`


# Other Notes

- It is best to disable the auto lock within Ubuntu. This can be done by going to System Settings -> Brightness and Lock

- The screen should also be set to not turn off when idle.


