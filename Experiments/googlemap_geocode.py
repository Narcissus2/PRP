##############################################################
# 2018/10/03  Wayne Wu
# 這個程式讀取file_name然後把所有位置名稱送去googlemaps去求經緯度
# 寫在out_file中
##############################################################

import json,time,simplejson
import geocoder
import requests


# GOOGLE_BASE_URL = 'https://maps.googleapis.com/maps/api/js?'
API_key = 'AIzaSyD7w8r1zaa7hXFr2TApNOiRHpWXhfwgWMc' #正式申請
API_key2 = 'AIzaSyBtZnL3HGl3qgEKdoC2gYFMU387ocl7tW8' #測試用
delay = 2
url = 'https://maps.googleapis.com/maps/api/geocode/json'
file_name = 'place_new.txt' #全部地名在這裡
out_file = 'place_xy.txt' #新的有座標的存在這


try:
    ofile = open(file_name,'r')
except IOError:
    print (file_name + ' is not accessible.')
    exit()

try:
    placefile = open(out_file,'w')
except IOError:
    print (out_file + ' is not accessible.')
    exit()

place = ofile.readline()
num = 0 #計數
limit = 1 #測試才需要 限制次數
while place :
	# if(num > limit) :
	# 	break;
	place = place.strip('\n')
	params = {'address':place, 'key':API_key}

	r = requests.get(url,params=params)
	# print(r) # <Response [200]>
	results = r.json()['results']

	print(place + ':')
	# print(results)
	if results != []:
	    location = results[0]['geometry']['location']
	    # location['lat'],location['lng']
	    print(location['lat'],location['lng'])
	    placefile.write(place + ' ' + str(location['lat']) + ' ' + str(location['lng']) + '\n')
	else :
	    print('no results')
	
	num += 1
	print('num = ' + str(num))
	place = ofile.readline()

placefile.close()
ofile.close()

