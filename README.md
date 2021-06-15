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

# Build and Run

This project just has a simple main.cpp that prints the C++ version string.

```
./build.sh
build/starter

# Prints "__cplusplus: 201703"
```
- Follow instructions here: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md#debian-package
   - On Ubuntu 18.04 with ARM64, use the following commands to install the `libk4a1.4-dev` package:
   ```
   curl -sSL https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
   ```
   ```
   sudo apt-add-repository https://packages.microsoft.com/ubuntu/18.04/multiarch/prod
   ```
   ```
   sudo apt-get update
   ```
   Then install the libk4a package with
   ```
   sudo apt-get install libk4a1.4-dev
   ```
   More information can be found at https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md#debian-package

# For buttons
sudo apt-get install libx11-dev
sudo apt-get install libxtst-dev

# For json configuration
https://github.com/open-source-parsers/jsoncpp

libx11-dev

libmesa-dev __NOTICE__: Package cannot be found. Need to update instructions.

libxi-dev

libxext-dev
