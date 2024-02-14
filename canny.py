import cv2
import numpy as np
 
img = cv2.imread('coins.jpg',cv2.IMREAD_GRAYSCALE)
 
filter = cv2.Canny(img,100,200)
cv2.imwrite('coinsCanny.jpg',filter)
