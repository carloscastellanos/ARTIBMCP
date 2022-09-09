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


def main():
    # load image
    img = loadImg('captures/dr5spray-seedlings-03.jpg')

    # target color (RGB)
    gfp = [0, 139, 0]

    # interval that covers the values in the tuple and are below and above them
    rrange = 5
    grange = 116
    brange = 10

    # since opencv loads images in BGR format, the color values need to be adjusted:
    boundaries = [([gfp[2], gfp[1]-grange, gfp[0]], [gfp[2]+brange, gfp[1]+grange, gfp[0]+rrange])]

    # for each range in the boundary list:
    for (lower, upper) in boundaries:
        # get the lower and upper part of the interval:
        lower = np.array(lower, dtype=np.uint8)
        upper = np.array(upper, dtype=np.uint8)

        # cv2.inRange is used to binarize the image
        # All the pixels that fall inside the interval [lower, upper] will be white
        # All the pixels that do not fall inside this interval will be black (for all three channels):
        mask = cv2.inRange(img, lower, upper)

        # Now, we AND the mask and the input image
        # All the pixels that are white in the mask will survive the AND operation,
        # all the black pixels will remain black
        andMask = cv2.bitwise_and(img, img, mask=mask)

        # You can use the mask to count the number of white pixels.
        # Remember that the white pixels in the mask are those that
        # fall in the defined range, that is, every white pixel corresponds
        # to a green pixel. Divide by the image size and you got the
        # percentage of green pixels in the original image:
        ratio_green = cv2.countNonZero(mask)/(img.size/3)

        # This is the color percent calculation
        colorPercent = (ratio_green * 100)

        # Print the color percent, use 2 figures past the decimal point
        print('gfp pixel percentage:', np.round(colorPercent, 2))

    cv2.imshow("img", img)
    cv2.imshow("binary mask", mask)
    cv2.imshow("ANDed mask", andMask)
    # numpy's hstack is used to stack two images horizontally,
    # so you see the various images generated in one figure:
    cv2.imshow("images", np.hstack([img, andMask]))

    while True:
        key = cv2.waitKey(1) & 0xFF
        if key == 27:
            break

    cv2.destroyAllWindows()


if __name__ == "__main__":
    sys.exit(main())
