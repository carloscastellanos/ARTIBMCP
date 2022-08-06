import sys
import os
import time
import logging
import cv2
import Syphon
import glfw
import gphoto2 as gp


def main():
    # use Python logging
    logging.basicConfig(
        format='%(levelname)s: %(name)s: %(message)s', level=logging.WARNING)
    callback_obj = gp.check_result(gp.use_python_logging())

    # open camera connection
    camera = gp.Camera()
    camera.init()

    # get configuration tree
    config = camera.get_config()

    # find the capture target config item
    capture_target = gp.check_result(gp.gp_widget_get_child_by_name(config, 'capturetarget'))

    # set value to 1 (sd card)
    value = gp.check_result(gp.gp_widget_get_choice(capture_target, 1))
    gp.check_result(gp.gp_widget_set_value(capture_target, value))
    # set config
    camera.set_config(config)
    
    # clean up
    camera.exit()
    return 0


if __name__ == "__main__":
    sys.exit(main())
