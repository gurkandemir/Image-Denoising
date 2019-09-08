import sys
from PIL import Image
import numpy as np

filepath = sys.argv[1]

imgX = Image.open(filepath)
imgX = imgX.convert('L')
img = np.asarray(imgX)
img = 2*(img > 128).astype(int)-1

file = open(sys.argv[2], "w")
txt = list(img)
for i in txt:
	for j in i:
		file.write(str(j) + " ")
	file.write('\n')
file.close()