#!/usr/bin/python
import time
import os
import shutil
import subprocess
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.cli import CLI

TESTFILES_DIRECTORY = "testfiles/"

SRV_PORT = 6000
SRV_DIR = "storage/"
SRV_LOG = "logs/server.log"

CLT_LOG = "logs/client.log"
CLT_RTO = 2000
TSK_OFILE = "logs/client.pcap"

# cleanup files (from previous run)
if os.path.exists(SRV_DIR):
    shutil.rmtree(SRV_DIR)
if os.path.exists(SRV_LOG):
    os.remove(SRV_LOG)
if os.path.exists(CLT_LOG):
    os.remove(CLT_LOG)
if os.path.exists(TSK_OFILE):
    os.remove(TSK_OFILE)

# steup and start mininet network
net = Mininet(link=TCLink)
server = net.addHost('server')
client = net.addHost('client')
s1 = net.addSwitch('s1')
c0 = net.addController('c0')

net.addLink(server, s1, delay='4ms', jitter='1ms', loss=1)
net.addLink(client, s1, delay='4ms', jitter='1ms', loss=1)
net.start()

server.cmd('chmod +x ../cmake-build-debug/bft_server/bft_server')
client.cmd('chmod +x ../cmake-build-debug/bft_client/bft_client')

# prepare and run experiments
server.cmd('../cmake-build-debug/bft_server/bft_server ' + str(SRV_PORT) + ' ' + SRV_DIR + ' > ' + SRV_LOG + ' &')
client.cmd('../cmake-build-debug/bft_client/bft_client ' + str(SRV_PORT) + ' ' + SRV_DIR + ' > ' + SRV_LOG + ' &')

client.cmd('tshark -i enp0s3 -f udp -w ' + TSK_OFILE + ' &')

# wait for a bit to make sure that tshark captures the very first datagram
time.sleep(1)

# sequentially upload test files
for file in os.listdir(TESTFILES_DIRECTORY):
    testfile = os.path.join(TESTFILES_DIRECTORY, file)
    srv_file = os.path.join(SRV_DIR, file)
    print('%-30s' % testfile, end='')

    client.cmd('../cmake-build-debug/bft_client/bft_client -r ' + str(CLT_RTO) + ' ', server.IP(), ' '
               + str(SRV_PORT) + ' ' + testfile + ' >> ' + CLT_LOG)

    diffoutput = subprocess.getoutput('diff -N ' + testfile + ' ' + srv_file)
    if diffoutput != "":
        print(" => \033[31m\033[1m FAILED \033[0m")
    else:
        print(" => \033[32m\033[1m PASSED \033[0m")

# wait for a bit to make sure that tshark records even the very last datagram
time.sleep(1)

# send SIGINT to stop server and tshark process
server.cmd('kill -s SIGINT %bft_server')
client.cmd('kill -s SIGINT %tshark')

# uncomment the following line to get an interactive shell
# CLI(net)

# stop mininet network
net.stop()
