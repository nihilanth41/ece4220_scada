# SCADA Project
## About

## Installation 
1. Clone the repository to somewhere on Historian machine and the two TS-7250 RTUs.
```
git clone https://github.com/nihilanth41/ece4220_scada.git scada_project
```
2. On the TS-7250(s) build and load the kernel module for the RTU.
```
cd scada_project/rtu/kernel
make
insmod adc.o
```
3. On the TS-7250(s) build and run the userspace RTU program.
``` 
cd ../user
make 
./rtu.bin <port number>
```
4. Build and run the Historian on the historian machine (probably NFS1).
``` 
cd scada_project/historian
make
./historian.bin <ip address of rtu> <port number>
```
## Usage
After following the steps above button press events from the RTU should show up in the Historian log. On the historian the log can be viewed using 'l' and the historian can be exited by pressing 'q'.

## Authors
Zachary Rump  
Tyler Shipman
