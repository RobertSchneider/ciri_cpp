import subprocess
import sys
import os
from subprocess import PIPE

from subprocess import check_output
#out = check_output(["sh", "-c"] + sys.argv[1].split(' '), shell=True)
p = subprocess.Popen(["sh", "-c", sys.argv[1]], stdout=PIPE, shell=True)
out = p.communicate()[0]
print out
