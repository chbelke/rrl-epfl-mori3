
import numpy as np
import cv2
# import glob
# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

num_squares_x = 6
num_squares_y = 9
# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((num_squares_x*num_squares_y,3), np.float32)
objp[:,:2] = np.mgrid[0:num_squares_y,0:num_squares_x].T.reshape(-1,2)
# Arrays to store object points and image points from all the images.
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.
num_good = 0
reject_every_n = 0

# images = glob.glob('*.jpg')


cv2.namedWindow("preview")
vc = cv2.VideoCapture(0)

if vc.isOpened(): # try to get the first frame
    rval, img = vc.read()
else:
    rval = False

while rval:
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, (num_squares_y,num_squares_x), None)

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

    if ret == True or key == 32:
        corners2 = cv2.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
        cv2.drawChessboardCorners(img, (num_squares_y,num_squares_x), corners2, ret)
        reject_every_n += 1            
        if reject_every_n != 10:
            continue
        objpoints.append(objp)
        imgpoints.append(corners)
        num_good += 1
        reject_every_n = 0
    
        print(ret, num_good)

    if num_good >= 50:
        break

    cv2.imshow('img', img)

    rval, img = vc.read()


ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

print()
print("mtx")
print("mtx[0,0] = {}".format(mtx[0,0]))
print("mtx[1,1] = {}".format(mtx[1,1]))
print("mtx[0,2] = {}".format(mtx[0,2]))
print("mtx[1,2] = {}".format(mtx[1,2]))
print("mtx[2,2] = {}".format(mtx[2,2]))
print()

print("dist")
print(dist)
for d in dist:
    print(d, end=", ")
print(" dtype=np.float32)")
print()

h,  w = img.shape[:2]
newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))
print("newcameramtx")
print(newcameramtx)
print()

print("roi")
print(roi)


x, y, w, h = roi
# dst = dst[y:y+h, x:x+w]
print("w, h")
print(w, h)
print()

if vc.isOpened(): # try to get the first frame
    rval, img = vc.read()
else:
    rval = False

while rval:
    dst = cv2.undistort(img, mtx, dist, None, newcameramtx)
    cv2.imshow('img', img)
    cv2.imshow('dst', dst)
    key = cv2.waitKey(1)
    if key == 27: # exit on ESC
        break
    rval, img = vc.read()


vc.release()
cv2.destroyAllWindows()

# img = cv2.imread('left12.jpg')

# crop the image
# x, y, w, h = roi
# dst = dst[y:y+h, x:x+w]
# cv2.imwrite('calibresult.png', dst)