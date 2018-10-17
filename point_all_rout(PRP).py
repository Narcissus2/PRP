import matplotlib.pyplot as plt
import numpy as np
import os
import math
import time

# ----------------------------------------------------------------
# 讀取explist.ini檔案去找測試結果和測式資料
# 找出檔案內的最佳路線畫出來放到RoutPicture的資料夾目錄下
# 顯示出最低distance 最低fuel consumped 還有平均distance 和 平均 fc
# 最後把最佳結果記錄到allresult.txt裡面
# ----------------------------------------------------------------



#找實驗檔
try:
    fexp = open('explist.ini','r')
except IOError:
    print ('explist.ini is not accessible.')
    exit()

explist = [] #這裡會存總共有哪些.ini
line = fexp.readline()
while line:
    line = line.strip('\n')
    explist.append(line)
    line = fexp.readline()

datalist = [] #存下要讀的.txt有哪些
for exp_index in range(len(explist)):
    print(explist[exp_index])
    explist[exp_index] = explist[exp_index].split('.')
    datalist.append(explist[exp_index][0]+ '.txt') #把.ini改成.txt
    print(datalist[exp_index])
    data_path = 'Experiments//' + datalist[exp_index]

#讀完ini檔-----改好.txt ------------------------------

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
    temp = f.readline()
    temp = temp.strip('\n')
    print("points = ",temp)
    point_num = int(temp)+1
    temp = f.readline() #廢物空行
    car_load = f.readline()
    # print(temp)
    car_load = car_load.split()
    # print(temp[0])
    print("empty load = " + car_load[0] + "  max load = " + car_load[1])
    temp = f.readline() #廢物空行
    speed_limit = f.readline()
    speed_limit = speed_limit.split()
    print("low speed = " + speed_limit[0] + "  high speed = " + speed_limit[1])

#抓完基本資訊-----------------------------


#跳過距離資訊 ----------------------------------
    temp = f.readline() #廢物空行
    for i in range(point_num):
        temp = f.readline()
    temp = f.readline() #廢物空行

#--------------------------------------------

#抓demand-------------------------------------------
    line = f.readline()
    demand = []
    while line:
        if line=='\n':
            line = f.readline()
            continue
        line = line.split()
        # print(line[2])
        demand.append(float(line[2]))
        line = f.readline()
    f.close()
#----------------------------------------------------------

#開啟座標檔案------------------------------------------
    # print('i = ' + str(exp_index))
    # print(datalist)
    data_path = 'Experiments//place_' + datalist[exp_index]
    try :
        fplace = open(data_path,'r')
    except IOError:
        print ('File is not accessible.')
        exit()
#--------------------------------------------

#抓取點座標------------------------------------------
    point_list = []
    for i in range(point_num):
        line = fplace.readline()
        line = line.split()
        #print(line)
        point_list.append(line)

    for i in range(point_num):
        print(point_list[i])
#--------------------------------------------


#----------畫氣泡圖---------------------
#把每個點畫出來
    i = 0

    while i < len(point_list):
        j=1
        while j<3:
            point_list[i][j] = float(point_list[i][j])
            j+=1
        #print(point_list[i][1]," ",point_list[i][2])
        # plt.plot(point_list[i][1],point_list[i][2],'o')
        #plot只能畫點和線
        plt.scatter(point_list[i][1],point_list[i][2],demand[i]*2+20,alpha = 0.5)
        #scatter 可以調整點的大小和透明度
        #x.append(point_list[i][1])
        #y.append(point_list[i][2])
        i += 1

    # plt.plot(x,y,'o')

    # plt.show()
    # 目前寫到這裡 2018.10.09-12:26
    # 太多點跟距離對不上... 沒屁用

#------------------------------------------

#------------讀output 找最佳解------------------
    try:
        fsol = open(datalist[exp_index],'r')
    except IOError:
        print (datalist[exp_index],' is not accessible.')
        exit()

    # fc = fuel comsumed
    rout_num = 0
    total_dis = 0
    total_fc = 0
    total_time = 0
    best_distance = 1e12
    best_emission = 1e12
    best_time = 1e12
    best_fc = 1e12 
    max_dis = 0
    max_fc = 0
    min_fc = 1e12
    max_time = 0
    min_time = 1e12

    best_rout = []

    rout = fsol.readline()
    while rout:
        #print (rout)
        if rout[0] == 'x':
            rout_num += 1
            sol_speed = fsol.readline() # 讀掉speed
            line = fsol.readline()
            line = line.split()
            fc = float(line[3].strip('(L)'))
            # print('fuel = ',fc)
            line = fsol.readline()
            line = line.split()
            time_temp = float(line[2].strip('(H)'))
            line = fsol.readline()
            line = line.split()
            distance = float(line[2].strip('(KM)'))
            total_dis += distance
            total_time += time_temp
            total_fc += fc
            if distance < best_distance:
                best_rout = rout
                best_distance = distance
                best_fc = fc
                best_time = time_temp
            if distance > max_dis:
                max_dis = distance
            if fc > max_fc :
                max_fc = fc
            if fc < min_fc:
                min_fc = fc
            if min_time > time_temp:
                min_time = time_temp

        rout = fsol.readline()

    fsol.close()
    #print (best_rout)
    print ('best distance = ',best_distance)
    print ('best fuel comsumed = ',best_fc)
    print ('best time = ',best_time)
    print ('average dis = ',round(total_dis/rout_num,3))
    print ('average fuel comsumed = ',round(total_fc/rout_num,3))
    print ('min fuel comsumed = ',min_fc)
    print ('max dis = ',max_dis)
    print ('max fc = ',max_fc)

#------------------------------------------------

#----------視窗繪製---------------------
    plt.title("vehicle routing-"+datalist[exp_index].strip('.txt'))

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
    vehicle_num = 30 # 可負載35 vehicle num (目前測資不超過)
    depot = 0
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

    print ('point list')
    print (point_list)
    for i in range(num_rout):
        x = []
        y = []
        load = 0.0
        print ("rout ",i,"個數是",len(each_rout[i]))
        for j in range(len(each_rout[i])):
            # x.append(point_list[each_rout[i][j]-1][1])
            # y.append(point_list[each_rout[i][j]-1][2])
            x.append(point_list[each_rout[i][j]][1])
            y.append(point_list[each_rout[i][j]][2])
            #print ("rout i j = ",rout[i][j])
            #print (float(demand[rout[i][j]-1]))
            # load += float(demand[each_rout[i][j]-1])
            load += float(demand[each_rout[i][j]])
            if j < len(each_rout[i])-1:
                #plt.text(point_list[each_rout[i][j]-1][1],point_list[each_rout[i][j]-1][2],j)
                # plt.text(point_list[each_rout[i][j]-1][1],point_list[each_rout[i][j]-1][2],str(point_list[each_rout[i][j]-1][0])) #改
                plt.text(point_list[each_rout[i][j]][1],point_list[each_rout[i][j]][2],str(point_list[each_rout[i][j]][0]))
        print ("load = ",load)
        plt.plot(x,y)

#----------------------------------------------------
#顯示所有畫好的圖

    save_name = 'RoutPicture//' + datalist[exp_index].strip('.txt') + '_' + str(best_distance)+'_' + str(best_fc) + '_' + str(best_time) + '.png'
    plt.savefig(save_name)
    plt.clf()
    #plt.show()
    try:    
        result_save = open('all_result.txt','a')
    except IOError:
        print ('all_result.txt is not accessible.')
        exit()

    result_save.write(datalist[exp_index].strip('.txt')+' '+str(best_distance)+' ' + str(best_fc)+' '+ str(best_time) + '\n')

    result_save.close()
    # time.sleep(1) # 間隔1秒
    # os.system("pause")
#------------------------------------------
