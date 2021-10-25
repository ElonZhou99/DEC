//
//  hk_camera.hpp
//  DEC_BOLT_INSPECTION
//
//  Created by ZhouWeijie on 2021/9/9.
//  email: elonzhou99@gmail.com
//

#include "hk_camera.hpp"

HikCamera::HikCamera()
{
    nRet = MV_OK;
    //*pData = NULL;
    //MV_FRAME_OUT_INFO_EX pstImageInfo = {0};
    stConvertParam = {0};
    bConvertRet = false;
}

HikCamera::~HikCamera()
{
    stop_grab_image();
    close_device();
    shutdown();
}


void HikCamera::start(float gain)
{
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
    {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
        shutdown();
        return;
    }
    if (stDeviceList.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO *pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                shutdown();
                return;
            }
            PrintDeviceInfo(pDeviceInfo);
        }
    }
    else
    {
        printf("Find No Devices!\n");
        shutdown();
        return;
    }
    printf("Please Intput camera index: ");
    unsigned int nIndex = 0;
    //scanf("%d", &nIndex);
    if (nIndex >= stDeviceList.nDeviceNum)
    {
        printf("Intput error!\n");
        shutdown();
        return;
    }
    
    
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
        shutdown();
        return;
    }
    unsigned int nAccessMode;
    unsigned short nSwitchoverKey;
    
    nRet = MV_CC_OpenDevice(handle, nAccessMode, nSwitchoverKey);
    if (MV_OK != nRet)
    {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
        shutdown();
        return;
    }
    
    // Detection network optimal package size(It only works for the GigE camera)
    if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
        if (nPacketSize > 0)
        {
            nRet = MV_CC_SetIntValue(handle, "GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK)
            {
                printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
            }
        }
        else
        {
            printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
        }
    }

    nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0);
    if (MV_OK != nRet)
    {
        printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
        shutdown();
        return;
    }
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
    if (MV_OK != nRet)
    {
        printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
        shutdown();
        return;
    }
    
    
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
        shutdown();
        return;
    }
    
    nRet = MV_CC_SetFloatValue(handle, "Gain", gain);
}

bool HikCamera::grab_image()
{
    //MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    pData = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
    if (NULL == pData)
    {
        shutdown();
        return false;
    }
    unsigned int nDataSize = stParam.nCurValue;
    nRet = MV_CC_GetOneFrameTimeout(handle, pData, nDataSize, &stImageInfo, 500);
    if (MV_OK != nRet)
    {
        printf("MV_CC_GetOneFrameTimeout fail! nRet [%x]\n", nRet);
        shutdown();
        return false;
    }
    printf("Now you GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n\n",
           stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);

    return true;
}

bool HikCamera::get_one_frame()
{
    unsigned int nDataSize = stParam.nCurValue;
    nRet = MV_CC_GetOneFrameTimeout(handle, pData, nDataSize, &stImageInfo, 500);
    if (MV_OK != nRet)
    {
        printf("MV_CC_GetOneFrameTimeout fail! nRet [%x]\n", nRet);
        shutdown();
        return false;
    }
    printf("Now you GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n\n",
           stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
    return true;
}

cv::Mat HikCamera::convert2Mat(cv::Mat srcImage)
{
    if (stImageInfo.enPixelType == PixelType_Gvsp_Mono8)
    {
        srcImage = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pData);
    }
    else
    {
        printf("faild to conver2Mat");
    }
    if (NULL == srcImage.data)
    {
        return srcImage;
    }
    return srcImage;
}

bool HikCamera::PrintDeviceInfo(MV_CC_DEVICE_INFO *pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("Not support.\n");
    }
    return true;
}

void HikCamera::stop_grab_image()
{
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
    }
}

void HikCamera::close_device()
{
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        printf("ClosDevice fail! nRet [0x%x]\n", nRet);
    }
}

void HikCamera::shutdown()
{
    if (handle != NULL)
    {
        MV_CC_DestroyHandle(handle);
        handle = NULL;
    }
    if (pData)
    {
        free(pData);
        pData = NULL;
    }

    printf("exit\n");
    return;
}
