# HP fan proxy
The HPE iLO system tends to over-drive the fans in their servers. Especially when you have any card plugged in PCIe sockets. This project use an arduino to lower the PWM signal.

### Hardware notices
The project is built upon an arduino nano with bunch of wires.

HP have a special socket for their fans. If you want to buy a wire on Taobao or eBay, you can search for 2x3 PH2.0 connector. And you need to manually cut the "ear" on the male connector. If you cannot find any female connector, you can buy some sockets and plug them on a male connector.

![connector-cut](https://raw.githubusercontent.com/Max-Sum/HP-fan-proxy/master/images/connector-cut.jpg)![connector-male](https://raw.githubusercontent.com/Max-Sum/HP-fan-proxy/master/images/connector-male.jpg)![connector-female](https://raw.githubusercontent.com/Max-Sum/HP-fan-proxy/master/images/connector-female.jpg)

Up: Cutting the ear  /  Middle: Finished male connector  /  Down: "Femal connector"

They even have a different connecting schemes on the motherboard and on the fan socket. I found some discussion on the internet showing the connector definition as follows. But my server (DL380e Gen8) have the PWM on Pin 2 on motherboard while PWM on Pin 3 on fan socket. Anyway you just need to find which one connects to the blue wire.

![post-8123-0-98784900-1394609613](/Users/Max/Projects/repos/HP-fan-proxy/images/post-8123-0-98784900-1394609613.png)

### Software
The original solution by [executivul](https://www.reddit.com/user/executivul) uses a low pass filter so that the PWM could be read by analog pins in Arduino. But I don't have any capacitors. Instead, I directly connects the PWM inputs to the analog pins and read them for many times then take the average. This turns out to work.

### Finish

![server](https://raw.githubusercontent.com/Max-Sum/HP-fan-proxy/master/images/server.jpg)

![wiring](https://raw.githubusercontent.com/Max-Sum/HP-fan-proxy/master/images/wiring.jpg)

![arduino](https://raw.githubusercontent.com/Max-Sum/HP-fan-proxy/master/images/arduino.jpg)

### Credit
This project is originated from multiple threads in homelab reddit.
https://www.reddit.com/r/homelab/comments/7vxo5n/hp_dl380e_g8_arduino_fan_control_project/
https://www.reddit.com/r/homelab/comments/6xfscd/making_the_dl380_g7_quiet_for_good/
https://www.reddit.com/r/homelab/comments/72k3jf/faking_the_fan_signal_on_a_dl380_g7/

[executivul](https://www.reddit.com/user/executivul) created the code for arduino.
