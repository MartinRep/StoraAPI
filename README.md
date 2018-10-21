# Netgear Stora API
Linux version 2.6.22.18-Netgear running on Stora NAS.
Provides access to motherboard API. Includes realtime CPU temperature readout, case FAN speed control System sleep and System alarm wake up.

## Access to the device

In order to access your Netgear Stora NAS device you need:
 - Serial number of device. Located on the bottom. Format XXXX-XXXX-XXXX-XXXX
 - IP address. Example: 192.168.0.100
 - username and password of any user account on Stora device.

To ssh to Stora you need to format the user accordingly:
ssh USERNAME_hipserv2_netgear_SERIAL_NUMBER@IP_ADDRESS

### Example: 
username: superUser  
serial number: NGDF-VGRE-XDCF-UTRF  
IP address: 192.168.0.100  

`ssh superuser_hipserv2_netgear_NGDF-VGRE-XDCF-UTRF@192.168.0.100`

This will bring you to bash shell.

## Compile and run board_API test
`gcc board_api.c -o cputemp`
`/.cputemp`

## Test Demo

CPU temp and FAN speed SLOWDOWN test example output
![Temp and FAN test demo](https://github.com/MartinRep/StoraAPI/blob/master/Resources/InitTestScreenShot.PNG)
