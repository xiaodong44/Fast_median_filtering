#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int test()
{
	return 2;
}
// 前面两个函数的代码在这里...

/**
 * @brief 生成测试图像（添加椒盐噪声）
 */
void generate_test_image(unsigned char *image, int width, int height, float noise_ratio) {
    // 生成渐变背景
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image[y * width + x] = (x + y) % 256;
        }
    }
    
    // 添加椒盐噪声
    int noise_pixels = (int)(width * height * noise_ratio);
    for (int i = 0; i < noise_pixels; i++) {
        int x = rand() % width;
        int y = rand() % height;
        
        // 50%概率为椒噪声（黑色），50%概率为盐噪声（白色）
        if (rand() % 2 == 0) {
            image[y * width + x] = 0;    // 椒噪声
        } else {
            image[y * width + x] = 255;  // 盐噪声
        }
    }
}

/**
 * @brief 保存PGM图像文件（便于查看结果）
 */
void save_pgm(const char *filename, unsigned char *image, int width, int height) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("无法创建文件：%s\n", filename);
        return;
    }
    
    fprintf(fp, "P5\n%d %d\n255\n", width, height);
    fwrite(image, 1, width * height, fp);
    fclose(fp);
}

/**
 * @brief 计算信噪比（PSNR）
 */
double calculate_psnr(unsigned char *original, unsigned char *filtered, int size) {
    double mse = 0.0;
    for (int i = 0; i < size; i++) {
        double diff = (double)original[i] - (double)filtered[i];
        mse += diff * diff;
    }
    mse /= size;
    
    if (mse == 0) return 100.0; // 完全一致
    
    return 20.0 * log10(255.0 / sqrt(mse));
}

int main() {
    srand((unsigned int)time(NULL));
    
    const int width = 512;
    const int height = 512;
    const int window_size = 3; // 3x3窗口
    
    // 分配内存
    unsigned char *original = (unsigned char*)malloc(width * height);
    unsigned char *noisy = (unsigned char*)malloc(width * height);
    unsigned char *filtered_basic = (unsigned char*)malloc(width * height);
    unsigned char *filtered_optimized = (unsigned char*)malloc(width * height);
    
    // 生成测试图像
    generate_test_image(original, width, height, 0.0); // 无噪声的原图
    generate_test_image(noisy, width, height, 0.05);   // 添加5%椒盐噪声
    
    printf("图像大小：%dx%d\n", width, height);
    printf("滤波窗口：%dx%d\n", window_size, window_size);
    
    // 测试基本版本
    clock_t start = clock();
    median_filter(noisy, filtered_basic, width, height, window_size);
    clock_t end = clock();
    printf("基本版本耗时：%.3f秒\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    // 测试优化版本
    start = clock();
    median_filter_optimized(noisy, filtered_optimized, width, height, window_size);
    end = clock();
    printf("优化版本耗时：%.3f秒\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    // 计算PSNR
    double psnr_basic = calculate_psnr(original, filtered_basic, width * height);
    double psnr_optimized = calculate_psnr(original, filtered_optimized, width * height);
    
    printf("基本版本PSNR：%.2f dB\n", psnr_basic);
    printf("优化版本PSNR：%.2f dB\n", psnr_optimized);
    
    // 保存结果（可选）
    save_pgm("noisy.pgm", noisy, width, height);
    save_pgm("filtered_basic.pgm", filtered_basic, width, height);
    save_pgm("filtered_optimized.pgm", filtered_optimized, width, height);
    printf("结果已保存到PGM文件\n");
    
    // 释放内存
    free(original);
    free(noisy);
    free(filtered_basic);
    free(filtered_optimized);
    
    return 0;
}
