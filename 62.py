import sys

def is_digit(c): return c in '0123456789'

lines=[]
fname=sys.argv[1]

open(fname, 'a').close()

with open(fname, "r+") as f:
    lines = f.readlines()

    print("> ", end='')
    sys.stdout.flush()
    for line in sys.stdin:
        line=line.lstrip()
        if line.rstrip() in ["quit", "q", "exit"]: break
        i=0
        while i<len(line) and is_digit(line[i]): i+=1
        loc=int(line[:i])-1
        line = line[i:].lstrip()
        if len(line)==0 or line[0] not in "ei":
            print(loc+1, "|", lines[loc])
        elif line[0] == 'e':
            lines[loc] = line[1:]
        elif line[0] == 'i':
            lines = lines[:loc] + [line[1:]] + lines[loc:]
        print("> ", end='')
        sys.stdout.flush()

    f.seek(0)
    f.writelines(lines)
    f.flush()
