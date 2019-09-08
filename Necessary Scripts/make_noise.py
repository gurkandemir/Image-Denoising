import sys
import numpy as np

img = np.loadtxt(sys.argv[1])

pi = float(sys.argv[2])
I,J = img.shape
flip = np.random.rand(I,J) < pi
X = img * (-1)**flip

file = open(sys.argv[3], "w")
txt = list(X)
for i in txt:
	for j in i:
		file.write(str(int(j)) + " ")
	file.write('\n')
file.close()