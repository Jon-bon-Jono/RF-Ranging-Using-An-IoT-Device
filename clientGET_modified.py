#!/usr/bin/env python3

#by Jonathan Williams
#User promted to Enter distance in cm
#Sends 10 requests to CoAP server
#Receives 10 rssi values
#Uses these to compute the p model (gets attenuation rate and system loss)
#When the user doesn't enter a number we advance to the next feature, where we attempt to estimate the distance between the sensor tags using on rssi request

import logging
import asyncio
import time
import sys
import numpy as np
from math import log1p, exp
from aiocoap import *

logging.basicConfig(level=logging.INFO)
rssis = []
reading_num = 10

@asyncio.coroutine
def main():
	x=1
	reading_interval = 1 #The time interval between each GET request in second

	while x<=reading_num:
		protocol = yield from Context.create_client_context()

		request = Message(code=GET)
		request.set_request_uri('coap://[aaaa::212:4b00:f0d:5284]:5683/net/parent/RSSI')
		try:
			response = yield from protocol.request(request).response
		except Exception as e:
			print('Failed to fetch resource:')
			print(e)
		else:
			print('Result: %s\n%r'%(response.code, response.payload))
			#Get the RSSI value from the payload
			print(response.payload.decode('ascii'))
			rssis.append(response.payload.decode('ascii'))
		x=x+1
		#put into sleep
		time.sleep(reading_interval)

if __name__ == "__main__":
	while 1:   
		#sample for given distance		
		print("Enter distance (cm)")
		d = input()
		try:
			d = float(d)
		except ValueError:
			break
		#collect 10 samples		
		asyncio.get_event_loop().run_until_complete(main())
		#compute propagation model		
		x = np.array([log1p(d)]*reading_num)
		rssis = [float(i) for i in rssis]		
		y = np.array(rssis)
		A = np.vstack([x, np.ones(len(x))]).T
		a, c = np.linalg.lstsq(A,y)[0]
		print("At {}cm we have: \n>>> a: {} (attenuation rate) \n>>> C: {} (system losses))\n".format(d,a,c))
		#print("Pathlos = {}".format(a*log1p(d)+c))
		rssis = []
	print("Move the sensortag and I will estimate the distance")
	reading_num=1
	while 1:
		#collect 1 sample
		asyncio.get_event_loop().run_until_complete(main())		
		#solve prop model equation to get d	
		print(rssis[0])
		print("a: {}, c: {}".format(a,c))
		d = exp((float(rssis[0]) - c)/a)
		print("Estimated distance: {}".format(d))
		print("Continue?")
		input()
		rssis = []
		
