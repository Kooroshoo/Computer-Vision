#include <math.h>
#include "image.h"


float nn_interpolate(image im, float x, float y, int c)
{
    // Find the nearest integer coordinates
    int nearest_x = round(x);
    int nearest_y = round(y);

    // Clamp coordinates to ensure they are within the image boundaries
    nearest_x = (int)fmax(0, fmin(round(x), im.w - 1));
    nearest_y = (int)fmax(0, fmin(round(y), im.h - 1));

    // Retrieve the pixel value at the nearest integer coordinates from the image
    return get_pixel(im, nearest_x, nearest_y, c);
}

image nn_resize(image im, int w, int h)
{
    // Create a new image with dimensions w x h and the same number of channels as im
    image resized = make_image(w, h, im.c);

    // Calculate scaling factors for width and height
    float width_scale = (float)im.w / w;
    float height_scale = (float)im.h / h;

    // Iterate through each pixel in the resized image
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Calculate the corresponding coordinates in the original image using nearest neighbor
            int nearest_x = round(x * width_scale);
            int nearest_y = round(y * height_scale);

            // Get the nearest neighbor pixel value from the original image
            for (int c = 0; c < im.c; ++c) {
                float val = get_pixel(im, nearest_x, nearest_y, c);
                set_pixel(resized, x, y, c, val);
            }
        }
    }

    return resized;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // Calculate floor and ceiling values for x and y
    int x_floor = (int)floor(x);
    int y_floor = (int)floor(y);
    int x_ceil = x_floor + 1;
    int y_ceil = y_floor + 1;

    // Calculate the differences between the coordinates and their floor values
    float dx = x - x_floor;
    float dy = y - y_floor;

    // Retrieve pixel values of the four nearest neighbors
    float q11 = get_pixel(im, x_floor, y_floor, c);
    float q12 = get_pixel(im, x_floor, y_ceil, c);
    float q21 = get_pixel(im, x_ceil, y_floor, c);
    float q22 = get_pixel(im, x_ceil, y_ceil, c);

    // Perform bilinear interpolation calculation
    float top = (1 - dx) * q11 + dx * q21;
    float bottom = (1 - dx) * q12 + dx * q22;
    return (1 - dy) * top + dy * bottom;
}

image bilinear_resize(image im, int w, int h)
{
    // Create a new image with dimensions w x h and the same number of channels as im
    image resized = make_image(w, h, im.c);

    // Calculate scaling factors for width and height
    float width_scale = (float)(im.w - 1) / w;
    float height_scale = (float)(im.h - 1) / h;

    // Iterate over each pixel in the resized image
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Iterate over each channel in the image
            for (int c = 0; c < im.c; ++c) {
                // Map coordinates to the original image using scaling factors
                float mapped_x = x * width_scale;
                float mapped_y = y * height_scale;

                // Perform bilinear interpolation and set the pixel value in the resized image
                float val = bilinear_interpolate(im, mapped_x, mapped_y, c);
                set_pixel(resized, x, y, c, val);
            }
        }
    }
    return resized;
}

