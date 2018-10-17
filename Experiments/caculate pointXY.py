# 2018/10/11 Wu 
# 讀取explist.txt找到所有data後
# 找出裡面的全部地名
# 用圓來算出兩個交點，定位每個座標點
# 然後寫到place_XXX.txt裡面
###################################################

import matplotlib.pyplot as plt
import numpy as np
import os
import time
import math

def sq(x):
    return float(x * x)


 #caculate insection --------------------------------------------------
def caculate_insection(x,y,R,a,b,S):

    # target point on table
    tx = float(10)
    ty = float(10)

    # hight of table
    h0 = float(5)

    # length of arm
    R = float(R)
    S = float(S)

    # arm point
    # 圓心點(x,y)
    x = float(x)
    y = float(y)

    # target point for arm
    # a = float(math.sqrt(sq(tx)+ sq(ty))) # a = (tx,ty)和(0,0)的長度
    # b = h0
    # 圓心點(a,b)
    a = float(a)
    b = float(b)

    # print ("arm target:", a, b)

    d = math.sqrt(sq(math.fabs(a-x)) + sq(math.fabs(b-y))) # d = (a,b)和(x,y)的距離
    # print ("desitens:", d) # 兩個圓心的距離

    if d > (R+S) or d < (math.fabs(R-S)):
        print ("This point can't be rached!")
        if x>a :
        	x3 = x + (x-a)*R/(R+S)
        	if y > b:
        		y3 = y + (y-b)*R/(R+S)
        	else:
        		y3 = b + (b-y)*S/(S+R)
        else:
        	x3 = a + (a-x)*S/(R+S)
        	if y > b:
        		y3 = y + (y-b)*R/(R+S)
        	else:
        		y3 = b + (b-y)*S/(S+R)
        point = [x3,y3,x3,y3]
        return point
        # return -1
        exit
        
    if d == 0 and R==S :
        print ("Can't rach arm point!")
        return -2
        exit

    A = (sq(R) - sq(S) + sq(d)) / (2 * d)
    h = math.sqrt(sq(R) - sq(A))

    x2 = x + A * (a-x)/d
    y2 = y + A * (b-y)/d

    #print x2, y2
    x3 = x2 - h * ( b - y ) / d
    y3 = y2 + h * ( a - x ) / d

    x4 = x2 + h * (b - y) / d
    y4 = y2 - h * (a - x) / d

    # print ("arm middle point:")
    # print (x3, y3)
    # print (x4, y4)
    point = [x3,y3,x4,y4] # 回傳的list 為(x3,x4)和(x4,y4)兩個交點
    return point

# --------------------------------------------------

# 去抓explist 找出所有data的檔案名稱

explist = 'explist.txt'
try:
    fexplist = open(explist,'r')
except IOError:
    print ( explist + ' is not accessible.')
    exit()

exp = fexplist.readline()

# 開始讀取每個data

while exp :
	exp = exp.strip('\n')
	try:
	    fexp = open(exp,'r')
	except IOError:
	    print ( exp + ' is not accessible.')
	    exit()

	place_exp = 'place_' + exp
	try:
	    fplace = open(place_exp,'w')
	except IOError:
	    print ( place_exp + ' is not accessible.')
	    exit()

	point_num = fexp.readline()
	point_num = int(point_num) + 1

	dummy_line_num = 5 #有5行才到矩陣的地方

	while dummy_line_num :
		line = fexp.readline() 
		dummy_line_num -= 1

	distance = []
	line = fexp.readline()
	for i in range(point_num):
		line = line.split()
		distance.append(line)
		line = fexp.readline()
		# print (line) 
		# time.sleep(10)

	# print(distance)

	# print ("Distance board")
	# for i in range(len(distance)):
	# 	print(distance[i])
	# print('\n')

#  距離矩陣製造完成 -----------------------------
	# 距離矩陣是不對稱的，我只用右上三角
	# a找b的距離就找board[a][b]
	# 依此類推

	# 去抓namelist -----------------------------
	namelist = []
	name = fexp.readline() #多一行
	while name :
		if name == '\n':
			name = fexp.readline()
			continue
		name = name.split()
		namelist.append(name[1])
		name = fexp.readline()
	# print('name list :')
	# print(namelist)

#  開始計算每個點的位置
	
	# 先初始化3個點，再用這三個點去找其他全部點
	pointXY = []
	pointXY.append([namelist[0],0,0])
	pointXY.append([namelist[1],distance[0][1],0])
	temp_point = caculate_insection(pointXY[0][1],pointXY[0][2],distance[0][2],pointXY[1][1],pointXY[1][2],distance[1][2])
	# print (temp_point)
	pointXY.append([namelist[2],temp_point[0],temp_point[1]]) #直接取 [0] [1]來當座標
	point_index = 3
	while point_index < len(namelist):
		temp_point = caculate_insection(pointXY[0][1],pointXY[0][2],distance[0][point_index],pointXY[1][1],pointXY[1][2],distance[1][point_index])
		temp_point2 = caculate_insection(pointXY[0][1],pointXY[0][2],distance[0][point_index],pointXY[2][1],pointXY[2][2],distance[2][point_index])
		correct_index = 0 
		min_length = math.sqrt(sq(temp_point[0]-temp_point2[0])+sq(temp_point[1]-temp_point2[1]))
		length = math.sqrt(sq(temp_point[0]-temp_point2[2])+sq(temp_point[1]-temp_point2[3]))
		# print ( 'length = ' + str(length))
		if min_length > length:
			correct_index = 0
			min_length = length
		for i in range(2):
			length = math.sqrt(sq(temp_point[2]-temp_point2[i*2])+sq(temp_point[3]-temp_point2[i*2+1]))
			# print ('len = '+ str(length))
			if min_length > length:
				correct_index = 1
				min_length = length
		pointXY.append([namelist[point_index],temp_point[correct_index],temp_point[correct_index+1]])

		# print('比較')
		# print (temp_point)
		# print (temp_point2)
		# time.sleep(50)
		point_index+=1
	# pointXY.append([namelist[2],distance[0][2],])
	for i in range(len(pointXY)):
		# print(pointXY[i])
		fplace.write(str(pointXY[i][0]) + ' ' + str(pointXY[i][1]) + ' ' + str(pointXY[i][2]) + '\n')
	fplace.close()
	# print(pointXY)

	# time.sleep(0.5)
	exp = fexplist.readline()

