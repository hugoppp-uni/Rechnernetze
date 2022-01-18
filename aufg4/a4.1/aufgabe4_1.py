from mininet.net import Mininet

from mininet.node import Controller, RemoteController, OVSKernelSwitch, UserSwitch

from mininet.cli import CLI

from mininet.log import setLogLevel

from mininet.link import Link, TCLink

 

def topology():

        net = Mininet( controller=RemoteController, link=TCLink, switch=OVSKernelSwitch )

 

        # Add hosts and switches

        h1 = net.addHost( 'h1', ip="10.0.1.2/24", mac="00:00:00:00:00:01" )

        h2 = net.addHost( 'h2', ip="10.1.0.2/16", mac="00:00:00:00:00:02" )

        r1 = net.addHost( 'r1')


       # c0 = net.addController( 'c0', controller=RemoteController, ip='127.0.0.1', port=6633 )

 

        net.addLink( r1, h1 )

        net.addLink( r1, h2 )

        net.build()

       # c0.start()


        r1.cmd("ifconfig r1-eth0 0")

        r1.cmd("ifconfig r1-eth1 0")

        r1.cmd("ifconfig r1-eth0 hw ether 00:00:00:00:01:01")

        r1.cmd("ifconfig r1-eth1 hw ether 00:00:00:00:01:02")

        r1.cmd("ip addr add 10.0.1.1/24 brd + dev r1-eth0")

        r1.cmd("ip addr add 10.1.0.1/16 brd + dev r1-eth1")

        r1.cmd("echo 1 > /proc/sys/net/ipv4/ip_forward")

        h1.cmd("ip route add default via 10.0.1.1")

        h2.cmd("ip route add default via 10.1.0.1")

        #print "*** Running CLI"

        CLI( net )

 

       # print "*** Stopping network"

        net.stop()

      

if __name__ == '__main__':

    setLogLevel( 'info' )

    topology()