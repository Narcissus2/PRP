import sys
from PyQt5.QtWidgets import QDialog, QApplication
from ShowResult import Ui_Form    #MyFirstUI 是你的.py檔案名字
import matplotlib.pyplot as plt
import numpy as np
import os

# PRP 的部分 ===================================================================================
#找實驗檔
def PRP(want_Distance,want_Fuel,want_Time,want_Write):
    Result_return = ""
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
        # print(explist[exp_index])
        explist[exp_index] = explist[exp_index].split('.')
        datalist.append(explist[exp_index][0]+ '.txt') #把.ini改成.txt
        # print(datalist[exp_index])

    #------------讀output 找最佳解------------------
        try:
            fsol = open(datalist[exp_index],'r')
        except IOError:
            print (datalist[exp_index],' is not accessible.')
            exit()

        # fc = fuel comsumed
        fp_ = 8
        fc_ = 1.4
        rout_num = 0

        total_dis = 0
        total_fc = 0 
        total_time = 0
        total_cost = 0

        best_dis = 1e12
        best_fc = 1e12
        best_time = 1e12
        best_cost = 1e12

        max_dis = 0
        max_fc = 0
        max_time = 0

        min_dis = 1e12
        min_fc = 1e12
        min_time = 1e12
        
        best_rout = []
        best_speed = []

        rout = fsol.readline()
        while rout:
            #print (rout)
            if rout[0] == 'x':
                rout_num += 1
                sol_speed = fsol.readline() # 讀掉speed
                line = fsol.readline()
                line = line.split()
                fc = float(line[3].strip('(L)')) # fuel consumed
                # print('fuel = ',fc)
                line = fsol.readline() # fuel cost (讀掉)
                line = fsol.readline()
                line = line.split()
                time_temp = float(line[2].strip('(H)')) # time 
                line = fsol.readline() # driver cost (讀掉)
                line = fsol.readline()
                line = line.split()
                distance = float(line[2].strip('(KM)')) # distance 
                line = fsol.readline()
                line = line.split()
                cost = float(line[3].strip('($)'))

                total_dis += distance
                total_time += time_temp
                total_fc += fc
                total_cost += cost

                if cost < best_cost:
                    best_rout = rout
                    best_dis = distance
                    best_fc = fc
                    best_time = time_temp
                    best_cost = cost

                if distance < min_dis:
                    min_dis = distance

                if fc > max_fc :
                    max_fc = fc

                if fc < min_fc:
                    min_fc = fc

                if min_time > time_temp:
                    min_time = time_temp

            rout = fsol.readline()

        fsol.close()
        # 都可以打開 目前不想開
        #print (best_rout)
        # print ('best distance = ',best_dis)
        # print ('best fuel comsumed = ',best_fc)
        # print ('best time = ',best_time)
        # print ('best cost = ',best_cost)
        # print ('average dis = ',round(total_dis/rout_num,3))
        # print ('average fuel comsumed = ',round(total_fc/rout_num,3))
        # print ('min fuel comsumed = ',min_fc)
        # print ('max dis = ',max_dis)
        # print ('max fc = ',max_fc)

        try:    
            result_save = open('all_result.txt','a')
        except IOError:
            print ('all_result.txt is not accessible.')
            exit()

        final_result = datalist[exp_index].strip('.txt')+' cost = '+str(best_cost)+'($) fc = ' + str(best_fc)+'(L) time = '+ str(best_time) + '(H) dis = ' + str(best_dis) + '(KM)\n'
        if(want_Time):
            final_result += "min time = " + str(min_time) + '(H)\n'
        if(want_Fuel):
            final_result += "min fuel = " + str(min_fc) + '(L)\n'
        if(want_Distance):
            final_result += "min Distance = " + str(min_dis) + '(KM)\n'
        if(want_Write):
            result_save.write(datalist[exp_index].strip('.txt')+' '+str(best_cost)+' ' + str(best_fc)+' '+ str(best_time) + str(best_dis) + '\n')
        

        Result_return += final_result
        result_save.close()

    return Result_return
#------------------------------------------------


# UI 部分 ===================================================================================
def Run(self,Distance,Fuel,Time,Write_file):
    s = PRP(Distance,Fuel,Time,Write_file)
    self.ui.textBrowser.setText(s)

# ============================================================================================
class AppWindow(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_Form()
        self.ui.setupUi(self)

        #綁上與點擊事件對應的function，所有東西都在ui底下！！
        self.ui.pushButton.clicked.connect(self.pushButton_Click)
        self.ui.checkBox.stateChanged.connect(self.checkBox_check)
        self.ui.checkBox_2.stateChanged.connect(self.checkBox2_check)
        self.ui.checkBox_3.stateChanged.connect(self.checkBox3_check)
        self.ui.checkBox_4.stateChanged.connect(self.checkBox4_check)
        self.show()
    Distance = False
    Fuel = False
    Time = False
    Write_file = False
    def pushButton_Click(self):
        Run(self,self.Distance,self.Fuel,self.Time,self.Write_file)

    def checkBox_check(self):
        self.Distance = not self.Distance
    def checkBox2_check(self):
        self.Fuel = not self.Fuel
    def checkBox3_check(self):
        self.Time = not self.Time
    def checkBox4_check(self):
        self.Write_file = not self.Write_file


app = QApplication(sys.argv)
w = AppWindow()
w.show()
sys.exit(app.exec_())