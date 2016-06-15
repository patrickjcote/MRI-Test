import sys
import time
from reel import Reel

cmd = sys.argv[1]
value = sys.argv[2]

x = Reel(0x48)
if(cmd == "rd"):
    x.rd(int(value))
elif(cmd == "cd"):
    x.cd()
elif(cmd == "q"):
    x.q()
elif(cmd == "pu"):
    x.pu()
elif(cmd == "s"):
    x.s()
    
