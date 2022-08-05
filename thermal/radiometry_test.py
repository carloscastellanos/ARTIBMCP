#!/usr/bin/env python
# -*- coding: utf-8 -*-

# based upon code by Andrew Jones & GroupGets:
# https://github.com/aytey/purethermal1-uvc-capture/blob/72ecd1d1ee9afe087fdb51ec2fe8f093f6356f62/python/uvc-radiometry.py#L361

from uvctypes import *
import cv2
import time
import os
import argparse
import numpy as np
import sys
sys.path.insert(0, '../utils/')
import Syphon
import glfw
try:
    from queue import Queue
except ImportError:
    from Queue import Queue

BUF_SIZE = 2
q = Queue(BUF_SIZE)


def py_frame_callback(frame, userptr):
    array_pointer = cast(frame.contents.data, POINTER(c_uint16 * (frame.contents.width * frame.contents.height)))
    data = np.frombuffer(array_pointer.contents, dtype=np.dtype(np.uint16)).reshape(frame.contents.height, frame.contents.width) # no copy

    # data = np.fromiter(
    #   frame.contents.data, dtype=np.dtype(np.uint8), count=frame.contents.data_bytes
    # ).reshape(
    #   frame.contents.height, frame.contents.width, 2
    # ) # copy

    if frame.contents.data_bytes != (2 * frame.contents.width * frame.contents.height):
        return

    if not q.full():
        q.put(data)


PTR_PY_FRAME_CALLBACK = CFUNCTYPE(None, POINTER(uvc_frame), c_void_p)(py_frame_callback)


def generate_color_map():
    """
    Conversion of the color map from GetThermal to a numpy LUT:
        https://github.com/groupgets/GetThermal/blob/bb467924750a686cc3930f7e3a253818b755a2c0/src/dataformatter.cpp#L6
    """

    lut = np.zeros((256, 1, 3), dtype=np.uint8)

    colormap_ironblack = [
        255, 255, 255, 253, 253, 253, 251, 251, 251, 249, 249, 249, 247, 247,
        247, 245, 245, 245, 243, 243, 243, 241, 241, 241, 239, 239, 239, 237,
        237, 237, 235, 235, 235, 233, 233, 233, 231, 231, 231, 229, 229, 229,
        227, 227, 227, 225, 225, 225, 223, 223, 223, 221, 221, 221, 219, 219,
        219, 217, 217, 217, 215, 215, 215, 213, 213, 213, 211, 211, 211, 209,
        209, 209, 207, 207, 207, 205, 205, 205, 203, 203, 203, 201, 201, 201,
        199, 199, 199, 197, 197, 197, 195, 195, 195, 193, 193, 193, 191, 191,
        191, 189, 189, 189, 187, 187, 187, 185, 185, 185, 183, 183, 183, 181,
        181, 181, 179, 179, 179, 177, 177, 177, 175, 175, 175, 173, 173, 173,
        171, 171, 171, 169, 169, 169, 167, 167, 167, 165, 165, 165, 163, 163,
        163, 161, 161, 161, 159, 159, 159, 157, 157, 157, 155, 155, 155, 153,
        153, 153, 151, 151, 151, 149, 149, 149, 147, 147, 147, 145, 145, 145,
        143, 143, 143, 141, 141, 141, 139, 139, 139, 137, 137, 137, 135, 135,
        135, 133, 133, 133, 131, 131, 131, 129, 129, 129, 126, 126, 126, 124,
        124, 124, 122, 122, 122, 120, 120, 120, 118, 118, 118, 116, 116, 116,
        114, 114, 114, 112, 112, 112, 110, 110, 110, 108, 108, 108, 106, 106,
        106, 104, 104, 104, 102, 102, 102, 100, 100, 100, 98, 98, 98, 96, 96,
        96, 94, 94, 94, 92, 92, 92, 90, 90, 90, 88, 88, 88, 86, 86, 86, 84, 84,
        84, 82, 82, 82, 80, 80, 80, 78, 78, 78, 76, 76, 76, 74, 74, 74, 72, 72,
        72, 70, 70, 70, 68, 68, 68, 66, 66, 66, 64, 64, 64, 62, 62, 62, 60, 60,
        60, 58, 58, 58, 56, 56, 56, 54, 54, 54, 52, 52, 52, 50, 50, 50, 48, 48,
        48, 46, 46, 46, 44, 44, 44, 42, 42, 42, 40, 40, 40, 38, 38, 38, 36, 36,
        36, 34, 34, 34, 32, 32, 32, 30, 30, 30, 28, 28, 28, 26, 26, 26, 24, 24,
        24, 22, 22, 22, 20, 20, 20, 18, 18, 18, 16, 16, 16, 14, 14, 14, 12, 12,
        12, 10, 10, 10, 8, 8, 8, 6, 6, 6, 4, 4, 4, 2, 2, 2, 0, 0, 0, 0, 0, 9,
        2, 0, 16, 4, 0, 24, 6, 0, 31, 8, 0, 38, 10, 0, 45, 12, 0, 53, 14, 0,
        60, 17, 0, 67, 19, 0, 74, 21, 0, 82, 23, 0, 89, 25, 0, 96, 27, 0, 103,
        29, 0, 111, 31, 0, 118, 36, 0, 120, 41, 0, 121, 46, 0, 122, 51, 0, 123,
        56, 0, 124, 61, 0, 125, 66, 0, 126, 71, 0, 127, 76, 1, 128, 81, 1, 129,
        86, 1, 130, 91, 1, 131, 96, 1, 132, 101, 1, 133, 106, 1, 134, 111, 1,
        135, 116, 1, 136, 121, 1, 136, 125, 2, 137, 130, 2, 137, 135, 3, 137,
        139, 3, 138, 144, 3, 138, 149, 4, 138, 153, 4, 139, 158, 5, 139, 163,
        5, 139, 167, 5, 140, 172, 6, 140, 177, 6, 140, 181, 7, 141, 186, 7,
        141, 189, 10, 137, 191, 13, 132, 194, 16, 127, 196, 19, 121, 198, 22,
        116, 200, 25, 111, 203, 28, 106, 205, 31, 101, 207, 34, 95, 209, 37,
        90, 212, 40, 85, 214, 43, 80, 216, 46, 75, 218, 49, 69, 221, 52, 64,
        223, 55, 59, 224, 57, 49, 225, 60, 47, 226, 64, 44, 227, 67, 42, 228,
        71, 39, 229, 74, 37, 230, 78, 34, 231, 81, 32, 231, 85, 29, 232, 88,
        27, 233, 92, 24, 234, 95, 22, 235, 99, 19, 236, 102, 17, 237, 106, 14,
        238, 109, 12, 239, 112, 12, 240, 116, 12, 240, 119, 12, 241, 123, 12,
        241, 127, 12, 242, 130, 12, 242, 134, 12, 243, 138, 12, 243, 141, 13,
        244, 145, 13, 244, 149, 13, 245, 152, 13, 245, 156, 13, 246, 160, 13,
        246, 163, 13, 247, 167, 13, 247, 171, 13, 248, 175, 14, 248, 178, 15,
        249, 182, 16, 249, 185, 18, 250, 189, 19, 250, 192, 20, 251, 196, 21,
        251, 199, 22, 252, 203, 23, 252, 206, 24, 253, 210, 25, 253, 213, 27,
        254, 217, 28, 254, 220, 29, 255, 224, 30, 255, 227, 39, 255, 229, 53,
        255, 231, 67, 255, 233, 81, 255, 234, 95, 255, 236, 109, 255, 238, 123,
        255, 240, 137, 255, 242, 151, 255, 244, 165, 255, 246, 179, 255, 248,
        193, 255, 249, 207, 255, 251, 221, 255, 253, 235, 255, 255, 24]

    def chunk(
        ulist, step): return map(lambda i: ulist[i: i + step], range(0, len(ulist), step))

    chunks = chunk(colormap_ironblack, 3)

    red = []
    green = []
    blue = []

    for chunk in chunks:
        red.append(chunk[0])
        green.append(chunk[1])
        blue.append(chunk[2])

    lut[:, 0, 0] = blue

    lut[:, 0, 1] = green

    lut[:, 0, 2] = red

    return lut


def ktof(val):
    return (1.8 * ktoc(val) + 32.0)


def ktoc(val):
    return (val - 27315) / 100.0


def ftok(val):
    return (((val-32) * 0.5555555556) * 100) + 27315


def ctok(val):
    return (val * 100.0) + 27315


def raw_to_8bit(data):
    cv2.normalize(data, data, 0, 65535, cv2.NORM_MINMAX)
    np.right_shift(data, 8, data)
    return cv2.cvtColor(np.uint8(data), cv2.COLOR_GRAY2RGB)


def display_temperature_f(img, val_k, loc, color):
    val = ktof(val_k)
    cv2.putText(img,"{0:.1f} degF".format(val), loc, cv2.FONT_HERSHEY_SIMPLEX, 0.75, color, 2)
    x, y = loc
    cv2.line(img, (x - 2, y), (x + 2, y), color, 1)
    cv2.line(img, (x, y - 2), (x, y + 2), color, 1)


def display_temperature_c(img, val_k, loc, color):
    val = ktoc(val_k)
    cv2.putText(img,"{0:.1f} degF".format(val), loc, cv2.FONT_HERSHEY_SIMPLEX, 0.75, color, 2)
    x, y = loc
    cv2.line(img, (x - 2, y), (x + 2, y), color, 1)
    cv2.line(img, (x, y - 2), (x, y + 2), color, 1)


def main():
    prevTime = 0  # will store last time capture time updated
    INTERVAL = 900  # 15 minutes

    DIMENSIONS = (320, 240)

    ctx = POINTER(uvc_context)()
    dev = POINTER(uvc_device)()
    devh = POINTER(uvc_device_handle)()
    ctrl = uvc_stream_ctrl()

    # ==== Syphon setup details ====
    # adding utils folder to the system path
    syphon_size = DIMENSIONS
    # Syphon.Server("window and syphon server name", frame size, show)
    syphon_thermal_server = Syphon.Server("ServerThermal", syphon_size, show=False)

    res = libuvc.uvc_init(byref(ctx), 0)
    if res < 0:
        print("uvc_init error")
        exit(1)

    try:
        res = libuvc.uvc_find_device(ctx, byref(dev), PT_USB_VID, PT_USB_PID, 0)
        if res < 0:
            print("uvc_find_device error")
            exit(1)

        try:
            res = libuvc.uvc_open(dev, byref(devh))
            if res < 0:
                print("uvc_open error")
                exit(1)

            print("device opened!")

            print_device_info(devh)
            print_device_formats(devh)

            frame_formats = uvc_get_frame_formats_by_guid(devh, VS_FMT_GUID_Y16)
            if len(frame_formats) == 0:
                print("device does not support Y16")
                exit(1)

            libuvc.uvc_get_stream_ctrl_format_size(devh, byref(ctrl), UVC_FRAME_FORMAT_Y16, frame_formats[0].wWidth, frame_formats[0].wHeight, int(1e7 / frame_formats[0].dwDefaultFrameInterval))

            res = libuvc.uvc_start_streaming(devh, byref(ctrl), PTR_PY_FRAME_CALLBACK, None, 0)
            if res < 0:
                print("uvc_start_streaming failed: {0}".format(res))
                exit(1)


            output_dir = "captures"
            color_map = generate_color_map()

            #
            # Min/max value to pin across the LUT
            #
            min_c = ctok(args.range[0])
            max_c = ctok(args.range[1])

            try:
                while True:
                    # get a frame from the queue
                    data = q.get(True, 500)
                    if data is None:
                        break

                    data = cv2.resize(data[:, :], DIMENSIONS)  # resize
                    minVal, maxVal, minLoc, maxLoc = cv2.minMaxLoc(data)

                    # Dirty-hack to ensure that the LUT is always scaled
                    # against the colors we care about
                    data[0][0] = min_c
                    data[-1][-1] = max_c
                    img = raw_to_8bit(data)
                    if(args.color):
                        img = cv2.LUT(img, color_map)

                    timestr = time.strftime("%Y%m%d-%H%M%S")

                    # Max/min values in the top-left
                    temp_str = "range(" + "{:.2f}, {:.2f}".format(ktoc(minVal), ktoc(maxVal)) + ")"

                    if(args.showtemps):
                        font = cv2.FONT_HERSHEY_PLAIN
                        cv2.putText(img, temp_str, (10, 230), font, 0.8, (255, 255, 255), 1, cv2.LINE_AA)

                    # time-lapse
                    if(args.timelapse):
                        currTime = time.time()
                        if(currTime-prevTime >= INTERVAL):
                            prevTime = currTime
                            cv2.imwrite(os.path.join(output_dir, "{:s}.jpg".format(timestr)), img)
                        # time.sleep(900)  # 900 = 15 mins

                    # display_temperature_c(img, min_c, minLoc, (255, 0, 0))
                    # display_temperature_c(img, max_c, maxLoc, (0, 0, 255))

                    # draw frame using opengl and send it to syphon
                    cv2.imshow('Lepton Radiometry', img)
                    if(args.color):
                        img_color_fix = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
                        syphon_thermal_server.draw_and_send(img_color_fix)
                    else:
                        syphon_thermal_server.draw_and_send(img)
                    cv2.waitKey(1)

                glfw.terminate()
                cv2.destroyAllWindows()
                exit()
            finally:
                libuvc.uvc_stop_streaming(devh)

            print("done")
        finally:
            libuvc.uvc_unref_device(dev)
    finally:
        libuvc.uvc_exit(ctx)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-r", "--range", nargs="*", default=[20, 40], type=float, help="The temperature range to look for")
    parser.add_argument("-c", "--color", action="store_true", help="use grayscale or a color LUT")
    parser.add_argument("-t", "--timelapse", action="store_true", help="timelapse images option")
    parser.add_argument("-s", "--showtemps", action="store_true", help="show temperature ranges")
    args = parser.parse_args()
    main()
