# 去抓原本檔案的matrix 
# 然後再抓新座標的matrix
# 然後開始比對，印出結果~ 
# 每筆測資都會有比對的matrix 每一格為舊的(matrix distance - 新的distance)/distance
# 是一個差距的百分比
# 然後會有max_gap(正差距) 和 min_gap(負差距) 目的是找到最大的差距
#########################################

import math
import time

def sq(x):
    return float(x * x)


# 去抓explist 找出所有data的檔案名稱

explist = 'explist.txt'
compare_file = 'compare.txt'
try:
    fexplist = open(explist,'r')
except IOError:
    print ( explist + ' is not accessible.')
    exit()

try:
    fcom = open(compare_file,'w')
except IOError:
    print ( compare_file + ' is not accessible.')
    exit()

exp = fexplist.readline()

# 開啟個別原始檔案
while exp :
	exp = exp.strip('\n')
	fcom.write(exp + '\n')
	try:
	    fexp = open(exp,'r')
	except IOError:
	    print ( exp + ' is not accessible.')
	    exit()

	# 跳過全部不需要的資訊
	dimension = fexp.readline()
	dimension = int(dimension) + 1 # 原本不包含倉庫所以要多+1
	dummy_line = 5
	while dummy_line:
		line = fexp.readline()
		dummy_line -= 1

	ori_dis = [] # 原本的distance matrix

	for i in range(dimension):
		line = fexp.readline()
		line = line.split()
		ori_dis.append(line)

	# print (ori_dis)
	

	# 開啟新座標檔案
	try:
	    fnewxy = open('place_' + exp,'r')
	except IOError:
	    print ( 'place_' + exp + ' is not accessible.')
	    exit()

	line = fnewxy.readline()
	place_data = [] # 裡面有地圖的 [名稱.x.y]
	while line : 
		line = line.split()
		place_data.append(line)
		line = fnewxy.readline()

	# print ('place data')
	# print (place_data)

	# 開始計算各點之間的距離
	new_dis_matrix = []

	for i in range(dimension):
		one_row = []
		for j in range(dimension):
			dis = math.sqrt(sq(float(place_data[i][1])-float(place_data[j][1]))+sq(float(place_data[i][2])-float(place_data[j][2])))
			one_row.append(dis)
		new_dis_matrix.append(one_row)

	# print ("new dis matrix")
	# print (new_dis_matrix)
	max_gap = float(0)
	min_gap = float(1e12)
	# 算完距離矩陣 來跟原本的矩陣比較
	result_matrix = []
	for i in range(dimension):
		one_row = []
		for j in range(dimension):
			if ori_dis[i][j] == '0':
				# print ('0.00.0.0.00')
				# time.sleep(10)
				gap = 0
			else:
				gap = (float(ori_dis[i][j]) - float(new_dis_matrix[i][j]))/float(ori_dis[i][j])
				# gap = (float(ori_dis[i][j]) - float(new_dis_matrix[i][j]))
			one_row.append(gap)
		result_matrix.append(one_row)

	# print ("result :")
	# print (result_matrix)
	for i in range (dimension):
		for j in range(dimension) :
			# print (str(max_gap) + '>' + str(result_matrix[i][j]) + '?')
			# print (str(min_gap) + '<' + str(result_matrix[i][j]) + '?')
			# time.sleep(1)
			if result_matrix[i][j] > max_gap:
				max_gap = result_matrix[i][j]
			if result_matrix[i][j] < min_gap:
				min_gap = result_matrix[i][j]
			fcom.write(str('%.3f'%result_matrix[i][j])+' ')
		fcom.write('\n')	

	fcom.write('max_gap = '+ str(max_gap)+'\n')
	fcom.write('min_gap = '+ str(min_gap)+'\n')

	# time.sleep(5)

	fnewxy.close()

	exp = fexplist.readline()

fexplist.close()
fcom.close()
