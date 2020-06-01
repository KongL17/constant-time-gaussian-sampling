import os

n = 50
result = []
for i in range(n):
    s = os.popen("./sample_no_128")
    r = s.readlines()
    result.append([int(r[0][:-1]), int(r[1][:-1])])
    print(int(r[0][:-1]))
    print(i)

data1 = 0
data2 = 0
for j in result:
    data1 += j[0]
    data2 += j[1]

print(data1/n, '\n', data2/n)


