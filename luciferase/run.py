"""
1. initialize OSC client
2. initialize dslr camera
3. initialize Syphon
4. load ml model
5. grab image from camera and save it
6. resize image to proper size for ml model
7. show image to model and get result (e.g. cluster id and response message)
8. perform contour analysis
9. perfrom luminosity analysis
10. send image to Max via Syphon
11. send response message, contour analysis and luminosity analysis via OSC
"""

# -*- coding: utf-8 -*-
import argparse
import sys
import os
import time
import logging
import argparse
import cv2
import numpy as np
import gphoto2 as gp
from pythonosc import udp_client, osc_message_builder, osc_bundle_builder

sys.path.insert(0, "../utils/")  # adding utils folder to the system path
import Syphon
import glfw

try:
    from queue import Queue
except ImportError:
    from Queue import Queue


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


def sendContour(id, perimeter, area, circularity, solidity, addr="/thermal/leaf"):
    # send OSC Message representing analysis of a leaf contour, in the form of:
    # ["/leaf", id, perimeter, area, circularity, solidity, addr]
    msg = osc_message_builder.OscMessageBuilder(address=addr)
    msg.add_arg(id, arg_type="i")
    msg.add_arg(perimeter, arg_type="f")
    msg.add_arg(area, arg_type="f")
    msg.add_arg(circularity, arg_type="f")
    msg.add_arg(solidity, arg_type="f")
    oscClient.send(msg.build())


def sendContours(bundle_list):
    # send OSC Bundle with OSC messages representing all contour bounding boxes:
    # ["/swarm/contour", id, area, x, y, w, h]
    bundle = osc_bundle_builder.OscBundleBuilder(osc_bundle_builder.IMMEDIATELY)
    for b in bundle_list:
        addr, id, perimeter, area, circularity, solidity = b
        msg = osc_message_builder.OscMessageBuilder(address=addr)
        msg.add_arg(id, arg_type="i")
        msg.add_arg(perimeter, arg_type="f")
        msg.add_arg(area, arg_type="f")
        msg.add_arg(circularity, arg_type="f")
        msg.add_arg(solidity, arg_type="f")
        bundle.add_content(msg.build())

    oscClient.send(bundle.build())


def setupCamera(target="Memory card"):
    # open camera connection
    camera = gp.Camera()
    camera.init()

    # get configuration tree
    config = camera.get_config()

    # find the capture target config item
    capture_target = config.get_child_by_name("capturetarget")

    # set value to Memory card (default) or Internal RAM
    # value = capture_target.get_value()
    capture_target.set_value(target)
    # set config
    camera.set_config(config)

    return camera


def main():
    prevTime = 0
    INTERVAL = args.interval  # default = 15 minutes = 900 seconds
    DIMENSIONS = (600, 400)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-t",
        "--threshold",
        default="127",
        help="The cutoff for the threshold algorithm (0-255)",
    )
    parser.add_argument(
        "-r",
        "--roi",
        required=True,
        nargs="+",
        help="the x/y and width/height of the roi",
    )
    parser.add_argument("--ip", default="127.0.0.1", help="The ip of the OSC server")
    parser.add_argument(
        "--port", type=int, default=5005, help="The port the OSC server is listening on"
    )
    parser.add_argument(
        "-b",
        "--bundle",
        action="store_true",
        help="Send contours as an OSC Bundle (instead of individual OSC Messages)",
    )
    parser.add_argument(
        "-i",
        "--interval",
        type=int,
        required=False,
        default=900,
        help="timelapse interval (default=900 seconds (15 minutes))",
    )
    args = parser.parse_args()
    # OSC
    oscClient = udp_client.UDPClient(args.ip, args.port)
    print(" ")
    sys.exit(main())
