import sys
from reel import Reel

print sys.argv[1]

x = Reel(0x48)
x.rd(int(sys.argv[1]))

x.cd()
