# DHT2x Protocol Analyzer for Saleae Logic
This plugin implements a decoder for the DHT2x 1-wire protocol. Default timings are based on the datasheet. The decoder is a simple threshold detector which checks HIGH pulse duration. Anything above (One_duration+Zero_duration)/2 is considered 1 and below is 0. There is checking for the host start signal which is any LOW duration > 1 ms. The decoder expects a message length of 40 bits in total (not including the initial host/sensor handshake).

It is a fork of a repo with the same name written by Jacob Miller: [DHT2xProtocolAnalyzer](https://github.com/jakeson21/DHT2xProtocolAnalyzer). It has been stripped of the Visual Studio and OSx functionality and tested in "Ubuntu 64bit Linux only"!

AFAICT, this decoder is working as expected.

# Screenshot
![Overview Screenshot](images/DHT22_Saleae-Logic_snap.png)

# Build Instructions
To build on Linux, run the build_analyzer.py script. The compiled libraries can be found in the debug and release folders.

> python build_analyzer.py

This plugin requires the latest release of the Saleae AnalyzerSDK. It is available as a submodule linked to the Saleae AnalyzerSDK Github repository.

>**Note:** I had to add "-std=c++17" to the compile options so the project would build without errors!
