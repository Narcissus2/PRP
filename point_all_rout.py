import matplotlib.pyplot as plt
import numpy as np
import os

# ----------------------------------------------------------------
# 讀取explist.ini檔案去找測試結果和測式資料
# 找出檔案內的最短路線畫出來放到Result的資料夾目錄下
# 顯示出最低distance 最低emission 還有平均distance 和 平均 emission
# 最後把最佳結果記錄到allresult.txt裡面
# ----------------------------------------------------------------

#找實驗檔
try:
    fexp = open('explist.ini','r')
except IOError:
    print ('explist.ini is not accessible.')
    exit()

explist = []
line = fexp.readline()
while line:
    line = line.strip('\n')
    explist.append(line)
    line = fexp.readline()

for all_exp in range(len(explist)):
    exp_path = 'Experiments//'+str(explist[all_exp])
#找測資檔名------------------------------------------
    fini = open(exp_path,'r')
    want_line = 7
    data_name = []
    for i in range(want_line):
        data_name = fini.readline()
    data_name = data_name.split()
    data_path = 'Experiments//' + data_name[2] + '.dat'
    print ("data path = ",data_path)

    fini.close()
#讀完ini檔-----------------------------------------

#open file 'r'唯讀模式
#try 檢查檔案是否存在
    try :
        f = open(data_path,'r')
    except IOError:
        print ('File is not accessible.')
        exit()
    
#從.dat 抓出要用的資料
#先抓dimension 數量--------------------------------
    temp = []
    want_line = 4 
    for i in range(want_line):
        temp = f.readline()
    temp = temp.split()
    dimension = int(temp[2])
    print ("dimension = ",dimension)
#抓完dimension 大小-----------------------------

#處理冗資訊 取得capacity----------------------
    want_line = 2
    temp = []
    for i in range(want_line):
        temp = f.readline()
    temp = temp.split()
    capacity = float(temp[2])
    print("capacity = ",capacity)
    temp = f.readline()
    temp = temp.split()
    vehicle_num = int(temp[2])
    print('vehicle num = ',vehicle_num)
    want_line = 1
#---------------------------------------------
    temp = f.readline()

#抓point x y ----------------------------------
    point_list = []
    for i in range(dimension):
        line = f.readline()
        line = line.split()
        #print(line)
        point_list.append(line)
#--------------------------------------------

#抓demand-------------------------------------------
    line = f.readline()
    demand = []
    for i in range(dimension):
        line = f.readline()
        line = line.split()
        #print(line)
        demand.append(float(line[1]))

    line = f.readline()
    line = f.readline()
    depot = int(line)
    print ("depot = ",depot)
    f.close()
#----------------------------------------------------------

#----------畫氣泡圖---------------------
#把每個點畫出來
    i = 0

    while i < len(point_list):
        j=0
        while j<3:
            point_list[i][j] = float(point_list[i][j])
            j+=1
        #print(point_list[i][1]," ",point_list[i][2])
        #plt.plot(point_list[i][1],point_list[i][2],'o')
        #plot只能畫點和線
        plt.scatter(point_list[i][1],point_list[i][2],demand[i]*2+20,alpha = 0.5)
        #scatter 可以調整點的大小和透明度
        #x.append(point_list[i][1])
        #y.append(point_list[i][2])
        i += 1

    #plt.plot(x,y,'o')

    #plt.show()

#------------------------------------------

#------------讀output 找最佳解------------------
    try:
        f = open(data_name[2]+'.txt','r')
    except IOError:
        print (data_name[2],'.txt is not accessible.')
        exit()

    rout_num = 0
    total_dis = 0
    total_emision = 0
    best_distance = 1e12
    best_emission = 1e12
    max_dis = 0
    max_emission = 0
    min_emission = 1e12
    best_rout = []

    rout = f.readline()
    while rout:
        #print (rout)
        if len(rout) < 10:
            rout = f.readline()
            continue
        rout_num += 1
        line = f.readline()
        line = line.split()
        distance = float(line[3])
        total_dis += distance
        line = f.readline()
        line = line.split()
        emission = float(line[2])
        total_emision += emission
        if distance < best_distance:
            best_rout = rout
            best_distance = distance
            best_emission = emission
        if distance > max_dis:
            max_dis = distance
        if emission > max_emission:
            max_emission = emission
        if emission < min_emission:
            min_emission = emission
        rout = f.readline()

    f.close()
    #print (best_rout)
    print ('best distance = ',best_distance)
    print ('best emission = ',best_emission)
    print ('average dis = ',round(total_dis/rout_num,3))
    print ('average emission = ',round(total_emision/rout_num,3))
    print ('min emission = ',min_emission)
    print ('max dis = ',max_dis)
    print ('max emission = ',max_emission)

#------------------------------------------------

#----------視窗繪製---------------------
    plt.title("vehicle routing-"+data_name[2])

#label 設定 x,y標籤名稱
    plt.ylabel("y")
    plt.xlabel("x")

    #plt.xscale('linear')
#設定畫圖兩邊邊界 xticks yticks
    #plt.xticks([ 0, 10, 20, 30, 40, 50, 60])
    #plt.yticks([10, 20, 30, 40, 50, 60, 70])

    #plt.xlim(0,60)
    #plt.ylim(10,70)

#-------------------------------------------

#-----------路線處理--------------------------------
    print(best_rout)
    best_rout = best_rout.strip('x chorme :')
    print ('原路線(未切割)')
    print (best_rout)
    best_rout = best_rout.split()
    for i in range(len(best_rout)):
        best_rout[i] = int(best_rout[i])

#先塞好空的，準備二維
    each_rout = []
    j = 0
    while j<vehicle_num+5:   #有可能沒解出vehicle 內的解，所以放寬
        each_rout.append([])
        j+=1
#先塞第一個depot方便後面處理
    each_rout[0].append(best_rout[0])
    num_rout = 0
#用depot來分割路線
    j = 1
    while j<len(best_rout)-1:
        if best_rout[j] == depot:
            each_rout[num_rout].append(best_rout[j])
            num_rout += 1
        each_rout[num_rout].append(best_rout[j])
        j+=1   

    each_rout[num_rout].append(depot)
    num_rout += 1
    print ("切開後路線")
    print (each_rout)
    print ("總共有",num_rout,"條路線")


    for i in range(num_rout):
        x = []
        y = []
        load = 0.0
        print ("rout ",i,"個數是",len(each_rout[i]))
        for j in range(len(each_rout[i])):
            x.append(point_list[each_rout[i][j]-1][1])
            y.append(point_list[each_rout[i][j]-1][2])
            #print ("rout i j = ",rout[i][j])
            #print (float(demand[rout[i][j]-1]))
            load += float(demand[each_rout[i][j]-1])
            if j < len(each_rout[i])-1:
                plt.text(point_list[each_rout[i][j]-1][1],point_list[each_rout[i][j]-1][2],j)
        print ("load = ",load)
        plt.plot(x,y)

#----------------------------------------------------
#顯示所有畫好的圖

    save_name = 'Result//' + data_name[2] + '_' + str(best_distance)+'_' + str(best_emission) + '.png'
    plt.savefig(save_name)
    plt.clf()
    #plt.show()
    try:    
        result_save = open('all_result.txt','a')
    except IOError:
        print ('all_result.txt is not accessible.')
        exit()

    result_save.write(data_name[2]+' '+str(best_distance)+' ' + str(best_emission)+'\n')

    result_save.close

    os.system("pause")
#------------------------------------------
