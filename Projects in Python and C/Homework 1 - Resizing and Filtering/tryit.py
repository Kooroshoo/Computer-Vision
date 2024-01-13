from uwimg import *

# Image resizing

im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*4, im.h*4)
save_image(a, "result/dog4x-nn")

im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "result/dog4x-bl")

# Image filtering with convolutions

im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "result/dog-box7")

im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "result/dogthumb")

im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "result/dog-gauss2")

# Hybrid images
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "result/low-frequency")
save_image(hfreq, "result/high-frequency")
save_image(reconstruct, "result/reconstruct")

# Sobel filters
im = load_image("data/dog.jpg")
res = sobel_image(im)
mag = res[0]
feature_normalize(mag)
save_image(mag, "result/magnitude")

im = load_image("data/dog.jpg")
res = colorize_sobel(im)
save_image(res, "result/colorized") 