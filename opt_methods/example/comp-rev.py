import os
import sys
import time

n = 5

def permutations(n):
    p = []
    indices = list(range(n))
    p.append(indices.copy())
    while True:
        low_index = n-1
        while low_index > 0 and indices[low_index-1] > indices[low_index]:
            low_index -= 1
        if low_index == 0:
            break
        low_index -= 1
        high_index = low_index+1
        while high_index < n and indices[high_index] > indices[low_index]:
            high_index += 1
        high_index -= 1
        indices[low_index], indices[high_index] = indices[
            high_index], indices[low_index]
        indices[low_index+1:] = reversed(indices[low_index+1:])
        p.append(indices.copy())
    return p

def count_logic(url):
    count = 0
    f = open(url, 'r')
    while True:
        s = f.readline()
        if s == '':
            break
        else:
            count += s.count('&')
            count += s.count('|')
            count += s.count("!")
    f.close()
    return count



pre = permutations(n)
#print(pre, len(pre))
f = open(r"C:\Users\sony\Desktop\opt_method\example\in.txt", 'r')
p = []
p_head = []
p_tial = []
for i in range(4):
    p_head.append(f.readline())
for i in range(n):
    p.append(f.readline())
while True:
    s = f.readline()
    if s == '':
        break
    else:
        p_tial.append(s)
f.close()

#f1 = open(r"C:\Users\sony\Desktop\opt_method\result.out", 'a')

global_count = 0
max = [0,999999999999999999999999999]
for i in pre:
    f = open(r"C:\Users\sony\Desktop\opt_method\example\cache.txt", 'w')
    for k in p_head:
        f.write(k)
    mm = 0
    for j in i:
        f.write(p[mm].split(' ')[0]+' '+p[j].split(' ')[1])
        mm += 1
    for m in p_tial:
        f.write(m)
    f.close()
    os.system(r"C:\Users\sony\Desktop\opt_method\example\espresso.exe -Dso -S1 -o eqntott C:\Users\sony\Desktop\opt_method\example\cache.txt > C:\Users\sony\Desktop\opt_method\example\cache.out")
    time.sleep(0.1)
    count = count_logic(r"C:\Users\sony\Desktop\opt_method\example\cache.out")
    print(count)
    #f1.write(str(count)+'\n')
    if count <= max[1]:
        max[0] = i.copy()
        max[1] = count
    #print(max)
    #f1.write(str(max)+'\n')
    global_count += 1
    print(global_count)
    #f1.write(str(global_count)+'\n')

print(max)
f1 = open(r"C:\Users\sony\Desktop\opt_method\example\result.out", 'a')
f1.write(str(max)+'\n')
f1.close()

s_r = " "
f2 = open(r"C:\Users\sony\Desktop\opt_method\example\in.txt", 'r')
for i in range(4):
    s_r += f2.readline()
s = []
for i in range(n):
    s.append(f2.readline())
for i in range(n):
    s_r += s[i].split(" ")[0] + ' ' + s[max[0][i]].split(' ')[1]
# for i in range(n):
#     (s[i], s[max[0][i]]) = (s[max[0][i]], s[i])
while True:
    s_p = f2.readline()
    print(s_p)
    if ".e" in s_p or s_p == "":
        break
    else:
        s_r += s_p
f2.close()

#for i in range(n):
#    s_r += s[i].split(" ")[0] + ' ' + s[max[0][i]].split(' ')[1]
#    s[max[0][i]]

s_r += ".e"
f3 = open(r"C:\Users\sony\Desktop\opt_method\example\in.txt", 'w')
f3.write(s_r)
f3.close()
