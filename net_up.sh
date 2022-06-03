brctl addbr br0
ip addr flush dev enp0s31f6
brctl addif br0 enp0s31f6
tunctl -t tap0 -u `whoami`
brctl addif br0 tap0
ifconfig enp0s31f6 up
ifconfig tap0 up
ifconfig br0 up
dhclient -v br0
