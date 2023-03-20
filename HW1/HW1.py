import random
from random import choice
import queue

frames = [ 10,20,30,40,50,60,70,80,90,100 ]
Ran_ref_str = []
Ran_dirtybit = []
Loc_ref_str = []
Loc_dirtybit = []
MyStr_ref_str = []
MyStr_dirtybit = []
keylist = []          #字典轉為陣列為排序用

def Random_dirtybit():
    while len(Ran_dirtybit) < 180000:
        Ran_dirtybit.append( random.randint(0,1) )   #dirtybit取隨機值存入Ran_dirtybit
    return Ran_dirtybit 
''' print(Random_dirtybit()) '''
file1 = open( 'Random_dirtybit.txt', 'w')
file1.write( str(Random_dirtybit()) )
''' file1.write( " ".join(str(i) for i in Random_dirtybit()) ) '''
file1.close()


def Random_ref_str():
    while len(Ran_ref_str) < 180000 :
        Ran_ref_str.append( random.randint(1,600))          #1~600取隨機值存入Ran_ref_str
    return Ran_ref_str
''' print( Random_ref_str() )  '''
file2 = open( 'Random_ref_str.txt', 'w')
file2.write( str(Random_ref_str()) )
''' file2.write( " ".join(str(i) for i in Random_ref_str()) ) '''
file2.close()


def Locality_dirtybit():
    while len(Loc_dirtybit) < 180000:
        Loc_dirtybit.append( random.randint(0,1) )     #dirtybit取隨機值存入Loc_dirtybit
    return Loc_dirtybit
''' print(Locality_dirtybit()) '''
file3 = open( 'Locality_dirtybit.txt', 'w')
file3.write( str(Locality_dirtybit()) )
''' file3.write( " ".join(str(i) for i in Locality_dirtybit()) ) '''
file3.close()

def Locality_ref_str():
    list = []
    while len(Loc_ref_str) < 180000 : 
        now_set_count = 0     #此集合目前擁有的元素數量
        goal_set_count = random.randint(1,100)    #此集合目標擁有的元素數量，每個集合擁有的元素為1~200個
        start_num = random.randint(1,600)    #某集合的第一個數
        end_num = start_num + random.randint(15,20)   #某集合的最後一個數
        while now_set_count < goal_set_count :    #目前擁有的=目標擁有的元素數量時才會跳出迴圈
            if end_num > 600 :
                end_num2 = end_num%600   #因為最後一個數超過600 所以end_num%600 1~end_num2
                list.append( random.randint( start_num,600 ))  #start_num~600取一個隨機數加入list
                list.append( random.randint( 1,end_num2 ))      #1~end_num2取一個隨機數加入list
                Loc_ref_str.append(choice(list))           #list內選到的數字加入Loc_ref_str
                now_set_count += 1  #此集合目前擁有的元素數量
            else:
                num = random.randint( start_num,end_num )        #選到的數字
                Loc_ref_str.append(num)           #加入Loc_ref_str
                now_set_count += 1  #此集合目前擁有的元素數量
    if len(Loc_ref_str) > 180000:
        num = len(Loc_ref_str)%180000
        for i in range(num):
            Loc_ref_str.pop(i)
    return Loc_ref_str 
''' print(Locality_ref_str()) '''
file4 = open( 'Locality_ref_str.txt', 'w')
file4.write( str(Locality_ref_str()) )
''' file4.write( " ".join(str(i) for i in Locality_ref_str()) ) '''
file4.close() 


def MyString_dirtybit():
    while len(MyStr_dirtybit) < 180000:
        MyStr_dirtybit.append( random.randint(0,1) )   #dirtybit取隨機值存入Ran_dirtybit
    return MyStr_dirtybit 
''' print(Random_dirtybit()) '''
file5 = open( 'MyString_dirtybit.txt', 'w')
file5.write( str(MyString_dirtybit()) )
''' file5.write( " ".join(str(i) for i in MyString_dirtybit()) ) '''
file5.close()

def MyString_ref_str():
    while len(MyStr_ref_str) < 180000:
        if len(MyStr_ref_str) <= 30000:
            MyStr_ref_str.append( random.randint(1,100) )
        elif len(MyStr_ref_str) <= 60000:
            MyStr_ref_str.append( random.randint(101,200) )
        elif len(MyStr_ref_str) <= 90000:
            MyStr_ref_str.append( random.randint(201,300) )
        elif len(MyStr_ref_str) <= 120000:
            MyStr_ref_str.append( random.randint(301,400) )
        elif len(MyStr_ref_str) <= 150000:
            MyStr_ref_str.append( random.randint(401,500) )
        else:
            MyStr_ref_str.append( random.randint(501,600) )
    return MyStr_ref_str
file6 = open( 'MyString_ref_str.txt', 'w')
file6.write( str(MyString_ref_str()) )
''' file6.write( " ".join(str(i) for i in MyString_ref_str()) ) '''
file6.close()


def FIFO(ref_str,dirtybit):
    for frame_size in frames:       #依序讀取frames陣列內的值(索引值為frame_size)
        page_fault = 0
        interrupt = 0
        disk_write = 0
        victim = 0           #發生page fault時，要從frame被拿出的數值
        now_queue = queue.Queue(frame_size)        #FIFO佇列，frame空間
        for e in ref_str:      #依序讀取ref_str陣列內的值(索引值為e)
            if e in now_queue.queue:    #要進來的值已經在frame中
                if dirtybit[e] ==1:
                    disk_write +=1          #若這個元素的dirtybit是1，則disk write一次
                else:
                    continue;
            else:                       #要進來的值不在frame中
                page_fault += 1
                disk_write += 1               #發生page fault，則disk write一次
                if len(now_queue.queue) < frame_size:   #frame沒有滿
                    now_queue.put(e)
                else:                             #frame滿了
                    victim = now_queue.get()     #從FIFO佇列拿走的數字
                    now_queue.put(e)        #FIFO佇列加入索引值e的值
        interrupt = disk_write                    #發生Disk write時需要呼叫OS所以有interrupt
        print("Frames: ",frame_size)
        print("Page fault: ",page_fault)
        print("Interrupt: ",interrupt)
        print("Disk write: ",disk_write,"\n")
        print("[Frame size]=",frame_size,"[Page fault]=",page_fault,"[Interrupt]=",interrupt,"[Disk write]=",disk_write,file=open('Result.txt','a'))



def Optimal(ref_str,dirtybit):
    for frame_size in frames:        #依序讀取frames陣列內的值(索引值為f)
        frame_table=[]
        future_str = []
        element_count = {}    #用字典存{值(key):後續會出現幾次(value)}
        page_fault = 0
        interrupt = 0
        disk_write = 0
        victim = 0           #發生page fault時，要從frame被拿出的數值
        for e in range(len(ref_str)):      #依序讀取ref_str陣列內的值(索引值為e)
            future_str.append(ref_str[e])
            continue;
        
#        print("future_str_建立",future_str)
        for e in range(len(ref_str)):
            value = ref_str[e]                 #要進來的值
            del future_str[0]        #刪掉future_str[0]
#            print("要進來的值:",value,"future str:",future_str)
            count = 0                    #字典的對應值(未來陣列中出現次數記錄)
        
            if value in frame_table:     #hit，不發生page fault
#                print("HIT!!!")
#                print("frame table:",frame_table,"\n") 
                if dirtybit[e] ==1:
                    disk_write +=1          #若這個元素的dirtybit是1，則disk write一次
                else:                       #dirtybit[e]=0
                    continue;
            else:                       #要進來的值不在frame中，發生page fault
                page_fault += 1
                disk_write += 1         #發生page fault，則disk write一次
#                print("frame table:",len(frame_table),"frames:",frame_size)
                if len(frame_table) < frame_size:   #frame沒有滿
#                    print("frame還沒滿")
                    frame_table.append( value )   #value加入frame_table
                    element_count[value] = count      #value加入element_count字典，值設為0
                    victim = 0
                else:                               #frame滿了，且要page fault，找victim
#                    print("滿了！")
                    for fs in range(len(future_str)):       #future_str內的值(索引值fs) 
#                        print("future_str:",future_str)
#                        print("victim:",victim)
                        for ft in range(len(frame_table)):
#                            print("fs,ft:",fs,ft)
                            keylist = list(element_count.values())         #以陣列返回字典的value存成keylist
                            keylist.sort()
                            if future_str[fs] == frame_table[ft]:     #frame_table[ft]值未來會用到
                                count = element_count[frame_table[ft]]  #字典ft之對應值+1
                                element_count[frame_table[ft]] = count+1    #更新字典element_count內的鍵frame_table[ft]值
                                    #找Victim，丟掉
                                keylist = list(element_count.values())         #以陣列返回字典的value存成keylist
                                keylist.sort()      #count值做升冪排列                                 
#                                print("dict:",element_count)
                                if keylist[1]>0:
                                    victim = list (element_count.keys()) [list (element_count.values()).index (0)]
                                    del element_count[ victim ]             #丟掉字典內的victim
                                    element_count[ value ] = 0
                                    frame_table.remove( victim )              #刪掉frame table內的victim
                                    frame_table.append( value )   #value加入frame_table
                                    victim = 601
                                    for i in range(len(frame_table)):
                                        element_count[frame_table[i]] = 0      #字典內的值全部改為0
                                    break;
                                #由值找key
                                else:
#                                    print("繼續往下找")
                                    break;
                                
                            elif (keylist[1] == 0 and fs == (len(future_str)-1)):  #未來陣列找到最後一個都沒找到victim
                                    victim = list (element_count.keys()) [list (element_count.values()).index (0)]              #victim即為element_count值為0的鍵
#                                    print("都沒指到的victim:",victim)
                                    del element_count[ victim ]             #丟掉字典內的victim
                                    element_count[ value ] = 0              #value加入字典 值設為0
                                    frame_table.remove( victim )              #刪掉frame table內的victim
                                    frame_table.append( value )   #value加入frame_table
                                    victim = 601;
#                                    print("找到最後了")
                                    break;   
                            else:
                                continue;
                                                                           

                        if victim == 601:
#                            print("已找到victim")
                            break;
                        else:
                            continue;                 

            victim = 0
#            print(element_count,"frame_table:",frame_table,"victim目前值:",victim)
#            print(" Page fault: ",page_fault," Interrupt: ",interrupt," Disk write: ",disk_write,"\n")
        interrupt = disk_write                    #發生Disk write時需要呼叫OS所以有interrupt
        element_count.clear()

        print("Frames: ",frame_size)
        print("Page fault: ",page_fault)
        print("Interrupt: ",interrupt)
        print("Disk write: ",disk_write,"\n")
        print("[Frame size]=",frame_size,"[Page fault]=",page_fault,"[Interrupt]=",interrupt,"[Disk write]=",disk_write,file=open('Result.txt','a'))

def ESC(ref_str,dirtybit):
    for frame_size in frames:       #依序讀取frames陣列內的值(索引值為frame_size)
        frame_table=[]
        page_fault = 0
        interrupt = 0
        disk_write = 0
        ref_bit = {}
        victim = 0           #發生page fault時，要從frame被拿出的數值
        for e in range(len(ref_str)):      #依序讀取ref_str陣列內的值(索引值為e)
            value = ref_str[e]                 #要進來的值
#            print("要進來的值:",value,"ref bit:",ref_bit,"frame table:",frame_table)
            
            if value in frame_table:    #要進來的值已經在frame中
#                print("HIT!!!")
                for i in range(len(frame_table)):
                    ref_bit[frame_table[i]] = 0      #字典內的值全部改為0
                ref_bit[value] = 1                   #將value加入字典 設為1
#                print("後來的ref bit:",ref_bit)
                if dirtybit[e] == 1:
                    disk_write += 1          #若這個元素的dirtybit是1，則disk write一次
                else:
                    continue;
            else:                       #要進來的值不在frame中
                page_fault += 1
                disk_write += 1               #發生page fault，則disk write一次
                if len(frame_table) < frame_size:   #frame沒有滿
#                    print("沒有滿")
                    frame_table.append(value)       #value加入frame table
                    for i in range(len(frame_table)):
                        ref_bit[frame_table[i]] = 0      #字典內的值全部改為0
                    ref_bit[value] = 1                   #將value加入字典 設為1

                else:                             #frame滿了
                    for i in range(len(frame_table)):           #依序找查frame table內的(reference bit,dirty bit)
                        get_ref_bit = ref_bit.get(frame_table[i])
                        if get_ref_bit == 0 and dirtybit[i] == 0:      #(0,0)
                            victim = frame_table[i]     #victim是(reference bit,dirty bit)=(0,0)的值
#                            print("(0,0)","victim:",victim)
                            break;
                        else:
                            victim = 0                  #都沒有(0,0)
                            continue;
                    if victim == 0:
                        for i in range(len(frame_table)):           #依序找查frame table內的(reference bit,dirty bit)
                            if get_ref_bit == 0 and dirtybit[i] == 1:      #(0,1)
                                victim = frame_table[i]     #victim是(reference bit,dirty bit)=(0,1)的值
#                                print("(0,1)","victim:",victim)
                                break;
                            else:
                                victim = 0               #都沒有(0,1)
                                continue;
                    else:
                        pass;
                    if victim == 0:
                        for i in range(len(frame_table)):           #依序找查frame table內的(reference bit,dirty bit)
                            if get_ref_bit == 1 and dirtybit[i] == 0:      #(1,0)
                                victim = frame_table[i]     #victim是(reference bit,dirty bit)=(1,0)的值
#                                print("(1,0)","victim:",victim)
                                break;
                            else:
                                victim = 0               #都沒有(1,0)
                                continue;
                    else:
                        pass;
                    if victim == 0:
                        victim = frame_table[0]
                        pass;
                    else:
                        pass;
                    
                    del ref_bit[ victim ]                #丟掉字典內的victim
#                    print("已刪除ref bit字典內victim")
                    frame_table.remove(victim)
                    frame_table.append(value)
                    for r in range(len(frame_table)):
                        ref_bit[frame_table[r]] = 0                #將frame table內的所有元素之reference bit全設為0
                    ref_bit[value] = 1                    #value之reference bit設為1
#            print("frame table:",frame_table,"ref bit:",ref_bit,"victim:",victim,"\n")
        interrupt = disk_write                    #發生Disk write時需要呼叫OS所以有interrupt
        print("Frames: ",frame_size)
        print("Page fault: ",page_fault)
        print("Interrupt: ",interrupt)
        print("Disk write: ",disk_write,"\n")
        print("[Frame size]=",frame_size,"[Page fault]=",page_fault,"[Interrupt]=",interrupt,"[Disk write]=",disk_write,file=open('Result.txt','a'))

def MyAlgorithm(ref_str,dirtybit):
    for frame_size in frames:        #依序讀取frames陣列內的值(索引值為f)
#        print(ref_str)
        frame_table=[]
        element_count = {}    #用字典存{值(key):已出現幾次(value)}
        page_fault = 0
        interrupt = 0
        disk_write = 0
        victim = 0           #發生page fault時，要從frame被拿出的數值
        
        for e in range(len(ref_str)):
            value = ref_str[e]                 #要進來的值
#            print("value:",value)
            count = 0                    #字典的對應值(frame table中出現次數記錄)
            if value in frame_table:           #HIT
                if dirtybit[e] == 1:
                    disk_write += 1
                    element_count[value] += 1    #該數字典值+1
#                    print("HIT!","frame table:",frame_table)
                    continue;
                else:
                    continue;
            else:
                page_fault += 1
                disk_write += 1
                if len(frame_table) < frame_size:   #frame還沒滿
#                    print("ADD")
                    frame_table.append(value)
                    element_count[value] = count+1           #value與對應值加入字典
#                    print("element count:",element_count,"frame table:",frame_table)
                    continue;
                else:                        #frame滿了
                    keylist = list(element_count.values())         #以陣列返回字典的value存成keylist
                    keylist.sort()        #count值做升冪排列
                    for i in range(len(frame_table)):
                        if dirtybit[e] == 1:
                            victim = list (element_count.keys()) [list (element_count.values()).index (keylist[i])]
                            frame_table.append(value)
                            element_count[value] = 1           #value加入字典 值設為1
                            del element_count[victim]
                            frame_table.remove(victim)
#                            print("victim:",victim,"frame table:",frame_table,"element count:",element_count)
                            break;
                        else:
                            continue;
        interrupt = disk_write
        print("Frames: ",frame_size)
        print("Page fault: ",page_fault)
        print("Interrupt: ",interrupt)
        print("Disk write: ",disk_write,"\n")
        print("[Frame size]=",frame_size,"[Page fault]=",page_fault,"[Interrupt]=",interrupt,"[Disk write]=",disk_write,file=open('Result.txt','a'))

Random_ref_str()
Random_dirtybit()
Locality_ref_str()
Locality_dirtybit()
MyString_ref_str()
MyString_dirtybit()

#----------------------FIFO----------------------
print("FIFO Random: ")
print("--------------------FIFO Random--------------------\n",file=open('Result.txt','w'))
FIFO(Ran_ref_str,Ran_dirtybit)
print("---------------------------------------------------------")


print("FIFO Locality:")
print("\n--------------------FIFO Locality--------------------\n",file=open('Result.txt','a'))
FIFO(Loc_ref_str,Loc_dirtybit)
print("---------------------------------------------------------")

print("FIFO My string:")
print("\n--------------------FIFO MyString--------------------\n",file=open('Result.txt','a'))
FIFO(MyStr_ref_str,MyStr_dirtybit)    
print("---------------------------------------------------------")

#----------------------Optimal----------------------
print("Optimal Random: ")
print("--------------------Optimal Random--------------------\n",file=open('Result.txt','a'))
Optimal(Ran_ref_str,Ran_dirtybit)
print("---------------------------------------------------------") 


print("Optimal Locality: ")
print("--------------------Optimal Locality--------------------\n",file=open('Result.txt','a'))
Optimal(Loc_ref_str,Loc_dirtybit)
print("---------------------------------------------------------")


print("Optimal My string: ")
print("--------------------Optimal MyString--------------------\n",file=open('Result.txt','a'))
Optimal(MyStr_ref_str,MyStr_dirtybit)
print("---------------------------------------------------------") 


#----------------------ESC----------------------
print("ESC Random: ")
print("--------------------ESC Random--------------------\n",file=open('Result.txt','a'))
ESC(Ran_ref_str,Ran_dirtybit)
print("---------------------------------------------------------")

print("ESC Locality: ")
print("--------------------ESC Locality--------------------\n",file=open('Result.txt','a'))
ESC(Loc_ref_str,Loc_dirtybit)
print("---------------------------------------------------------")

print("ESC My string: ")
print("--------------------ESC MyString--------------------\n",file=open('Result.txt','a'))
ESC(MyStr_ref_str,MyStr_dirtybit)
print("---------------------------------------------------------")

#----------------------My Algorithm----------------------
print("My Algorithm Random: ")
print("--------------------My Algorithm Random--------------------\n",file=open('Result.txt','a'))
MyAlgorithm(Ran_ref_str,Ran_dirtybit)
print("---------------------------------------------------------")

print("My Algorithm Locality: ")
print("--------------------My Algorithm Locality--------------------\n",file=open('Result.txt','a'))
MyAlgorithm(Loc_ref_str,Loc_dirtybit)
print("---------------------------------------------------------")

print("My Algorithm My string: ")
print("--------------------My Algorithm MyString--------------------\n",file=open('Result.txt','a'))
MyAlgorithm(MyStr_ref_str,MyStr_dirtybit)
print("---------------------------------------------------------")