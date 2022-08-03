# ARTIBMCP

## Dependencies:
- [libuvc](https://github.com/libuvc/libuvc)
- [libgphoto2](https://github.com/gphoto/libgphoto2)
- [libusb](https://github.com/libusb/libusb)
- [PyOpenGL](http://pyopengl.sourceforge.net/)

Best installed via a package manager (e.g. Homebrew)

All python packages are best installed in a virtual environment (e.g. Anaconda). Example:
```
conda create -n [name of your environment] python=3.7 ipykernel
conda activate [name of your environment]
pip install -r requirements.txt
```

To access your virtual environment via Jupyter notebooks on your machine:
```
python -m ipykernel install --user --name [name of your environment]
```

Note: the syphonpy package works with the Syphon Framework, which is MacOS only
