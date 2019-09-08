import sys
import numpy as np
import warnings
warnings.filterwarnings("ignore")

from matplotlib import pyplot as plt
import imageio

filepath = sys.argv[1]

img = np.loadtxt(filepath)
imageio.imwrite(sys.argv[2], img)

plt.imshow(img,cmap='gray',vmin=-1,vmax=1)
plt.show()