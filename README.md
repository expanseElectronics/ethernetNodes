![ethernet](https://user-images.githubusercontent.com/63847434/127887093-1f3dc97f-a095-4233-8183-4786d8f3eddc.png)

### **About:**
Dual-port ethernet Artnet nodes are somewhat expensive nowadays with simple [Pathport Duo's](https://stagedepot.co.uk/lighting/control-dimming/dmx-management/city-theatrical-pathport-c-series-node?sku=CT-P6202&gclid=CjwKCAjwuvmHBhAxEiwAWAYj-JbNi8wZCGz_KHbozcLFQ_qSdAPo95IHTKV8BdgGUl6DZUhPz_-OCBoC5V8QAvD_BwE) setting theatres back around £750. 
Based on our experience designing small-form-factor wireless DMX nodes, we took it upon ourselves to design a cheap and affordable wired ethernet node that would provide similar features to popular DMX nodes from established manufacturers such as ETC, City Theatrical and Chauvet DJ.  

### **Some demos:**
Before selling on Tindie, I have used our DMX nodes in many productions. Most recently, on a Cambridge Theatre Company production (Our House, The Madness Musical) in my local school's theatre [(Leys Great Hall)](https://www.theleys.net/591/venue-hire/great-hall) where we used one of our nodes to control our Cyc backdrop over the stage. It took us less than two minutes to plug the node into our ethernet switch and connect to our ETC Nomad console. 

![Photograph of 'Chicago' by CTC, performed at Leys Great Hall.](https://www.lightsoundjournal.com/wp-content/uploads/2020/01/Robe-Chicago-DSC_5091-photo-by-Eliza-Wilmot-e1578990456228.jpg "Photograph of 'Chicago' by CTC, performed at Leys Great Hall.")

*Photograph of 'Chicago' by CTC, performed at Leys Great Hall.*

*Our ethernet nodes reduces latency, provides USB-C programmability, allows for one DMX input and two RDM compatible outputs .*

### Specifications:
Our ethernet node comes with two DMX ports. Port A can be configured as an input, or an output, with full support for Artnet V4 and up to 325packet/s. Port B is strictly an output but can be configured to support sACN.  

All ports are configured over the web interface, which is hosted at an automatically assigned IP address. (DCHP)

When first booting the device, it will attempt to configure DHCP, and if it is unable to, it will host a Static IP at 10.0.0.1, which can be connected to via peer-to-peer or via a switch. Static addresses are then available to be configured from the web interface. The node will be able to listen and output/input ArtNet on the peer-to-peer network. 

![webinterface](https://user-images.githubusercontent.com/63847434/127889196-3878f0a8-2d72-41e2-a7bf-808ad1c5d497.JPG)

*A screenshot of the default page on the web interface.*

Both ports on the device come with status LEDs which are pre-configured to be dim and can be turned off via a firmware upload. The status LEDs show if the port is configured as an input, output and whether it is receiving or transmitting data. 

### Enclousre:
We have partnered with [Lincoln Binns](https://lincolnbinns.com/) and are using their E-Case C range, which allows a wide range of mounting hardware and customization. 

![enter image description here](https://lincolnbinns.com/pub/media/catalog/product/cache/de11c78541e91402d501d037956c13c3/e/-/e-case_c_enclosure_kit_pic1.png)

*An image of Lincoln Binns's E-Case.*

Both ports are located on the outside of the case, along with a twinned A port for use as an input. There is also a USB-C connector for power and data transfer, including the ethernet port and reset button.
