import subprocess
import sys
import os
from subprocess import PIPE

os.chdir("../models/syntaxnet")
p = subprocess.Popen(["./syntaxnet/demo2.sh"], stdout=PIPE, stdin=PIPE)
out = p.communicate(input=sys.argv[1])[0]
print out
