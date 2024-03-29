import cv2
import numpy as np
from flirpy.camera.lepton import Lepton


with Lepton() as camera:
    camera.setup_video(0)
    while True:
        img = camera.grab().astype(np.float32)

        # Rescale to 8 bit
        img = 255*(img - img.min())/(img.max()-img.min())

        # Apply colourmap - try COLORMAP_JET if INFERNO doesn't work.
        # You can also try PLASMA or MAGMA
        img_col = cv2.applyColorMap(img.astype(np.uint8), cv2.COLORMAP_INFERNO)

        cv2.imshow('Lepton', img_col)
        if cv2.waitKey(1) == 27:
            break  # esc to quit

cv2.destroyAllWindows()

# camera = Lepton()
# # camera.setup_video(1)
# image = camera.grab()
# camera.close()
