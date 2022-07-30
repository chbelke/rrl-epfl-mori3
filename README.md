# Joint-Utility-Mori-Program

**Authors:** Christoph Belke, Kevin Holdcroft, Alex Sigrist, Jamie Paik - *Reconfigurable Robotics Lab, EPFL*

This repo contains all the code relevant to controlling and communication with the third version of RRL's modular origami robot, **Mori**.


# Overview
 - MoriController.X: Main low-level controller
 - ESP8266: Control for ESP8266EX wifi chip
 - External: Overall external control

 ## Mori controller
The main controller of a module consists of a microcontroller driving all core functionalities that interfaces with an ESP8266 over Serial. The 16-bit microcontroller from Microchip, dsPIC33EP512GM604 ([datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/70000689d.pdf)), is programmed via ([MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)).

 ## ESP8266 WiFi
Firmware flashed via Arduino IDE. ESP8266 hardware must be installed: https://arduino-esp8266.readthedocs.io/en/latest/installing.html
The above includes the following libraries:
`ESP8266WiFi.h`
`ArduinoOTA.h`

Other libraries:
`PubSubClient.h` (https://github.com/knolleary/pubsubclient, included in repo)

 ## Global Control:
Implemented in Python 3.6.6

Install non-standard libraries in `requirements.txt`

## To run the gui:
Execute `python3 mainMori.py` located in the External folder.

It has been tested on Ubuntu 20.04, as well as in the Ubuntu subenviroment for Windows (WSL 1). 

`names.py` must be changed to match the last six characters in the ESP's MAC address.

### To run with WSL 1:
Run download and run XLaunch to view the GUI. (https://sourceforge.net/projects/vcxsrv/)

Update the .bashrc file with the following command:
`echo "export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0.0" >> ~/.bashrc`

Follow the firewall configuration instructions given here:
`https://github.com/cascadium/wsl-windows-toolbar-launcher`

 ## MQTT:
 MQTT broker is handled by a desktop running Ubuntu 20.04 or Raspberry Pi.
 (simple tutorial [HERE](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/))

 The broker should have a fixed local IP of *192.168.1.2*

## Recreating figures
Graphs in the Mori3 publication are generated from in the `ProgramInputs` folder. All necessary data is within the folders.
 
All have been executed using MATLAB 2020b.

The reconfigurable surface graph is within the `SurfaceFigure` folder and generated via executing `Mori3_SurfacePlot_04.m` with `HexUpData.mat` in the working directory.

The manupulator graph is within the `ManipulatorFigure` folder and generated via seperate `Arm_Overhead_2.m` and `Arm_Analysis_4.m` scripts.

The track graph is within the `TrackFigure` folder and generated via `Track_Analysis_4.m` with `Track_Data.m` and `Track_model.m` in the working directories.

# Publications

### Hardware:
[1] C. H. Belke and J. Paik. **Mori: A Modular Origami Robot**. *IEEE/ASME Transactions on Mechatronics,* vol. 22, no. 5, pp. 2153-2164, Oct. 2017. **[link](https://doi.org/10.1109/TMECH.2017.2697310)**

[2] C. H. Belke and J. Paik. **Automatic Couplings with Mechanical Overload Protection for Modular Robots**. *IEEE/ASME Transactions on Mechatronics,* vol. 24, no. 3, pp. 1420-1426, Jun. 2019. **[link](https://doi.org/10.1109/TMECH.2019.2907802)**

[3] K. A. Holdcroft, C. H. Belke, S. Bennani and J. Paik, **3PAC: A Plug-and-Play System for Distributed Power Sharing and Communication in Modular Robots**. *IEEE/ASME Transactions on Mechatronics,* vol. 27, no. 2, pp. 858-867, April 2022, **[link](https://doi.org/10.1109/TMECH.2021.3073538)**


### Algorithms:
[4] M. Yao, C. H. Belke, H. Cui and J. Paik. **A reconfiguration strategy for modular robots using origami folding**. *The International Journal of Robotics Research,* vol. 38, no. 1, pp. 73-89, Jan. 2019. **[link](https://doi.org/10.1177%2F0278364918815757)**

[5] M. Yao, X. Xiao, C. H. Belke, H. Cui and J. Paik. **Optimal Distribution of Active Modules in Reconfiguration Planning of Modular Robots**. *Journal of Mechanisms and Robotics,* vol. 11, no. 1, pp. 011017-011017-9, Dec. 2018. **[link](https://doi.org/10.1177%2F0278364918815757)**

[6] M. Yao, H. Cui, X. Xiao, C. H. Belke and J. Paik. **Towards Peak Torque Minimization for Modular Self-Folding Robots**. *2018 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS),* Madrid, 2018, pp. 7975-7982. **[link](https://doi.org/10.1109/IROS.2018.8593648)**

[7] K. A. Holdcroft, C. H. Belke, A. Sigrist, and J. Paik. **Holdcroft, Kevin, et al. "Hybrid Wireless–Local Communication via Information Propagation for Modular Robotic Synchronization Applications**. *Advanced Intelligent Systems,* vol. 4, no. 1, pp. 2100226, Mar. 2022, **[link](https://doi.org/10.1002/aisy.202100226)**


If you use Mori3 within an academic work, please cite\*:

    @article{
    	*INSERT BIBTEX HERE*
     }

\*Manuscript under review
