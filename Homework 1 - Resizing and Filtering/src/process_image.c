#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

int clamp(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

float get_pixel(image im, int x, int y, int c)
{
    // Clamp the coordinates to ensure they are within the valid range
    x = clamp(x, 0, im.w - 1);
    y = clamp(y, 0, im.h - 1);
    c = clamp(c, 0, im.c - 1);

    // Calculate the index for the desired pixel in the data array
    int pixels_per_channel = im.h * im.w;                                   // Total number of pixels in a channel
    int channel_start = pixels_per_channel * c;                             // Starting index of the channel
    int row_offset = im.w * y;                                              // Offset for the row within the channel
    int pixel_offset = x;                                                   // Offset for the pixel within the row

    // Calculate the index within the data array
    int data_index = channel_start + row_offset + pixel_offset;

    // Retrieve the pixel value at the calculated index
    float pixel_value = im.data[data_index];

    return pixel_value;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // Check if the coordinates are within the valid range
    if (x < 0 || x >= im.w || y < 0 || y >= im.h || c < 0 || c >= im.c) {
        printf("Invalid coordinates or channel!\n");
        return; 
    }

    // Calculate the index for the desired pixel in the data array
    int pixels_per_channel = im.h * im.w;                                   // Total number of pixels in a single channel
    int channel_start = pixels_per_channel * c;                             // Starting index of the channel
    int row_offset = im.w * y;                                              // Offset for the row within the channel
    int pixel_offset = x;                                                   // Offset for the pixel within the row

    // Calculate the index within the data array
    int data_index = channel_start + row_offset + pixel_offset;

    // Set the pixel value at the calculated index to the provided value v
    im.data[data_index] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);

    memcpy(copy.data, im.data, im.c * im.h * im.w * sizeof(float));

    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);  // Ensure input image has 3 channels (RGB)

    image gray = make_image(im.w, im.h, 1);  // Create a grayscale image

    // Convert RGB to grayscale using luminance-preserving formula: Y = 0.299R + 0.587G + 0.114B
    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            float r = get_pixel(im, x, y, 0);  // Red channel
            float g = get_pixel(im, x, y, 1);  // Green channel
            float b = get_pixel(im, x, y, 2);  // Blue channel

            // Calculate luminance
            float luminance = 0.299 * r + 0.587 * g + 0.114 * b;

            // Set the grayscale pixel value
            set_pixel(gray, x, y, 0, luminance);
        }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    // Ensure the channel c is within the valid range of channels in the image
    assert(c >= 0 && c < im.c);

    // Iterate through each pixel of the specified channel and apply the shift
    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            // Get the current pixel value in the specified channel
            float current_value = get_pixel(im, x, y, c);

            // Add the shift value v to the pixel value
            float shifted_value = current_value + v;

            // Set the shifted pixel value back to the image
            set_pixel(im, x, y, c, shifted_value);
        }
    }
}

void clamp_image(image im)
{
    // Iterate through each pixel and clamp the pixel values within the range [0, 1]
    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            for (int c = 0; c < im.c; ++c) {
                // Get the current pixel value
                float current_value = get_pixel(im, x, y, c);

                // Clamp the pixel value within the range [0, 1]
                float clamped_value = (current_value < 0) ? 0 : ((current_value > 1) ? 1 : current_value);

                // Set the clamped pixel value back to the image
                set_pixel(im, x, y, c, clamped_value);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for (int i = 0; i < im.h; i++)
    {
        for (int j = 0; j < im.w; j++)
        {
            float R = get_pixel(im, j, i, 0);
            float G = get_pixel(im, j, i, 1);
            float B = get_pixel(im, j, i, 2);

            float V = three_way_max(R, G, B);
            float m = three_way_min(R, G, B);
            float C = V - m;

            float S = 0;
            if (V != 0)
                S = C / V;

            float H_ = 0;
            if (C != 0)
            {
                if (V == R)
                    H_ = (G - B) / C;
                else if (V == G)
                    H_ = ((B - R) / C) + 2;
                else if (V == B)
                    H_ = ((R - G) / C) + 4;
            }

            float H = 0;
            if (H_ < 0)
                H = (H_ / 6) + 1;
            else
                H = H_ / 6;

            set_pixel(im, j, i, 0, H);
            set_pixel(im, j, i, 1, S);
            set_pixel(im, j, i, 2, V);
        }
    } 
}

void hsv_to_rgb(image im)
{
    for (int i = 0; i < im.h; i++)
    {
        for (int j = 0; j < im.w; j++)
        {
            float H = get_pixel(im, j, i, 0);
            float S = get_pixel(im, j, i, 1);
            float V = get_pixel(im, j, i, 2);

            float C = S * V;
            float m = 0;
            if (V != C)
                m = V - C;
            float H_ = 6 * H;

            float R = V;
            float G = V;
            float B = V;

            if ((H_ > 0) && (H_ < 1))
            {
                R = V;
                B = m;
                G = H_ * C + m;
            }
            else if (H_ == 1)
            {
                R = V;
                G = V;
                B = m;
            }
            else if ((H_ > 1) && (H_ < 2))
            {
                G = V;
                B = m;
                R = (((H_ - 2) * C) - m) * -1;
            }
            else if (H_ == 2)
            {
                R = m;
                G = V;
                B = m;
            }
            else if ((H_ > 2) && (H_ < 3))
            {
                R = m;
                G = V;
                B = (H_ - 2) * C + m;
            }
            else if (H_ == 3)
            {
                R = m;
                G = V;
                B = V;
            }
            else if ((H_ > 3) && (H_ < 4))
            {
                R = m;
                B = V;
                G = (((H_ - 4) * C) - m) * -1;
            }
            else if (H_ == 4)
            {
                R = m;
                G = m;
                B = V;
            }
            else if ((H_ > 4) && (H_ < 5))
            {
                G = m;
                B = V;
                R = (H_ - 4) * C + m;
            }
            else if (H_ == 5)
            {
                R = V;
                G = m;
                B = V;
            }
            else if ((H_ > 5) && (H_ < 6))
            {
                R = V;
                B = m;
                G = ((((H_ / 6) - 1) * 6 * C) - m) * -1;
            }
            else
            {
                R = V;
                G = m;
                B = m;
            }
            set_pixel(im, j, i, 0, R);
            set_pixel(im, j, i, 1, G);
            set_pixel(im, j, i, 2, B);
        }
    }
}
