/******************/
/*****������Ϣ*****/
/******************/

/*
 * �汾��˵��
 * [����ֶ�][��汾].[�׶ΰ汾].[�޶��汾]+[����汾] [�׶ζ���]
 * ����ֶ�     V �����汾
 * ��汾       �໥�����ݵİ汾���´��ֶ�
 * �׶ΰ汾     �����������������´��ֶ�
 * �޶��汾     ���ܲ��䣬�Բ����Ƚ����޸ĵ�������´��ֶ�
 * ����汾     ��ĳ���汾�޸�BUG����԰��޸Ĵ��ֶ�
 * �׶ζ���     Alpha ��һ�׶�(�����汾)
 *              Beta �ڶ��׶�(���԰汾)
 *              ������Ϊ���а汾
 */

#define PROJECT_NAME   "Integrate"   // ��������
#define SYSTEM_VERSION "BL V1.05"    // ϵͳ�汾
/* �豸���� BH1801 */
const uint16_t DeviceType @(CHIP_APP_START_ADDRESS + FIRMWARE_INFO_DEVICE_OFFSET) = 9000;
/* ����汾�� */
const char SystemVersion[24] @(CHIP_APP_START_ADDRESS + FIRMWARE_INFO_VERSION_OFFSET) = SYSTEM_VERSION;
/*������־ÿ�����47���ַ�,��Ҫ����16��*/
const char UpdateLog[IAP_INFO_COL][IAP_INFO_SIZE] @(CHIP_APP_START_ADDRESS + FIRMWARE_INFO_LOG_OFFSET) = {
    "1.�����°������ײ�����",
    "2.����HTTP����",
};
