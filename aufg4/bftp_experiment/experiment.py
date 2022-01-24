#!/usr/bin/python
import time
import os
import shutil
import subprocess
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.cli import CLI
import os
import numpy as np
import re
import matplotlib.pyplot as plt

# Check your Ethernet interface with ifconfig and insert the name here
ETH_INTERFACE = 'client-eth0'

SERVER_BIN = os.environ.get('SERVER_BIN')
CLIENT_BIN = os.environ.get('CLIENT_BIN')

TESTFILES_DIRECTORY = "testfiles/"

SRV_PORT = 6000
SRV_DIR = "storage/"
SRV_LOG = "logs/server.log"

CLT_RTO = 800
CLT_LOG = "logs/client.log"
TSK_OFILE = "logs/client.pcap"

PLOT_DIR = "logs/"

# cleanup files (from previous run)
if os.path.exists(SRV_DIR):
    shutil.rmtree(SRV_DIR)
if os.path.exists(SRV_LOG):
    os.remove(SRV_LOG)
if os.path.exists(CLT_LOG):
    os.remove(CLT_LOG)
if os.path.exists(TSK_OFILE):
    os.remove(TSK_OFILE)
if not os.path.exists(PLOT_DIR):
    os.mkdir(PLOT_DIR)

def plot_results(testfile: str, timeout_values: np.ndarray, retransmissions: list, duplicates: list, durations: list):
    assert len(timeout_values) == len(retransmissions) == len(duplicates) == len(durations)
    plt.figure(figsize=(10, 5))
    plt.suptitle('File transfer: ' + testfile)
    plt.style.use('seaborn')
    plt.subplot(1, 2, 1)
    plt.title('Retransmissions and duplicates')
    plt.plot(timeout_values, retransmissions, 'og-', label='Retransmissions')
    plt.plot(timeout_values, duplicates, 'or-', label='Duplicates')
    plt.xlabel('Retransmission timeout (RTO)')
    plt.xlim(min(timeout_values), max(timeout_values))
    plt.ylabel('Packets')
    plt.yticks(np.arange(0, max(retransmissions + duplicates)+1, 1))
    plt.legend()

    plt.subplot(1, 2, 2)
    plt.title('Duration of file transfer')
    plt.plot(timeout_values, durations, 'ob-', label='Duration of file transfer')
    plt.xlabel('Retransmission timeout (RTO)')
    plt.xlim(min(timeout_values), max(timeout_values))
    plt.ylabel('Time [ms]')
    plt.savefig(f'plots/{file}.png')


# steup and start mininet network
net = Mininet(link=TCLink)
server = net.addHost('server')
client = net.addHost('client')
s1 = net.addSwitch('s1')
c0 = net.addController('c0')

# net.addLink(server, s1, delay='4ms', jitter='1ms', loss=1)
# net.addLink(client, s1, delay='4ms', jitter='1ms', loss=1)
net.addLink(server, s1, delay='0ms', jitter='1ms', loss=1)
net.addLink(client, s1, delay='0ms', jitter='1ms', loss=1)
net.start()

print('Server IP:', server.IP())
print('Client IP:', client.IP())
server.cmd('chmod +x ' + SERVER_BIN)
client.cmd('chmod +x ' + CLIENT_BIN)

# prepare and run experiments
server.cmd(SERVER_BIN + ' -d ' + str(SRV_PORT) + ' ' + SRV_DIR + ' &> ' + SRV_LOG + ' &')
client.cmd(f'tshark -i {ETH_INTERFACE} -f udp -w {TSK_OFILE} &')

# wait for a bit to make sure that tshark captures the very first datagram
time.sleep(1)

timeout_values = np.arange(500, 2001, 500)
# sequentially upload test files
for file in os.listdir(TESTFILES_DIRECTORY):
    testfile = os.path.join(TESTFILES_DIRECTORY, file)
    srv_file = os.path.join(SRV_DIR, file)
    retransmissions = []
    duplicates = []
    durations = []

    for client_rto in timeout_values:
        # if os.path.exists(CLT_LOG):
        #     os.remove(CLT_LOG)
        print("Transfer " + testfile + " with RTO of " + str(client_rto) + " ms")
        client.cmd(f'{CLIENT_BIN} -d -r {str(client_rto)} {str(server.IP())} {str(SRV_PORT)} {testfile} &> {CLT_LOG}')
        time.sleep(1)

        print('%-30s' % testfile, end='')
        diffoutput = subprocess.getoutput('diff -N ' + testfile + ' ' + srv_file)
        if diffoutput != "":
            print(" => \033[31m\033[1m FAILED \033[0m")
        else:
            print(" => \033[32m\033[1m PASSED \033[0m")
        rtm = re.findall(r'\d+', client.cmd("awk '/retransmissions:/ {print $7}' ../bftp_experiment/logs/client.log"))[-1]
        dps = re.findall(r'\d+', server.cmd("awk '/duplicates:/ {print $7}' ../bftp_experiment/logs/server.log"))[-1]
        dur = re.findall(r'\d+', server.cmd("awk '/completed in/ {print $9}' ../bftp_experiment/logs/server.log"))[-1]
        print("Client retransmissions: " + rtm + ", Server duplicates: " + dps)
        print("Duration: " + dur)
        retransmissions.append(int(rtm))
        duplicates.append(int(dps))
        durations.append(int(dur))
        time.sleep(1)

    plot_results(testfile, timeout_values, retransmissions, duplicates, durations)
    time.sleep(2)

# wait for a bit to make sure that tshark records even the very last datagram
time.sleep(1)

# send SIGINT to stop server and tshark process
server.cmd('kill -s SIGINT %bft_server')
client.cmd('kill -s SIGINT %tshark')

# uncomment the following line to get an interactive shell
# CLI(net)

# stop mininet network
net.stop()
