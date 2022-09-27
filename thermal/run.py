"""
1. initialize uvc thermal camera
2. initialize OSC client
4. initialize Syphon
5. load ml model
6. grab image from camera and save it
7. resize image to propoer size for ml model
8. show image to model and get result (e.g. cluster id and response message)
9. perform contour analysis
10. perfrom luminosity analysis
11. send image to Max via Syphom
12. send response message, contour analysis and luminosity analysis via OSC
"""
