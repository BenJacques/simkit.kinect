# Nano Setup
- Flash nano SD using setup instructions here: https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#write

# Update Ubuntu Packages
sudo apt update
sudo apt upgrade

sudo apt-get install cmake ninja-build
sudo apt-get install curl zip unzip tar

# Install vcpkg and associated packages
cd ~/Documents/
mkdir repos
cd repos
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
export VCPKG_FORCE_SYSTEM_BINARIES=1
./vcpkg integrate install
./vcpkg install jsoncpp


# Install Azure Kinect SDK
curl -sSL https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
sudo apt-add-repository https://packages.microsoft.com/ubuntu/18.04/multiarch/prod
sudo apt-get update
sudo apt-get install libk4a1.4-dev

'TODO: copy 99-k4a.rules into /etc/udev/rule.d/

# Install JetsonGPIO
git clone https://github.com/pjueon/JetsonGPIO
cd JetsonGPIO/build
make all
sudo make install

sudo groupadd -f -r gpio
sudo usermod -a -G gpio your_user_name

sudo cp JetsonGPIO/99-gpio.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger

# Other Notes
It is best to disable the auto lock within Ubuntu. This can be done by going to System Settings -> Brightness and Lock