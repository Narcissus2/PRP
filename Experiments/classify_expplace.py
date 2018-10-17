
import time

exp_name = 'explist.txt'
allplace_data = 'place_xy.txt'


# open explist
try:
    flist = open(exp_name,'r')
    print(exp_name + ' is loading.')
except IOError:
    print (exp_name + ' is not accessible.')
    exit()

# open the exp data
exp = flist.readline()

while exp :

	# open exp 
	exp = exp.strip('\n')
	try:
	    fexp = open(exp,'r')
	except IOError:
	    print (exp + ' is not accessible.')
	    exit()

	exp_place_name = 'place_' + exp
	try:
	    indi_exp_place = open(exp_place_name,'w')
	except IOError:
	    print (exp_place_name + ' is not accessible.')
	    exit()

	line = fexp.readline()

	while line :
		if line == '\n':
			line = fexp.readline()
			continue

		line = line.split()

		# find the place name, I want to search
		if (len(line) > 2):
			# check line[1] is alpha ?
			if not line[1].isdigit():
				place_want_to_find = line[1]
				# print(line[1])
				# time.sleep(2)
				try:
				    fplace = open(allplace_data,'r')
				except IOError:
				    print (allplace_data + ' is not accessible.')
				    exit()

				place = fplace.readline()
				find = False
				while place : 
					place_splt = place.split()
					# place = place.strip('\n')
					if place_splt[0] == place_want_to_find:
						indi_exp_place.write(place)
						# print(place)
						find = True
						# time.sleep(0.5)
						break
					place = fplace.readline()
				if find == False :
					print (place_want_to_find + ' not found')
				fplace.close()
		line = fexp.readline()
	print(exp_place_name + ' is finish')
	exp = flist.readline()
	indi_exp_place.close()


flist.close()