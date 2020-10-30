import serial
import csv
import time
import csv
import os
import sys
import signal

with serial.Serial('/dev/ttyUSB0', 9600, timeout=1) as ser:
        
    while True:
        t = time.localtime()
        current_time = time.strftime("%Y_%m_%d_%H_%M_%S", t)
        path = os.getcwd()+"/Data/SPIC/"+current_time+"_S9_C147"
		#path = os.path.abspath("/home/felin/Downloads/Recorder_S10_C156/Data/SPIC") +"/"+ current_time+"_S10_C156"
        os.mkdir(path)

        with open(path+"/"+current_time+"_SPIC_Datas_S9_C147.csv", 'w') as csvfile:
            
            t0 = time.localtime()
            Flag=True

            while Flag == True:

				try:
					t1 = time.localtime()
					if (t1.tm_mday == t0.tm_mday+1) & (t1.tm_hour >= 0) & (t1.tm_min >= 0) & (t1.tm_sec >= 0):
						Flag=False
						open(path+ "/ready.txt",'w')
					
					line = ser.read() #lit 1 octet
					
					if line == '\x7E':
						ser.read(16) #lit 16 octets
						mode = ser.read() # lit le 18 eme octet
						if mode == '\x43':
							ser.read(7) #lit 7 octets
							id = ser.read(10) #lit 10 octets
							ser.read()#lit 1 octets
							byte_weight_kg = ser.read(4)
							EOL = ser.read()
							if EOL!='\x3B':
								byte_weight_kg = byte_weight_kg + EOL
							weight_kg = float(byte_weight_kg)/100
							weight_str = str(weight_kg)
							t = time.localtime()
							current_time = time.strftime("%Y_%m_%d_%H_%M_%S", t)
							line_to_write = current_time + ',' + id + ',' + weight_str + '\n'
							csvfile.write(line_to_write)
							print("{}  : id : {}, weight : {}".format(current_time,id,weight_kg))
							csvfile.flush()
						else:
							continue	
				except KeyboardInterrupt:
					print ('Program Interrupted\n')
					csvfile.close()
					open(path+ "/ready.txt",'w')
					sys.exit(0)
	
				except:
					continue