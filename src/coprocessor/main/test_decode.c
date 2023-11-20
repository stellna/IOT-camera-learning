#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <esp_err.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <esp_log.h>
#include <driver/sdmmc_host.h>
#include <driver/sdmmc_defs.h>
#include <sdmmc_cmd.h>
#include <sys/stat.h>
#include <esp_jpg_decode.h>
#include <jpeg_decoder.h>

const char *TAG = "test_decode";

#define IMAGE_H 320
#define IMAGE_W 240
#define input_path "/sdcard/pic_1.jpg"

/* function prototype */

static FILE* __open_file(char *file_name);
static uint32_t  __get_file_size(FILE *file);
static uint8_t* __read_file(FILE *file, uint32_t size);
static void __init_pixels(uint8_t **pixels);
static void convert_rgb_to_bgr(uint8_t *pixels, size_t width, size_t height);
static void write_cpp_header(const uint8_t *pixels, size_t width, size_t height);

static FILE* __open_file(char *file_name) {
    return fopen(file_name, "r"); 
}

static uint32_t __get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    uint32_t size = ftell(file);
    rewind(file);
    return size;
}

static uint8_t* __read_file(FILE *file, uint32_t size) {
    uint8_t *buffer = malloc(size);
    fread(buffer, 1, size, file);
    fclose(file);
    return buffer;
}

static void __init_pixels(uint8_t **pixels) {
    *pixels = NULL;
    *pixels = calloc(IMAGE_H * IMAGE_W, sizeof(uint8_t) * 3);

    if ((*pixels) == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for pixels");
        return;
    }
}
void convert_rgb_to_bgr(uint8_t *pixels, size_t width, size_t height) {
    for (size_t i = 0; i < width * height; ++i) {
        uint8_t *pixel = pixels + i * 3;
        uint8_t temp = pixel[0]; // Temporarily store the red value
        pixel[0] = pixel[2];    // Swap red and blue
        pixel[2] = temp;        // Store the temporary value in blue
    }
}

static void write_cpp_header(const uint8_t *pixels, size_t width, size_t height) {
    const char *file_name = "/sdcard/image.hpp";
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    fprintf(file, "#pragma once\n#include <stdint.h>\n\n");
    fprintf(file, "#define IMAGE_HEIGHT %zu\n", height);
    fprintf(file, "#define IMAGE_WIDTH %zu\n", width);
    fprintf(file, "#define IMAGE_CHANNEL 3\n\n");
    fprintf(file, "const static uint8_t IMAGE_DATA[] = {\n    ");

    for (size_t i = 0; i < width * height * 3; ++i) {
        fprintf(file, "%d,", pixels[i]);
        if ((i + 1) % 32 == 0) { // Change 32 to control how many values per line
            fprintf(file, "\n    ");
        }
    }

    fprintf(file, "\n};\n");
    fclose(file);
    ESP_LOGI(TAG, "Header file saved as main/include/image.hpp");
}

void test_decode_task(uint8_t **pixels) {
    FILE *file = __open_file(input_path);
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open image file");
        return;
    }
    
    uint32_t size = __get_file_size(file);
    uint8_t *buffer = __read_file(file, size);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to read image file");
        return;
    }

    __init_pixels(pixels);
    if (*pixels == NULL) {
        free(buffer);
        return;
    }

    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)buffer,
        .indata_size = size,
        .outbuf = (uint8_t *)(*pixels),
        .outbuf_size = IMAGE_H * IMAGE_W * sizeof(uint8_t) * 3,
        .out_format = JPEG_IMAGE_FORMAT_RGB888,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 1
        }
    };

    esp_jpeg_image_output_t outimg;
    esp_err_t err = esp_jpeg_decode(&jpeg_cfg, &outimg);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Decoded image size: %dpx x %dpx", outimg.width, outimg.height);
        convert_rgb_to_bgr(*pixels, outimg.width, outimg.height);
        write_cpp_header(*pixels, outimg.width, outimg.height);
        printf("Write finished\n");
    } 
    else {
        ESP_LOGE(TAG, "JPEG Decode Failed");
        free(buffer);
        free(*pixels);
        *pixels = NULL;
        return;
    }

    free(buffer);
}