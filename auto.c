#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OUT_NAME_LABEL  "<name>OOCOutputFile</name>"   // 输出文件名标签
#define OUT_FILE_SUFFIX ".bin"                         // 输出文件后缀

/**
 * @brief  自动更新工程输出文件版本号
 * @note
 * @param  source: 版本文件路径
 * @param  target: 工程文件路径
 * @param  key_1: 工程名键
 * @param  key_2: 版本号键
 * @retval None
 */
void UpVersion_Project(char* source, char* target, char* key_1, char* key_2) {
    char buf[1024];
    char project[100] = {0};
    char version[100] = {0};
    getcwd(buf, sizeof(buf));
    strcat(buf, "\\");
    strcat(buf, source);
    FILE* fp;
    FILE* fp_temp;
    fp = fopen(buf, "r");
    if (fp == NULL)
        return;
    /* 遍历文件获取工程名和版本号 */
    while (!feof(fp) && ((*project == '\0') || (*version == '\0'))) {
        *buf = '\0';
        fgets(buf, sizeof(buf), fp);
        if (*project == '\0') {
            char* poi = strstr(buf, key_1);
            if (poi != NULL) {
                poi = strchr(poi, '\"') + 1;
                strncat(project, poi, strchr(poi, '\"') - poi);
            }
        }
        if (*version == '\0') {
            char* poi = strstr(buf, key_2);
            if (poi != NULL) {
                poi = strchr(poi, '\"') + 1;
                strncat(version, poi, strchr(poi, '\"') - poi);
            }
        }
    }
    fclose(fp);
    if ((*project == '\0') || (*version == '\0'))
        return;
    getcwd(buf, sizeof(buf));   // 获取工作地址
    strcat(buf, "\\");          //
    strcat(buf, target);        // 获取目标文件地址
    fp = fopen(buf, "r+");      // 打开目标文件
    if (fp == NULL)             // 打开失败
        return;
    getcwd(buf, sizeof(buf));     // 获取工作地址
    strcat(buf, "\\tmp");         //
    fp_temp = fopen(buf, "w+");   // 创建临时文件
    while (!feof(fp)) {
        *buf = '\0';
        fgets(buf, sizeof(buf), fp);
        fwrite(buf, strlen(buf), 1, fp_temp);
        char* poi = strstr(buf, OUT_NAME_LABEL);   //
        if (poi == NULL)                           // 未读取到关键字
            continue;
        fgets(buf, sizeof(buf), fp);             // 再读取一行
        char* start = strstr(poi, "<state>");    // 跳转到值
        char* end   = strstr(poi, "</state>");   // 跳转到值
        if (start == NULL || end == NULL)        // 未读取到关键字
            continue;
        fwrite(buf, poi - buf + 7, 1, fp_temp);                         // 写入前序数据
        fwrite(project, strlen(project), 1, fp_temp);                   // 写入工程名
        fwrite(" ", 1, 1, fp_temp);                                     // 写入连接符
        fwrite(version, strlen(version), 1, fp_temp);                   // 写入版本号
        fwrite(OUT_FILE_SUFFIX, strlen(OUT_FILE_SUFFIX), 1, fp_temp);   // 写入版本号
        fwrite(end, strlen(end), 1, fp_temp);                           // 后续数据
    }
    fseek(fp, 0, SEEK_SET);
    fseek(fp_temp, 0, SEEK_SET);
    ftruncate(fileno(fp), 0);
    while (!feof(fp_temp)) {
        *buf = '\0';
        fgets(buf, sizeof(buf), fp_temp);
        fwrite(buf, strlen(buf), 1, fp);
    }
    fclose(fp);
    fclose(fp_temp);
    getcwd(buf, sizeof(buf));   // 获取工作地址
    strcat(buf, "\\tmp");       //
    remove(buf);                // 删除临时文件
}
/**
 * @brief  入口函数
 * @note
 * @param  argc: 输入参数数量
 * @param  argv: 输入参数
 * @retval
 */
int main(int argc, char** argv) {
    if (argc < 2)
        return 1;
    switch (*argv[1]) {
        case 'p':
            if (argc == 6) {
                UpVersion_Project(argv[2], argv[3], argv[4], argv[5]);
            } else {
                return 1;
            }
            break;
        case 'v':
            break;
    }
    return 0;
}
