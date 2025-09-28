/******************/
/*****升级信息*****/
/******************/

/*
 * 版本号说明
 * [标记字段][大版本].[阶段版本].[修订版本]+[补充版本] [阶段定义]
 * 标记字段     V 正常版本
 * 大版本       相互不兼容的版本更新此字段
 * 阶段版本     功能性新增调整更新此字段
 * 修订版本     功能不变，对参数等进行修改的情况更新此字段
 * 补充版本     在某个版本修复BUG或测试版修改此字段
 * 阶段定义     Alpha 第一阶段(开发版本)
 *              Beta 第二阶段(测试版本)
 *              留空则为发行版本
 */

#define PROJECT_NAME   "Integrate"   // 工程名称
#define SYSTEM_VERSION "BL V1.05"    // 系统版本
/* 设备类型 BH1801 */
const uint16_t DeviceType @(CHIP_APP_START_ADDRESS + FIRMWARE_INFO_DEVICE_OFFSET) = 9000;
/* 程序版本号 */
const char SystemVersion[24] @(CHIP_APP_START_ADDRESS + FIRMWARE_INFO_VERSION_OFFSET) = SYSTEM_VERSION;
/*升级日志每行最多47个字符,不要超过16行*/
const char UpdateLog[IAP_INFO_COL][IAP_INFO_SIZE] @(CHIP_APP_START_ADDRESS + FIRMWARE_INFO_LOG_OFFSET) = {
    "1.更新新版联网底层驱动",
    "2.新增HTTP驱动",
};
