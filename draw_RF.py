import matplotlib.pyplot as plt
import numpy as np
import os

# ----------------------------------------------------------------
# 讀取explist.ini檔案去找測試結果和測式資料
# 讀取到Results \\ 資料夾下的 Run .txt 的檔案
# 畫出全部實驗的點和最佳曲線
# ----------------------------------------------------------------

#讀實驗檔
try:
    fexp = open('explist.ini','r')
except IOError:
    print ('explist.ini is not accessible.')
    exit()

algorithm = 'NSGAIII'

explist = []
line = fexp.readline()
while line:
    line = line.strip('\n')
    explist.append(line)
    line = fexp.readline()

for all_exp in range(len(explist)):
    exp_path = 'Experiments//'+str(explist[all_exp])
#讀取測資檔名改成要讀取的檔名------------------------------------------
    fini = open(exp_path,'r')
    want_line = 7
    data_name = []
    for i in range(want_line):
        data_name = fini.readline()
    data_name = data_name.split()
    for run_num in range(20):
        data_path = 'Results//'+  algorithm + '-' + data_name[2] + '-' + 'Run' + str(run_num) + '.txt'
        print ("data path = ",data_path)

        fini.close()
        #os.system('pause')
#讀完ini檔-----------------------------------------

#open file 'r'唯讀模式
#try 檢查檔案是否存在
        try :
            f = open(data_path,'r')
        except IOError:
            print ('File is not accessible.')
            exit()

#抓 x y ----------------------------------
        point_list = []
        line = f.readline()
        while line:
            line = f.readline()
            line = line.split()
            if not line: 
                break
            #print(line)
            point_list.append(line)
        #os.system('pause')
        f.close()
#--------------------------------------------

#----------畫氣泡圖---------------------
#把每個點畫出來
        i = 0
        x = []
        y = []
        point_list.sort()
        while i < len(point_list):
            j=0
            while j<2:
                point_list[i][j] = float(point_list[i][j])
                j+=1
            #print(point_list[i][1]," ",point_list[i][2])
            plt.plot(point_list[i][0],point_list[i][1],'o')
            #plt.plot(point_list[i][0],point_list[i][1])
            k = 0
            check = 1
            while k < i:
                if point_list[k][0]< point_list[i][0] and point_list[k][1]< point_list[i][1]:
                    check = 0
                    break
                elif point_list[k][0] < point_list[i][0] and point_list[k][1] == point_list[i][1]:
                    check = 0
                    break
                elif point_list[k][0] == point_list[i][0] and point_list[k][1] < point_list[i][1]:
                    check = 0
                    break
                k+=1
                #print ('k=',k)
            if check == 1:
                x.append(point_list[i][0])
                y.append(point_list[i][1])
            #print ('i=',i)
            i += 1
        #os.system('pause')
        #print ('xlen = ', len(x))
        #print ('ylen = ', len(y))
        check = 1
        while check:
            check = 0
            for xnum in range(len(x)-1):
                #print ('xnum = ',xnum)
                #print ('range = ',range(len(x)))
                xnum2 = xnum+1
                while xnum2 < len(x):
                    #print ('x[',xnum2,'] <= x[',xnum,']') 
                    if xnum == xnum2:
                        continue
                    if x[xnum2] <= x[xnum] and y[xnum2] <= y[xnum]:
                        #print ('yes x[',xnum2,'] <= x[',xnum,']') 
                        del x[xnum]
                        del y[xnum]
                        xnum = 0
                        #print (len(x))
                        #print (len(y))
                        check = 1
                        break
                    xnum2 += 1
                
        #os.system('pause') 
        print ('after xlen = ', len(x))
        print ('x = ',x)
        print ('after ylen = ', len(y))
        print ('y = ',y)
        plt.plot(x,y,'-')
        

    #plt.show()

#------------------------------------------


#----------視窗繪製---------------------
        plt.title("vehicle routing-RF-"+data_name[2])
        #label 設定 x,y標籤名稱
        plt.ylabel("emission")
        plt.xlabel("distance")

    #plt.xscale('linear')
#設定畫圖兩邊邊界 xticks yticks
    #plt.xticks([ 0, 10, 20, 30, 40, 50, 60])
    #plt.yticks([10, 20, 30, 40, 50, 60, 70])

    #plt.xlim(0,60)
    #plt.ylim(10,70)

#-------------------------------------------

#顯示所有畫好的圖

        save_name = 'RF//'+ data_name[2] + '-' + 'Run' + str(run_num) +  '.png'
        plt.savefig(save_name)
        plt.clf()
        #plt.show()

os.system("pause")
#------------------------------------------
