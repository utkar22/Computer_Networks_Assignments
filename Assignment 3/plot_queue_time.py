import matplotlib.pyplot as plt

f = open("tcp-example.tr","r")
x = f.readlines()
f.close()

enqueue = []
dequeue = []

diff = []

for i in x:
    ls = i.split()
    if ("/NodeList/1/DeviceList/1" in ls[2]):
        if ls[0] == "+":
            enqueue.append(float(ls[1]))
        elif ls[0] == "-":
            dequeue.append(float(ls[1]))

for i in range(min(len(enqueue),len(dequeue))):
    diff.append(dequeue[i]-enqueue[i])


f = open("tcp-example.txt","w")

for i in range(len(diff)):
    print(f"{enqueue[i]} {diff[i]}",file = f)

f.close()

plt.plot(enqueue[:len(diff)],diff)
plt.show()
