inp=input("enter text: ")
d=dict()
for i in inp:
    if not i.isalpha(): continue
    d.setdefault(i, 0)
    d[i]+=1

l=sorted(list(d.items()), key=lambda x: x[1])
l=["unknown"]*(26-len(l))+[i[0] for i in l]
print("letter -- possible match")

for i,j in zip(l, "zjqxkvbpgwyfmculdhrsnioate"):
    print(i, "--", j)
