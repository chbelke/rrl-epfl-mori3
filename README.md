# Joint-Utility-Mori-Program

**Authors:** Christoph Belke, Kevin Holdcroft, Alex Sigrist, Jamie Paik - *Reconfigurable Robotics Lab, EPFL*

This repo contains all the code relevant to controlling and communication with the third version of RRL's modular origami robot, **Mori**.


# Overview
 - MoriController.X: Main low-level controller
 - ESP8266: Control for ESP8266EX wifi chip
 - External: Overall external control

 ## Mori controller
The main controller of a module consists of a microcontroller driving all core functionalities that interfaces with an ESP8266 over Serial. The 16-bit microcontroller from Microchip, dsPIC33EP512GM604 ([datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/70000689d.pdf)), is programmed via ([MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)).

 ## ESP8266 wifi
Firmware flashed via Arduino IDE. ESP8266 hardware must be installed: https://arduino-esp8266.readthedocs.io/en/latest/installing.html
The above includes the following libraries:
`ESP8266WiFi.h`
`ArduinoOTA.h`

Other libraries:
`PubSubClient.h` (https://github.com/knolleary/pubsubclient, included in repo)

 ## Global Control:
Implemented in Python 3.6.6

Nonstandard libraries:
`paho-mqtt` (https://pypi.org/project/paho-mqtt/)
`termcolor` (https://pypi.org/project/termcolor/)
`numpy` (https://pypi.org/project/numpy/)
`tkinter` (https://wiki.python.org/moin/TkInter)

 ## Other:
 MQTT broker is handled by a Raspberry Pi B+
 (simple tutorial [HERE](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/))

## To run the gui:
Execute `python3 mainMori.py` located in the External folder

Currently it operates in the Ubuntu subenviroment for Windows (found in the app store). Run download and run XLaunch to view the GUI. (https://sourceforge.net/projects/vcxsrv/)

Install non-standard libraries:
`python3 -m pip install termcolor`
`python3 -m pip install paho-mqtt`
`python3 -m pip install numpy`
`sudo apt-get install python3-tk`

Update the .bashrc file with the following command:
`echo "export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0.0" >> ~/.bashrc`

Follow the firewall configuration instructions given here:
`https://github.com/cascadium/wsl-windows-toolbar-launcher`


# Publications

### Hardware:
[1] C. H. Belke and J. Paik. **Mori: A Modular Origami Robot**. *IEEE/ASME Transactions on Mechatronics,* vol. 22, no. 5, pp. 2153-2164, Oct. 2017. **[link](https://doi.org/10.1109/TMECH.2017.2697310)**

[2] C. H. Belke and J. Paik. **Automatic Couplings with Mechanical Overload Protection for Modular Robots**. *IEEE/ASME Transactions on Mechatronics,* vol. XX, no. X, pp. XXXX-XXXX, Jun. 2019. **[link](https://doi.org/10.1109/TMECH.2019.2907802)**


### Algorithms:
[3] M. Yao, C. H. Belke, H. Cui and J. Paik. **A reconfiguration strategy for modular robots using origami folding**. *The International Journal of Robotics Research,* vol. 38, no. 1, pp. 73-89, Jan. 2019. **[link](https://doi.org/10.1177%2F0278364918815757)**

[4] M. Yao, X. Xiao, C. H. Belke, H. Cui and J. Paik. **Optimal Distribution of Active Modules in Reconfiguration Planning of Modular Robots**. *Journal of Mechanisms and Robotics,* vol. 11, no. 1, pp. 011017-011017-9, Dec. 2018. **[link](https://doi.org/10.1177%2F0278364918815757)**

[5] M. Yao, H. Cui, X. Xiao, C. H. Belke and J. Paik. **Towards Peak Torque Minimization for Modular Self-Folding Robots**. *2018 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS),* Madrid, 2018, pp. 7975-7982. **[link](https://doi.org/10.1109/IROS.2018.8593648)**


If you use Mori v3 within an academic work, please cite:

    @article{
    	*INSERT BIBTEX HERE*
     }


