import numpy as np
import cv2
import argparse
import math
import sys
sys.path.insert(0, '../utils/')  # adding utils folder to the system path
import Syphon
import glfw
from pythonosc import udp_client, osc_message_builder, osc_bundle_builder


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


def sendContour(id, perimeter, area, circularity, solidity, addr='/thermal/leaf'):
    # send OSC Message representing analysis of a leaf contour, in the form of:
    # ["/leaf", id, perimeter, area, circularity, solidity, addr]
    msg = osc_message_builder.OscMessageBuilder(address=addr)
    msg.add_arg(id, arg_type='i')
    msg.add_arg(perimeter, arg_type='f')
    msg.add_arg(area, arg_type='f')
    msg.add_arg(circularity, arg_type='f')
    msg.add_arg(solidity, arg_type='f')
    oscClient.send(msg.build())


def sendContours(bundle_list, addr='/thermal/leaf'):
    # send OSC Bundle with OSC messages representing all contour bounding boxes:
    # ["/swarm/contour", id, area, x, y, w, h]
    bundle = osc_bundle_builder.OscBundleBuilder(osc_bundle_builder.IMMEDIATELY)
    for b in bundle_list:
        id, perimeter, area, circularity, solidity = b
        msg = osc_message_builder.OscMessageBuilder(address=addr)
        msg.add_arg(id, arg_type='i')
        msg.add_arg(perimeter, arg_type='f')
        msg.add_arg(area, arg_type='f')
        msg.add_arg(circularity, arg_type='f')
        msg.add_arg(solidity, arg_type='f')
        bundle.add_content(msg.build())

    oscClient.send(bundle.build())


def main():
    DIMENSIONS = (320, 240)

    # load image, convert to gray and scale down
    img = loadImg('captures/20220715-193723.jpg', gray=True)

    # ==== Syphon setup details ====
    # Syphon.Server("window and syphon server name", frame size, show)
    syphon_thermalcv_server = Syphon.Server("ServerThermalCV", DIMENSIONS, show=False)

    # the roi variables
    x, y, w, h = args.roi  # -r 80 60 190 160 -t 140
    # remove areas outside of roi
    img[0:int(y), 0:int(x)] = np.zeros((int(y), int(x)), dtype=np.uint8)  # top left
    img[0:int(y), img.shape[1]-int(x):img.shape[1]] = np.zeros((int(y), int(x)), dtype=np.uint8)  # top right
    img[img.shape[0]-int(y):img.shape[0], img.shape[1]-int(x):img.shape[1]] = np.zeros((int(y), int(x)), dtype=np.uint8)  # bot right
    img[img.shape[0]-int(y):img.shape[0], 0:int(x)] = np.zeros((int(y), int(x)), dtype=np.uint8)  # bot left

    # blur & threshold
    imgBlur = cv2.medianBlur(img, 5)
    ret, thresh = cv2.threshold(imgBlur, int(args.threshold), 255, cv2.THRESH_BINARY)

    # find Contours
    contours, hierarchy = cv2.findContours(thresh.copy(), cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)

    out = np.zeros_like(thresh)

    leaves_bundle = []

    # draw the contours
    for i in range(len(contours)):
        # -1 in 4th column means it's an external contour
        if hierarchy[0][i][3] == -1:
            perimeter = cv2.arcLength(contours[i], True)
            area = cv2.contourArea(contours[i])
            circularity = 4*math.pi*(area/(perimeter*perimeter))
            hull = cv2.convexHull(contours[i], False)
            hullArea = cv2.contourArea(hull)
            solidity = area / hullArea
            if(args.bundle):
                leaves_bundle.append([i, perimeter, area, circularity, solidity])
            else:
                sendContour(i, perimeter, area, circularity, solidity)
            x, y, w, h = cv2.boundingRect(contours[i])
            cv2.drawContours(out, contours, i, (204, 204, 204), 3)
            cv2.putText(out, str(i), (x, y - 1), cv2.FONT_HERSHEY_PLAIN, 1, (255, 255, 0), 1)
            print("contour " + str(i) + ":")
            print("  permimeter:" + str(perimeter))
            print("  area:" + str(area))
            print("  circularity:" + str(circularity))
            print("  solidity:" + str(solidity))
            print(" ")
            print("---------------------------------")
            print(" ")

    if(args.bundle):
        sendContours(leaves_bundle)

    cv2.imshow("Image orig", loadImg('captures/20220715-193723.jpg'))
    cv2.imshow("Contours", out)
    cv2.imshow("Image", img)
    # draw frame using opengl and send it to syphon
    out2 = cv2.cvtColor(out, cv2.COLOR_GRAY2RGB)
    syphon_thermalcv_server.draw_and_send(out2)

    while True:
        key = cv2.waitKey(1) & 0xFF
        if key == 27:
            break

    glfw.terminate()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--threshold", default="127", help="The cutoff for the threshold algorithm (0-255)")
    parser.add_argument("-r", "--roi", required=True, nargs="+", help="the x/y and width/height of the roi")
    parser.add_argument("--ip", default="127.0.0.1", help="The ip of the OSC server")
    parser.add_argument("--port", type=int, default=5005, help="The port the OSC server is listening on")
    parser.add_argument("-b", "--bundle", action="store_true", help="Send contours as an OSC Bundle (instead of individual OSC Messages)")
    args = parser.parse_args()
    # OSC
    oscClient = udp_client.UDPClient(args.ip, args.port)
    print(" ")
    sys.exit(main())
