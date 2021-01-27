import cv2
import numpy as np

mtx = np.zeros((3,3))
mtx[0,0] = 746.4909984007513
mtx[1,1] = 746.641656940557
mtx[0,2] = 304.534352530265
mtx[1,2] = 247.27130406293284
mtx[2,2] = 1.0

dist = np.array([-0.20507558,  0.84151962,  0.00276314, -0.00344598, -1.6605042 ],  dtype=np.float32)
w = 640
h = 480
newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))

cv2.namedWindow("preview")
vc = cv2.VideoCapture(0)

if vc.isOpened(): # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    img = cv2.undistort(frame, mtx, dist, None, newcameramtx)
    cv2.imshow("preview", img)
    rval, frame = vc.read()
    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()
cv2.destroyWindow("preview")