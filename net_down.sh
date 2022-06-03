brctl delif br0 tap0
tunctl -d tap0
brctl delif br0 enp0s31f6
ifconfig br0 down
brctl delbr br0
ifconfig enp0s31f6 up
dhclient -v enp0s31f6
