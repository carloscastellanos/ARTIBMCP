# -*- coding: utf-8 -*-
'''
code based on:
https://stackoverflow.com/questions/66757199/color-percentage-in-image-for-python-using-opencv
'''

import cv2
import numpy as np
import sys


def loadImg(s, read_as_float32=False, gray=False):
    if read_as_float32:
        img = cv2.imread(s).astype(np.float32) / 255
    else:
        img = cv2.imread(s)
    if gray:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return img


def scaleImg(img, scaleFactor=0.5):
    width = int(img.shape[1] * scaleFactor)
    height = int(img.shape[0] * scaleFactor)
    return cv2.resize(img, (width, height), interpolation=cv2.INTER_AREA)


def main():
    # load image
    img = loadImg('captures/DSC00523.JPG')

    # Convert the image to HSV
    hsvImage = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # rough RGB ranges: 10, 11, 0 - 202, 175, 29
    # The HSV mask values, defined for the green color ranges
    lowerValues = np.array([38, 148, 16])
    upperValues = np.array([160, 255, 255])

    # Create the HSV mask
    # cv2.inRange is used to binarize the image
    # All the pixels that fall between lowerValues, upperValues will be white
    # All the pixels that do not fall inside this range will be black
    hsvMask = cv2.inRange(hsvImage, lowerValues, upperValues)

    # AND mask & input image:
    # All the pixels that are white in the mask will survive the AND operation,
    # all the black pixels will remain black
    hsvANDMask = cv2.bitwise_and(img, img, mask=hsvMask)

    # You can use the mask to count the number of white pixels.
    # Remember that the white pixels in the mask are those that
    # fall in the defined range, that is, every white pixel corresponds
    # to a green pixel. Divide by the image size and you got the
    # percentage of green pixels in the original image
    # note: img.size gives the number of pixels fro all 3 color channels combined
    ratio_green = cv2.countNonZero(hsvMask)/(img.size/3)

    # This is the color percent calculation
    colorPercent = (ratio_green * 100)

    # Print the color percent, use 2 figures past the decimal point
    print('gfp pixel percentage:', np.round(colorPercent, 2))

    cv2.imshow("img", img)
    cv2.imshow("binary mask", hsvMask)
    cv2.imshow("ANDed mask", hsvANDMask)
    # numpy's hstack is used to stack two images horizontally,
    # so you see the various images generated in one figure:
    cv2.imshow("images", np.hstack([img, hsvANDMask]))

    while True:
        key = cv2.waitKey(1) & 0xFF
        if key == 27:
            break

    cv2.destroyAllWindows()


if __name__ == "__main__":
    sys.exit(main())
