#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"


#define TWOPI 6.2831853
#define EXP 2.718


void l1_normalize(image im)
{
    float sum = 0;

    // Calculate the sum of all pixel values in the image
    for (int c = 0; c < im.c; ++c) {
        for (int y = 0; y < im.h; ++y) {
            for (int x = 0; x < im.w; ++x) {
                sum += get_pixel(im, x, y, c);
            }
        }
    }

    // Normalize each pixel value by dividing it by the total sum
    for (int c = 0; c < im.c; ++c) {
        for (int y = 0; y < im.h; ++y) {
            for (int x = 0; x < im.w; ++x) {
                float normalized_value = get_pixel(im, x, y, c) / sum;
                set_pixel(im, x, y, c, normalized_value);
            }
        }
    }
}

image make_box_filter(int w)
{
    // Create a new image for the box filter with dimensions w x w and one channel
    image filter = make_image(w, w, 1);

    // Calculate the uniform value for each entry in the box filter
    float uniform_value = 1.0 / (w * w);

    // Set each entry in the box filter to the uniform value
    for (int y = 0; y < w; ++y) {
        for (int x = 0; x < w; ++x) {
            set_pixel(filter, x, y, 0, uniform_value);
        }
    }

    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    float c_step = 0;

    if (preserve == 1)
        c_step = im.c;
    else
        c_step = 1;
    
    image convolve_img = make_image(im.w, im.h, c_step);

    for (int c = 0; c < c_step; c++)
    {
        for (int y = 0; y < im.h; y++)
        {
            for (int x = 0; x < im.w; x++)
            {
                int step = filter.h;
                float result = 0;
                for (int i = -step / 2; i < step / 2 + 1; i++)
                {
                    for (int j = -step / 2; j < step / 2 + 1; j++)
                    {
                        float filter_val = get_pixel(filter,j + step / 2, i + step / 2, c);
                        float im_val = get_pixel(im, x + j, y + i, c);
                        result += (filter_val * im_val);
                    }
                }
                set_pixel(convolve_img, x, y, c, result);
            }
        }
    }
    
    return convolve_img;
}

image make_highpass_filter()
{
    image high_filter = make_image(3, 3, 1);

    high_filter.data[1] = -1;
    high_filter.data[3] = -1;
    high_filter.data[4] = 4;
    high_filter.data[5] = -1;
    high_filter.data[7] = -1;
    
    return high_filter;
}

image make_sharpen_filter()
{
    image sharpen_filter = make_image(3, 3, 1);

    sharpen_filter.data[1] = -1;
    sharpen_filter.data[3] = -1;
    sharpen_filter.data[4] = 5;
    sharpen_filter.data[5] = -1;
    sharpen_filter.data[7] = -1;

    return sharpen_filter;
}

image make_emboss_filter()
{
    image emboss_filter = make_image(3, 3, 1);

    emboss_filter.data[0] = -2;
    emboss_filter.data[1] = -1;
    emboss_filter.data[3] = -1;
    emboss_filter.data[4] = 1;
    emboss_filter.data[5] = 1;
    emboss_filter.data[7] = 1;
    emboss_filter.data[8] = 2;

    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    int filter_size = sigma * 6;
    if(filter_size % 2 == 0){
        filter_size += 1;
    }
    image filter = make_box_filter(filter_size);
    
    for (int j = 0; j < filter.w; j++){
        for (int k = 0; k < filter.h; k++){
            float gx = j - 3*sigma;
            float gy = k - 3*sigma; 
            float gaussian_value = (1.0/(TWOPI*powf(sigma,2))) * powf(EXP, -1.0*(powf(gx,2)+powf(gy,2))/(2*powf(sigma,2)));

            set_pixel(filter, j, k, 0, gaussian_value);
        }

    }
    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result_image = make_image(a.w, a.h, a.c); 
    
    for (int j = 0; j < a.w; j++){
        for (int k = 0; k < a.h; k++){
            for (int l = 0; l < a.c; l++){
                float result_value = get_pixel(a, j, k, l) + get_pixel(b, j, k, l);
                set_pixel(result_image, j, k, l, result_value);
            }
        }
    }
    return result_image;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result_image = make_image(a.w, a.h, a.c); 
    
    for (int j = 0; j < a.w; j++){
        for (int k = 0; k < a.h; k++){
            for (int l = 0; l < a.c; l++){
                float result_value = get_pixel(a, j, k, l) - get_pixel(b, j, k, l);
                set_pixel(result_image, j, k, l, result_value);
            }
        }
    }
    return result_image;
}

image make_gx_filter()
{
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, -1);
    set_pixel(im, 1, 0, 0, 0);
    set_pixel(im, 2, 0, 0, 1);
    set_pixel(im, 0, 1, 0, -2);
    set_pixel(im, 1, 1, 0, 0);
    set_pixel(im, 2, 1, 0, 2);
    set_pixel(im, 0, 2, 0, -1);
    set_pixel(im, 1, 2, 0, 0);
    set_pixel(im, 2, 2, 0, 1);
    return im;
}

image make_gy_filter()
{
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, -1);
    set_pixel(im, 1, 0, 0, -2);
    set_pixel(im, 2, 0, 0, -1);
    set_pixel(im, 0, 1, 0, 0);
    set_pixel(im, 1, 1, 0, 0);
    set_pixel(im, 2, 1, 0, 0);
    set_pixel(im, 0, 2, 0, 1);
    set_pixel(im, 1, 2, 0, 2);
    set_pixel(im, 2, 2, 0, 1);
    return im;
}

void feature_normalize(image im)
{
    float value;
    float min = INFINITY;
    float max = INFINITY * -1;
    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            for (int l = 0; l < im.c; l++){
                value =  get_pixel(im, j, k, l);
                min = (value < min) ? value : min;
                max = (value > max) ? value : max;
            }
        }
    }
    float range = max - min;
    float new_value;
    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            for (int l = 0; l < im.c; l++){
                if(range == 0){
                    set_pixel(im, j, k, l, 0.0);
                }
                else{
                    new_value = (get_pixel(im, j, k, l) - min) / range;
                    set_pixel(im, j, k, l, new_value);
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    image gx = make_gx_filter();
    image gy = make_gy_filter();
    image gx_filter_image = convolve_image(im, gx, 0);
    image gy_filter_image = convolve_image(im, gy, 0);

    image * sobel_image =  calloc(2, sizeof(image));
    sobel_image[0] = make_image(im.w, im.h, 1);
    sobel_image[1] = make_image(im.w, im.h, 1);

    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            float g = sqrtf(powf(get_pixel(gx_filter_image, j, k, 0), 2) + powf(get_pixel(gy_filter_image, j, k, 0), 2));
            set_pixel(sobel_image[0], j, k, 0, g);
            float theta = atan2f(get_pixel(gy_filter_image, j, k, 0), get_pixel(gx_filter_image, j, k, 0));
            set_pixel(sobel_image[1], j, k, 0, theta);
        }
    }
    return sobel_image;
}

image colorize_sobel(image im)
{
    image *sobel = sobel_image(im); // Compute Sobel gradients for the input image
    image mag = sobel[0]; // Magnitude
    image angle = sobel[1]; // Angle

    // Create an HSV image to work with hue, saturation, and value
    image hsv = make_image(im.w, im.h, 3);

    // Resize the magnitude image to match the input image size
    image resized_mag = nn_resize(mag, im.w, im.h); // or bilinear_resize

    // Modify the hue, saturation, and value based on Sobel gradients
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float saturation = resized_mag.data[y * im.w + x];
            float hue = 0.5 + 0.5 * angle.data[y * im.w + x] / (2 * M_PI); // Normalized hue
            float value = 0.5 + 0.5 * resized_mag.data[y * im.w + x]; // Normalized value

            // Update the HSV image with modified values
            set_pixel(hsv, x, y, 0, hue); // Update hue
            set_pixel(hsv, x, y, 1, saturation); // Update saturation
            set_pixel(hsv, x, y, 2, value); // Update value
        }
    }

    // Convert the HSV image to RGB color space using hsv_to_rgb function
    hsv_to_rgb(hsv);

    // Free allocated memory
    free_image(sobel[0]);
    free_image(sobel[1]);
    free(sobel);
    free_image(resized_mag);

    return hsv;
}
