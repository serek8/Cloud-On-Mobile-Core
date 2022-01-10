# Cloud-On-Mobile-Core

An application for hosting files in your own cloud - your mobile device's disk. Access your data anytime with a fast and secure BLIK-like authentication with a web application.


### Build a native client

```
git clone https://github.com/serek8/Cloud-On-Mobile-Core.git
cd Cloud-On-Mobile-Core
mkdir build
cd build
cmake ..
cmake --build .
```

### Run a native client

```
./CloudOnMobileCoreClient cloudon.cc 9293
```

The production instance of the CloudOnMobile server is at port `9293`.
The client stores your data inside `./hosted_files` 

