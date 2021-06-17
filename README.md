# Use this repo as base of new repo

1. Clone repo into a new local project folder:

```
git clone git@github.com:spindance/starter.cpp.git my-new-project
cd my-new-project
```

2. Rename the URL of your `origin` remote to `upstream`:

```
git remote rename origin upstream
```

3. Add a new `origin` remote that points to your new project repo (you many
   need to create the repo in github first):

```
git remote add origin git@github.com:spindance/my-new-project.git
```

4. Double-check your remotes are correct:

```
git remote -v
```

# Installing Prerequisites

1) Install pip
   - This allows us to install the version of CMake required by VCPKG. The highest version of cmake available in ubuntu's apt repository is incompatible with the version of VCPKG we will be installing later.
   ```
   sudo apt-get update
   sudo apt-get install python3-pip
   ```
2) Install cmake via pip
   ```
   python3 -m pip install --upgrade pip
   python3 -m pip install --upgrade cmake
   ```
3) Download and install VCPKG
   ```
   git clone https://github.com/microsoft/vcpkg
   ./vcpkg/bootstrap-vcpkg.sh
   ```
4) Enable VCPKG_FORCE_SYSTEM_BINARIES for VCPKG
   ```
   cd ./vcpkg
   export VCPKG_FORCE_SYSTEM_BINARIES=1
   ```
5) Integrate VCPKG installation
   ```
   ./vcpkg integrate install
   ```
6) Install jsoncpp with vcpkg
   ```
   ./vcpkg install jsoncpp
   ```
7) Install json configuration and button required packages
   ```
   sudo apt-get update
   sudo apt-get install libx11-dev libxi-dev libxext-dev libxtst-dev
   ```
   - See https://github.com/open-source-parsers/jsoncpp for more information.

   - __DEVELOPER NOTE__: Package `libmesa-dev` cannot be located in apt repository. Doesn't appear to cause any errors but we should look into this.
8) Install Azure Kinect Sensor SDK
   ```
   curl -sSL https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
   sudo apt-add-repository https://packages.microsoft.com/ubuntu/18.04/multiarch/prod
   sudo apt-get update
   sudo apt-get install libk4a1.4-dev
   ```
   - See https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md#debian-package for more information.
   - __NOTE__: The `apt-add-repository` line has a specific URL for Ubuntu 18.04 ARM64 builds. If the ubuntu version is different or if the system has an AMD64 processor, you will need to update this URL to reflect these changes. See Microsoft's github repo linked above for additional information on this step.
9) Setup JetsonGPIO dependencies
   ```
   git clone https://github.com/jetsonhacks/jetsonGPIO.git
   cd JetsonGPIO/build
   make all
   sudo make install
   ```
   - See https://github.com/pjueon/JetsonGPIO for more information.
10) Update the `simkit.kinect/build.sh` script with correct path.
      - Within `build.sh` there is a line where `-DCMAKE_TOOLCHAIN_FILE` is specified.
      - Update this filepath to match the path to your `vcpkg.cmake` file.
11) Verify that you have an external drive located at `/media`. Otherwise you'll need to update the code for `simkit.kinect` to reflect the location of the external drive which will be used to save captured images from the kinect.
      - In the event that you need to update this location, open `simkit.kinect/src/main.cpp` and near the top of the file there is a line with the code shown below.
      - Update the filepath in this line to match an existing folder on an external drive. If the folder does not exist, you will receive an error saying that the external drive could not be located.
      ```
      static const char* root_dir = "/media/FarmData01/Datasets/";
      ```
      

# Build and Run

This project just has a simple main.cpp that prints the C++ version string.

```
./build.sh
build/starter

# Prints "__cplusplus: 201703"
```
