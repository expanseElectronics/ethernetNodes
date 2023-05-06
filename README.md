![ethernet](https://expanseelectronics.com/images/IMG_20211019_124949.jpg)

### **About:**
Dual-port ethernet Artnet nodes are somewhat expensive nowadays with simple [Pathport Duo's](https://stagedepot.co.uk/lighting/control-dimming/dmx-management/city-theatrical-pathport-c-series-node?sku=CT-P6202&gclid=CjwKCAjwuvmHBhAxEiwAWAYj-JbNi8wZCGz_KHbozcLFQ_qSdAPo95IHTKV8BdgGUl6DZUhPz_-OCBoC5V8QAvD_BwE) setting theatres back around £750. 
Based on our experience designing small-form-factor wireless DMX nodes, we took it upon ourselves to design a cheap and affordable wired ethernet node that would provide similar features to popular DMX nodes from established manufacturers such as ETC, City Theatrical and Chauvet DJ.  

*Our ethernet nodes reduces latency, provides USB-C programmability, allows for one DMX input and two RDM compatible outputs .*

You can purchase our nodes on our website or by clicking [here](https:/expanseelectronics.com/).

### Specifications:
Our ethernet node comes with two DMX ports. Port A can be configured as an input, or an output, with full support for Artnet V4 and up to 325packet/s. Port B is strictly an output but can be configured to support sACN.  

All ports are configured over the web interface, which is hosted at an automatically assigned IP address. (DCHP)

When first booting the device, it will attempt to configure DHCP, and if it is unable to, it will host a Static IP at 10.0.0.1, which can be connected to via peer-to-peer or via a switch. Static addresses are then available to be configured from the web interface. The node will be able to listen and output/input ArtNet on the peer-to-peer network. 

![webinterface](https://user-images.githubusercontent.com/63847434/127889196-3878f0a8-2d72-41e2-a7bf-808ad1c5d497.JPG)

*A screenshot of the default page on the web interface.*

Both ports on the device come with status LEDs which are pre-configured to be dim and can be turned off via a firmware upload. The status LEDs show if the port is configured as an input, output and whether it is receiving or transmitting data. 

### Enclousre:
We have partnered with [Lincoln Binns](https://lincolnbinns.com/) and are using their E-Case C range, which allows a wide range of mounting hardware and customization. 
Both ports are located on the outside of the case, along with a twinned A port for use as an input. There is also a USB-C connector for power and data transfer, including the ethernet port and reset button.

### Credits:
A lot of this project is forked from Matt's ESP8266 based WiFi ArtNet to DMX, RDM and LED Pixels project found [here](https://github.com/mtongnz/ESP8266_ArtNetNode_v2). We thank Matt as a team for creating his amazing project with all it's features. 
