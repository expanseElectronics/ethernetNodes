### Ethernet Node Specifications
Our ethernet node comes with two DMX ports. Port A can be configured as an input, or an output, with full support for Artnet V4 and up to 325packet/s. Port B is strictly an output, but can be configured to support sACN output.  

All port are configured over the web interface, which is hosted at an automatically assigned IP address. (DCHP)
We are currently unable to implement static IP addressing due to memory limitations on our microcontroller. 

![Web Interface](https://expanseelectronics.com/files/images/webinterface.JPG)
*A screenshot of the default page on the web inteface.*

Both ports on the device come with status LEDs which are pre-configured to be dim, and can be turned off via a firmware upload. The status LEDs show if the port is configured as an input, output, and wether it is receiving or transmitting data. 

### Enclousre
For our ethernet node, we have partnered with [Lincoln Binns](https://lincolnbinns.com/) and are using their E-Case C range, which allows a wide range of mounting hardware, and customization. 
![enter image description here](https://lincolnbinns.com/pub/media/catalog/product/cache/de11c78541e91402d501d037956c13c3/e/-/e-case_c_enclosure_kit_pic1.png)

On the outside of the case, are where both ports are located, along with a twinned A port for use as an input. There is also a USB-C connector for power and data transfer, along with the ethernet port and reset button. 
