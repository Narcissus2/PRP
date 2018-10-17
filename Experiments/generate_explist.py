# 2018/10/1  Wayne Wu
# function generate_explist()
# 讀取list_name的檔案後將有suffix的句子寫入explist_name檔案中
# 
################################################################
import time

def generate_explist():
	# open the list.txt 
	list_name = 'list.txt'

	try:
	    fexp = open(list_name,'r')
	except IOError:
	    print (list_name + ' is not accessible.')
	    exit()

	# read the list.txt and write the explist.txt
	explist_name = 'explist.txt'

	try:
		fout = open(explist_name,'w')
	except IOError:
		print (explist_name + ' is not accessible.')
		exit()

	suffix = 'txt'
	filename = fexp.readline()
	while filename:
		# print(filename)
		if filename.find(suffix)>=0:
			# print(filename.find(suffix))
			# print (filename)
			fout.write(filename)
		filename = fexp.readline()

	fexp.close()
	fout.close()
#----------------------------------------------------------------
def list_all_place():
	#read explist_name
	explist_name = 'explist.txt'

	try:
	    fexp = open(explist_name,'r')
	    print(explist_name + ' is loading.')
	except IOError:
	    print (explist_name + ' is not accessible.')
	    exit()

	# ready to write place file
	place_file_name = 'place.txt'
	try:
	    fplace = open(place_file_name,'w')
	    print(place_file_name + ' is loading.')
	except IOError:
	    print (place_file_name + ' is not accessible.')
	    exit()

	#read file
	num = 0
	file_name = fexp.readline()
	# print('filename = ' + file_name)
	while file_name :
		#open file
		file_name = file_name.strip('\n')
		try:
		    ofile = open(file_name,'r')
		except IOError:
		    print (file_name + ' is not accessible.')
		    exit()

		# process the file -- read the file
		line = ofile.readline()
		while line :
			# cut the sequence
			# not '\n'
			if line == '\n':
				line = ofile.readline()
				continue
			# split : cut line be many content 
			line = line.split()
			# print('line = ' + line[0])
			# print('len = ' + str(len(line)))
			# if len(line) > 2 , I can check the line[1]
			if (len(line) > 2):
				# check line[1] is alpha ?
				if not line[1].isdigit():
					# print(line[1])
					fplace.write(line[1] + '\n')
					num += 1
				
			line = ofile.readline()
			# time.sleep(x) : wait x second(s)
			# time.sleep(1)
		#close file
		ofile.close()

		file_name = fexp.readline()
	print ('before sort :')
	print (str(num) + ' points')
	fexp.close()
	fplace.close()

#----------------------------------------------------------------
def delete_the_duplicate():
	place_file_name = 'place.txt'
	try:
	    ofile = open(place_file_name,'r')
	except IOError:
	    print (place_file_name + ' is not accessible.')
	    exit()

	place_new_name = 'place_new.txt'
	try:
	    newfile = open(place_new_name,'w')
	except IOError:
	    print (place_new_name + ' is not accessible.')
	    exit()

	original_list = []
	new_list = []
	line = ofile.readline()
	while line:
		original_list.append(line)
		line = ofile.readline()

	# set 就不會有重複了
	new_list = list(set(original_list))
	for i in range(len(new_list)) :
		newfile.write(new_list[i])
	print('after sort :')
	print(str(len(new_list)) + ' points')
	ofile.close()
	newfile.close()

#----------------------------------------------------------------
print('----------------generate_explist ---------------')
generate_explist()
print('-------------- list_all_place --------------- ')
list_all_place()
print('-------------- delete the duplicate -----------')
delete_the_duplicate()