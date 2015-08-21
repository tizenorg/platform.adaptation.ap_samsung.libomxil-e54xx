/*
 * Copyright 2012 Samsung Electronics S.LSI Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * @file        Exynos_OSAL_Platform_Specific.c
 * @brief
 * @author      Seungbeom Kim (sbcrux.kim@samsung.com)
 * @author      Hyeyeon Chung (hyeon.chung@samsung.com)
 * @author      Yunji Kim (yunji.kim@samsung.com)
 * @author      Jinsung Yang (jsgood.yang@samsung.com)
 * @version     2.0.0
 * @history
 *   2012.02.20 : Create
 */

#include <stdio.h>
#include <stdlib.h>
#include <mm_types.h>

#include "Exynos_OSAL_Semaphore.h"
#include "Exynos_OMX_Baseport.h"
#include "Exynos_OMX_Basecomponent.h"
#include "Exynos_OMX_Macros.h"
#include "Exynos_OMX_Vdec.h"
#include "Exynos_OSAL_Platform_Specific.h"
#include "exynos_format.h"

#include "ExynosVideoApi.h"

#undef  EXYNOS_LOG_TAG
#define EXYNOS_LOG_TAG    "Exynos_OSAL_PB"
#define EXYNOS_LOG_OFF
#include "Exynos_OSAL_Log.h"

#ifdef __cplusplus
extern "C" {
#endif


OMX_ERRORTYPE Exynos_OSAL_LockPBHandle(
    OMX_IN OMX_U32 handle,
    OMX_IN OMX_U32 width,
    OMX_IN OMX_U32 height,
    OMX_IN OMX_COLOR_FORMATTYPE format,
    OMX_OUT OMX_PTR planes)
{
    FunctionIn();

    OMX_ERRORTYPE ret = OMX_ErrorNone;

#if 0
    GraphicBufferMapper &mapper = GraphicBufferMapper::get();
    buffer_handle_t bufferHandle = (buffer_handle_t) handle;
#ifdef USE_DMA_BUF
    private_handle_t *priv_hnd = (private_handle_t *) bufferHandle;
#endif
    Rect bounds(width, height);
    ExynosVideoPlane *vplanes = (ExynosVideoPlane *) planes;
    void *vaddr[MAX_BUFFER_PLANE];

    Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: handle: 0x%x", __func__, handle);

    int usage = 0;

    switch (format) {
    case OMX_COLOR_FormatYUV420Planar:
    case OMX_COLOR_FormatYUV420SemiPlanar:
    case OMX_SEC_COLOR_FormatNV12Tiled:
        usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_YUV_ADDR;
        break;
    default:
        usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN;
        break;
    }

    if (mapper.lock(bufferHandle, usage, bounds, vaddr) != 0) {
        Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: mapper.lock() fail", __func__);
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

#ifdef USE_DMA_BUF
    vplanes[0].fd = priv_hnd->fd;
    vplanes[0].offset = 0;
    vplanes[1].fd = priv_hnd->u_fd;
    vplanes[1].offset = priv_hnd->uoffset;
    vplanes[2].fd = priv_hnd->v_fd;
    vplanes[2].offset = priv_hnd->voffset;
#endif
    vplanes[0].addr = vaddr[0];
    vplanes[1].addr = vaddr[1];
    vplanes[2].addr = vaddr[2];

    Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: buffer locked: 0x%x", __func__, *vaddr);
#endif

EXIT:
    FunctionOut();

    return ret;
}

OMX_ERRORTYPE Exynos_OSAL_UnlockPBHandle(OMX_IN OMX_U32 handle)
{
    FunctionIn();

    OMX_ERRORTYPE ret = OMX_ErrorNone;
#ifdef SLP_PLATFORM
#else
    GraphicBufferMapper &mapper = GraphicBufferMapper::get();
    buffer_handle_t bufferHandle = (buffer_handle_t) handle;

    Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: handle: 0x%x", __func__, handle);

    if (mapper.unlock(bufferHandle) != 0) {
        Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: mapper.unlock() fail", __func__);
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

    Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: buffer unlocked: 0x%x", __func__, handle);
#endif

EXIT:
    FunctionOut();

    return ret;
}

OMX_ERRORTYPE Exynos_OSAL_LockPB(
    OMX_IN OMX_PTR pBuffer,
    OMX_IN OMX_U32 width,
    OMX_IN OMX_U32 height,
    OMX_IN OMX_COLOR_FORMATTYPE format,
    OMX_OUT OMX_U32 *pStride,
    OMX_OUT OMX_PTR planes)
{
    FunctionIn();

    OMX_ERRORTYPE ret = OMX_ErrorNone;
#ifdef SLP_PLATFORM

    ExynosVideoPlane *vplanes = (ExynosVideoPlane *) planes;
    MMVideoBuffer *buffer = (MMVideoBuffer *) pBuffer;


    vplanes[0].fd = buffer->handle.dmabuf_fd[0];
    vplanes[0].offset = 0;
    vplanes[1].fd = buffer->handle.dmabuf_fd[1];
    vplanes[1].offset = 0; //priv_hnd->uoffset;
    vplanes[2].fd = 0; //priv_hnd->v_fd;
    vplanes[2].offset = 0; //priv_hnd->voffset;

    vplanes[0].addr = buffer->data[0]; //vaddr[0];
    vplanes[1].addr = buffer->data[1]; //vaddr[1];
    vplanes[2].addr = NULL; //vaddr[2];

    Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "Exynos_OSAL_LockPB:fd[0](%d)  fd[1](%d)  a[0](%p)  a[1](%p)", 
        buffer->handle.dmabuf_fd[0], buffer->handle.dmabuf_fd[1], buffer->data[0], buffer->data[1]);
#else
    android_native_buffer_t *pANB = (android_native_buffer_t *) pBuffer;

    ret = Exynos_OSAL_LockPBHandle((OMX_U32)pANB->handle, width, height, format, planes);
    *pStride = pANB->stride;
#endif

EXIT:
    FunctionOut();

    return ret;
}

#ifdef SLP_PLATFORM
OMX_ERRORTYPE Exynos_OSAL_UnlockPB(OMX_IN OMX_PTR pBuffer, EXYNOS_OMX_DATA *pData, EXYNOS_OMX_BASEPORT *pExynosPort,EXYNOS_OMX_BASEPORT *pExynosInPort)
#else
OMX_ERRORTYPE Exynos_OSAL_UnlockPB(OMX_IN OMX_PTR pBuffer, EXYNOS_OMX_DATA *pData)
#endif
{
    FunctionIn();

    OMX_ERRORTYPE ret = OMX_ErrorNone;
#ifdef SLP_PLATFORM
    MMVideoBuffer *pSlpOutBuf = NULL;
    DECODE_CODEC_EXTRA_BUFFERINFO *pBufferInfo = NULL;

    pSlpOutBuf = (MMVideoBuffer *)pBuffer;
    if (pSlpOutBuf == NULL) {
      Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "pBuffer is NULL!");
      ret = OMX_ErrorInsufficientResources;
      goto EXIT;
    }
    memset(pSlpOutBuf, 0, sizeof(MMVideoBuffer));

    pBufferInfo = (DECODE_CODEC_EXTRA_BUFFERINFO *)pData->extInfo;

    if (pExynosPort->cropRectangle.nWidth != 0 && pExynosPort->cropRectangle.nHeight != 0) {
        /* modify for h264 trim */
        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "this has cropRectangle(h264).crop.nWidth = %d, crop.nHeight = %d",
            pExynosPort->cropRectangle.nWidth, pExynosPort->cropRectangle.nWidth);
        pSlpOutBuf->width[0] = pExynosPort->cropRectangle.nWidth;
        pSlpOutBuf->width[1] = pExynosPort->cropRectangle.nWidth;
        pSlpOutBuf->height[0] = pExynosPort->cropRectangle.nHeight;
        pSlpOutBuf->height[1] = pExynosPort->cropRectangle.nHeight/2;
    } else {
        pSlpOutBuf->width[0] = pBufferInfo->imageWidth;
        pSlpOutBuf->width[1] = pBufferInfo->imageWidth;
        pSlpOutBuf->height[0] = pBufferInfo->imageHeight;
        pSlpOutBuf->height[1] = pBufferInfo->imageHeight/2;
    }
    pSlpOutBuf->stride_width[0] = ALIGN(pBufferInfo->imageWidth, S5P_FIMV_NV12M_HALIGN);
    pSlpOutBuf->stride_width[1] = ALIGN(pBufferInfo->imageWidth, S5P_FIMV_NV12M_HALIGN);
    pSlpOutBuf->stride_height[0] = ALIGN(pBufferInfo->imageHeight, S5P_FIMV_NV12M_HALIGN);
    pSlpOutBuf->stride_height[1] = ALIGN(pBufferInfo->imageHeight/2, S5P_FIMV_NV12M_HALIGN);


/*
    if (pVideoDec->bDRMPlayerMode == OMX_TRUE) {
        pSlpOutBuf->a[0] = 0;
        pSlpOutBuf->a[1] = 0;
    } else {
*/
        pSlpOutBuf->data[0] = pData->buffer.multiPlaneBuffer.dataBuffer[0];
        pSlpOutBuf->data[1] = pData->buffer.multiPlaneBuffer.dataBuffer[1];
//    }
    pSlpOutBuf->data[2] = 0; /* omx do not use this plane */

    pSlpOutBuf->handle.dmabuf_fd[0] = pData->buffer.multiPlaneBuffer.fd[0];
    pSlpOutBuf->handle.dmabuf_fd[1] = pData->buffer.multiPlaneBuffer.fd[1];
    pSlpOutBuf->handle.dmabuf_fd[2] = 0;

    pSlpOutBuf->handle.bo[0] = pData->buffer.multiPlaneBuffer.tbm_bo[0];
    pSlpOutBuf->handle.bo[1] = pData->buffer.multiPlaneBuffer.tbm_bo[1];
    pSlpOutBuf->handle.bo[2] = NULL;

    if(pExynosInPort->portDefinition.format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        pSlpOutBuf->size[0] = CHOOSE_MAX_SIZE(calc_plane(pBufferInfo->imageWidth,pBufferInfo->imageHeight), new_calc_plane(pBufferInfo->imageWidth,pBufferInfo->imageHeight));
        pSlpOutBuf->size[1] = CHOOSE_MAX_SIZE(calc_plane(pBufferInfo->imageWidth,(pBufferInfo->imageHeight) / 2), new_calc_plane(pBufferInfo->imageWidth,(pBufferInfo->imageHeight) / 2));
        Exynos_OSAL_Log(EXYNOS_LOG_TRACE,"H264 foramt and y_size=%d, uv_size=%d",pSlpOutBuf->size[0],pSlpOutBuf->size[1]);
    } else {
        pSlpOutBuf->size[0]= CHOOSE_MAX_SIZE(calc_yplane(pBufferInfo->imageWidth,pBufferInfo->imageHeight), new_calc_yplane(pBufferInfo->imageWidth,pBufferInfo->imageHeight));
        pSlpOutBuf->size[1] = CHOOSE_MAX_SIZE(calc_uvplane(pBufferInfo->imageWidth,(pBufferInfo->imageHeight) / 2), new_calc_uvplane(pBufferInfo->imageWidth,(pBufferInfo->imageHeight) / 2));
        Exynos_OSAL_Log(EXYNOS_LOG_TRACE,"foramt is %d, and y_size=%d, uv_size=%d",pExynosInPort->portDefinition.format.video.eCompressionFormat ,
                        pSlpOutBuf->size[0],pSlpOutBuf->size[1]);
    }


    //pSlpOutBuf->type = 1; /* use fd mode */

    Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "fd (%d, %d, %d) received from MFC", pSlpOutBuf->handle.dmabuf_fd[0], pSlpOutBuf->handle.dmabuf_fd[1], pSlpOutBuf->handle.dmabuf_fd[2]);
#else
    android_native_buffer_t *pANB = (android_native_buffer_t *) pBuffer;

    ret = Exynos_OSAL_UnlockPBHandle((OMX_U32)pANB->handle);
#endif
EXIT:
    FunctionOut();

    return ret;
}

#if 0
OMX_ERRORTYPE useAndroidNativeBuffer(
    EXYNOS_OMX_BASEPORT      *pExynosPort,
    OMX_BUFFERHEADERTYPE **ppBufferHdr,
    OMX_U32                nPortIndex,
    OMX_PTR                pAppPrivate,
    OMX_U32                nSizeBytes,
    OMX_U8                *pBuffer)
{
    OMX_ERRORTYPE         ret = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *temp_bufferHeader = NULL;
    unsigned int          i = 0;
    OMX_U32               width, height;
    OMX_U32               stride;
    ExynosVideoPlane      planes[MAX_BUFFER_PLANE];

    FunctionIn();

    if (pExynosPort == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pExynosPort->portState != OMX_StateIdle) {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }
    if (CHECK_PORT_TUNNELED(pExynosPort) && CHECK_PORT_BUFFER_SUPPLIER(pExynosPort)) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    temp_bufferHeader = (OMX_BUFFERHEADERTYPE *)Exynos_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE));
    if (temp_bufferHeader == NULL) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    Exynos_OSAL_Memset(temp_bufferHeader, 0, sizeof(OMX_BUFFERHEADERTYPE));

    for (i = 0; i < pExynosPort->portDefinition.nBufferCountActual; i++) {
        if (pExynosPort->bufferStateAllocate[i] == BUFFER_STATE_FREE) {
            pExynosPort->extendBufferHeader[i].OMXBufferHeader = temp_bufferHeader;
            pExynosPort->bufferStateAllocate[i] = (BUFFER_STATE_ASSIGNED | HEADER_STATE_ALLOCATED);
            INIT_SET_SIZE_VERSION(temp_bufferHeader, OMX_BUFFERHEADERTYPE);
            temp_bufferHeader->pBuffer        = pBuffer;
            temp_bufferHeader->nAllocLen      = nSizeBytes;
            temp_bufferHeader->pAppPrivate    = pAppPrivate;
            if (nPortIndex == INPUT_PORT_INDEX)
                temp_bufferHeader->nInputPortIndex = INPUT_PORT_INDEX;
            else
                temp_bufferHeader->nOutputPortIndex = OUTPUT_PORT_INDEX;

            width = pExynosPort->portDefinition.format.video.nFrameWidth;
            height = pExynosPort->portDefinition.format.video.nFrameHeight;
            Exynos_OSAL_LockPB(temp_bufferHeader->pBuffer, width, height,
                                pExynosPort->portDefinition.format.video.eColorFormat,
                                &stride, planes);
#ifdef USE_DMA_BUF
            pExynosPort->extendBufferHeader[i].buf_fd[0] = planes[0].fd;
            pExynosPort->extendBufferHeader[i].buf_fd[1] = planes[1].fd;
            pExynosPort->extendBufferHeader[i].buf_fd[2] = planes[2].fd;
#endif
            pExynosPort->extendBufferHeader[i].pYUVBuf[0] = planes[0].addr;
            pExynosPort->extendBufferHeader[i].pYUVBuf[1] = planes[1].addr;
            pExynosPort->extendBufferHeader[i].pYUVBuf[2] = planes[2].addr;
            Exynos_OSAL_UnlockANB(temp_bufferHeader->pBuffer);
            Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "useAndroidNativeBuffer: buf %d pYUVBuf[0]:0x%x , pYUVBuf[1]:0x%x ",
                            i, pExynosPort->extendBufferHeader[i].pYUVBuf[0],
                            pExynosPort->extendBufferHeader[i].pYUVBuf[1]);

            pExynosPort->assignedBufferNum++;
            if (pExynosPort->assignedBufferNum == pExynosPort->portDefinition.nBufferCountActual) {
                pExynosPort->portDefinition.bPopulated = OMX_TRUE;
                /* Exynos_OSAL_MutexLock(pExynosComponent->compMutex); */
                Exynos_OSAL_SemaphorePost(pExynosPort->loadedResource);
                /* Exynos_OSAL_MutexUnlock(pExynosComponent->compMutex); */
            }
            *ppBufferHdr = temp_bufferHeader;
            ret = OMX_ErrorNone;

            goto EXIT;
        }
    }

    Exynos_OSAL_Free(temp_bufferHeader);
    ret = OMX_ErrorInsufficientResources;

EXIT:
    FunctionOut();

    return ret;
}
#endif

OMX_ERRORTYPE Exynos_OSAL_GetPBParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_INOUT OMX_PTR     ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    EXYNOS_OMX_BASECOMPONENT *pExynosComponent = NULL;

    FunctionIn();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = Exynos_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pExynosComponent = (EXYNOS_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (pExynosComponent->currentState == OMX_StateInvalid ) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if (ComponentParameterStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    switch (nIndex) {
#if 0 /* SLP_PLATFORM */
    case OMX_IndexParamGetAndroidNativeBuffer:
    {
        GetAndroidNativeBufferUsageParams *pANBParams = (GetAndroidNativeBufferUsageParams *) ComponentParameterStructure;
        OMX_U32 portIndex = pANBParams->nPortIndex;

        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: OMX_IndexParamGetAndroidNativeBuffer", __func__);

        ret = Exynos_OMX_Check_SizeVersion(pANBParams, sizeof(GetAndroidNativeBufferUsageParams));
        if (ret != OMX_ErrorNone) {
            Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Exynos_OMX_Check_SizeVersion(GetAndroidNativeBufferUsageParams) is failed", __func__);
            goto EXIT;
        }

        if (portIndex >= pExynosComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        /* NOTE: OMX_IndexParamGetAndroidNativeBuffer returns original 'nUsage' without any
         * modifications since currently not defined what the 'nUsage' is for.
         */
        pANBParams->nUsage |= (GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP
                           | GRALLOC_USAGE_HW_ION | GRALLOC_USAGE_HWC_HWOVERLAY);
    }
        break;
#endif
    default:
    {
        Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Unsupported index (%d)", __func__, nIndex);
        ret = OMX_ErrorUnsupportedIndex;
        goto EXIT;
    }
        break;
    }

EXIT:
    FunctionOut();

    return ret;
}

OMX_ERRORTYPE Exynos_OSAL_SetPBParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    EXYNOS_OMX_BASECOMPONENT *pExynosComponent = NULL;
    EXYNOS_OMX_VIDEODEC_COMPONENT *pVideoDec = NULL;

    FunctionIn();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = Exynos_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pExynosComponent = (EXYNOS_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (pExynosComponent->currentState == OMX_StateInvalid ) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if (ComponentParameterStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pVideoDec = (EXYNOS_OMX_VIDEODEC_COMPONENT *)pExynosComponent->hComponentHandle;

    switch (nIndex) {
#ifdef SLP_PLATFORM
    case OMX_IndexParamEnablePlatformSpecificBuffers:
#else
    case OMX_IndexParamEnableAndroidBuffers:
#endif
    {
        EnableGemBuffersParams *pPBParams = (EnableGemBuffersParams *) ComponentParameterStructure;
        OMX_U32 portIndex = pPBParams->nPortIndex;
        EXYNOS_OMX_BASEPORT *pExynosPort = NULL;

        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: OMX_IndexParamEnablePlatformSpecificBuffers", __func__);

        ret = Exynos_OMX_Check_SizeVersion(pPBParams, sizeof(EnableGemBuffersParams));
        if (ret != OMX_ErrorNone) {
            Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Exynos_OMX_Check_SizeVersion(EnableGemBuffersParams) is failed", __func__);
            goto EXIT;
        }

        if (portIndex >= pExynosComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pExynosPort = &pExynosComponent->pExynosPort[portIndex];
        if (CHECK_PORT_TUNNELED(pExynosPort) && CHECK_PORT_BUFFER_SUPPLIER(pExynosPort)) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

#ifdef SLP_PLATFORM
         /* PB and DPB Buffer Sharing */
        if ((portIndex == OUTPUT_PORT_INDEX) &&
            ((pExynosPort->bufferProcessType & BUFFER_PBSHARE) == BUFFER_PBSHARE)) {
            pExynosPort->bufferProcessType = BUFFER_SHARE;
            pExynosPort->portDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SEC_COLOR_FormatNV12Tiled;
            Exynos_OSAL_Log(EXYNOS_LOG_INFO, "output buffer sharing mode is on");
        }
        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "pExynosPort->portDefinition.format.video.eColorFormat: 0x%x", pExynosPort->portDefinition.format.video.eColorFormat);
#else
        if ((portIndex == OUTPUT_PORT_INDEX) &&
            (pExynosPort->bufferProcessType & BUFFER_COPY)) {
            pExynosPort->bufferProcessType = BUFFER_COPY;
            pExynosPort->portDefinition.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
        }
#endif
        pExynosPort->bIsPBEnabled = pPBParams->enable;
        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "pExynosPort->bIsPBEnabled: %d", pExynosPort->bIsPBEnabled);
    }
        break;

#if 0 /* SLP_PLATFORM */
    case OMX_IndexParamUseAndroidNativeBuffer:
    {
        UseAndroidNativeBufferParams *pANBParams = (UseAndroidNativeBufferParams *) ComponentParameterStructure;
        OMX_U32 portIndex = pANBParams->nPortIndex;
        EXYNOS_OMX_BASEPORT *pExynosPort = NULL;
        android_native_buffer_t *pANB;
        OMX_U32 nSizeBytes;

        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: OMX_IndexParamUseAndroidNativeBuffer, portIndex: %d", __func__, portIndex);

        ret = Exynos_OMX_Check_SizeVersion(pANBParams, sizeof(UseAndroidNativeBufferParams));
        if (ret != OMX_ErrorNone) {
            Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Exynos_OMX_Check_SizeVersion(UseAndroidNativeBufferParams) is failed", __func__);
            goto EXIT;
        }

        if (portIndex >= pExynosComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pExynosPort = &pExynosComponent->pExynosPort[portIndex];
        if (CHECK_PORT_TUNNELED(pExynosPort) && CHECK_PORT_BUFFER_SUPPLIER(pExynosPort)) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        if (pExynosPort->portState != OMX_StateIdle) {
            Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Port state should be IDLE", __func__);
            ret = OMX_ErrorIncorrectStateOperation;
            goto EXIT;
        }

        pANB = pANBParams->nativeBuffer.get();

        /* MALI alignment restriction */
        nSizeBytes = ALIGN(pANB->width, 16) * ALIGN(pANB->height, 16);
        nSizeBytes += ALIGN(pANB->width / 2, 16) * ALIGN(pANB->height / 2, 16) * 2;

        ret = useAndroidNativeBuffer(pExynosPort,
                                     pANBParams->bufferHeader,
                                     pANBParams->nPortIndex,
                                     pANBParams->pAppPrivate,
                                     nSizeBytes,
                                     (OMX_U8 *) pANB);
        if (ret != OMX_ErrorNone) {
            Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: useAndroidNativeBuffer is failed: err=0x%x", __func__,ret);
            goto EXIT;
        }
    }
        break;

    case OMX_IndexParamStoreMetaDataBuffer:
    {
        StoreMetaDataInBuffersParams *pANBParams = (StoreMetaDataInBuffersParams *) ComponentParameterStructure;
        OMX_U32 portIndex = pANBParams->nPortIndex;
        EXYNOS_OMX_BASEPORT *pExynosPort = NULL;

        Exynos_OSAL_Log(EXYNOS_LOG_TRACE, "%s: OMX_IndexParamStoreMetaDataBuffer", __func__);

        ret = Exynos_OMX_Check_SizeVersion(pANBParams, sizeof(StoreMetaDataInBuffersParams));
        if (ret != OMX_ErrorNone) {
            Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Exynos_OMX_Check_SizeVersion(StoreMetaDataInBuffersParams) is failed", __func__);
            goto EXIT;
        }

        if (portIndex >= pExynosComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pExynosPort = &pExynosComponent->pExynosPort[portIndex];
        if (CHECK_PORT_TUNNELED(pExynosPort) && CHECK_PORT_BUFFER_SUPPLIER(pExynosPort)) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pExynosPort->bStoreMetaData = pANBParams->bStoreMetaData;
    }
        break;
#endif
    default:
    {
        Exynos_OSAL_Log(EXYNOS_LOG_ERROR, "%s: Unsupported index (%d)", __func__, nIndex);
        ret = OMX_ErrorUnsupportedIndex;
        goto EXIT;
    }
        break;
    }

EXIT:
    FunctionOut();

    return ret;
}

#if 0 // we can remove this later.
OMX_ERRORTYPE Exynos_OSAL_GetInfoFromMetaData(OMX_IN OMX_BYTE pBuffer,
                                           OMX_OUT OMX_PTR *ppBuf)
{
    OMX_ERRORTYPE      ret = OMX_ErrorNone;
    MetadataBufferType type;

    FunctionIn();

/*
 * meta data contains the following data format.
 * payload depends on the MetadataBufferType
 * --------------------------------------------------------------
 * | MetadataBufferType                         |          payload                           |
 * --------------------------------------------------------------
 *
 * If MetadataBufferType is kMetadataBufferTypeCameraSource, then
 * --------------------------------------------------------------
 * | kMetadataBufferTypeCameraSource  | physical addr. of Y |physical addr. of CbCr |
 * --------------------------------------------------------------
 *
 * If MetadataBufferType is kMetadataBufferTypeGrallocSource, then
 * --------------------------------------------------------------
 * | kMetadataBufferTypeGrallocSource    | buffer_handle_t |
 * --------------------------------------------------------------
 */

    /* MetadataBufferType */
    Exynos_OSAL_Memcpy(&type, (MetadataBufferType *)pBuffer, sizeof(MetadataBufferType));

    if (type == kMetadataBufferTypeCameraSource) {
        void *pAddress = NULL;

        /* Address. of Y */
        Exynos_OSAL_Memcpy(&pAddress, pBuffer + sizeof(MetadataBufferType), sizeof(void *));
        ppBuf[0] = (void *)pAddress;

        /* Address. of CbCr */
        Exynos_OSAL_Memcpy(&pAddress, pBuffer + sizeof(MetadataBufferType) + sizeof(void *), sizeof(void *));
        ppBuf[1] = (void *)pAddress;

    } else if (type == kMetadataBufferTypeGrallocSource) {
        buffer_handle_t    pBufHandle;

        /* buffer_handle_t */
        Exynos_OSAL_Memcpy(&pBufHandle, pBuffer + sizeof(MetadataBufferType), sizeof(buffer_handle_t));
        ppBuf[0] = (OMX_PTR)pBufHandle;
    }

EXIT:
    FunctionOut();

    return ret;
}

OMX_COLOR_FORMATTYPE Exynos_OSAL_Hal2OMXPixelFormat(
    unsigned int hal_format)
{
    OMX_COLOR_FORMATTYPE omx_format;
    switch (hal_format) {
    case HAL_PIXEL_FORMAT_YCbCr_422_I:
        omx_format = OMX_COLOR_FormatYCbYCr;
        break;
    case HAL_PIXEL_FORMAT_YCbCr_420_P:
        omx_format = OMX_COLOR_FormatYUV420Planar;
        break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
        omx_format = OMX_COLOR_FormatYUV420SemiPlanar;
        break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED:
        omx_format = (OMX_COLOR_FORMATTYPE)OMX_SEC_COLOR_FormatNV12Tiled;
        break;
    case HAL_PIXEL_FORMAT_ARGB888:
        omx_format = OMX_COLOR_Format32bitARGB8888;
        break;
    default:
        omx_format = OMX_COLOR_FormatYUV420Planar;
        break;
    }
    return omx_format;
}

unsigned int Exynos_OSAL_OMX2HalPixelFormat(
    OMX_COLOR_FORMATTYPE omx_format)
{
    unsigned int hal_format;
    switch (omx_format) {
    case OMX_COLOR_FormatYCbYCr:
        hal_format = HAL_PIXEL_FORMAT_YCbCr_422_I;
        break;
    case OMX_COLOR_FormatYUV420Planar:
        hal_format = HAL_PIXEL_FORMAT_YCbCr_420_P;
        break;
    case OMX_COLOR_FormatYUV420SemiPlanar:
        hal_format = HAL_PIXEL_FORMAT_YCbCr_420_SP;
        break;
    case OMX_SEC_COLOR_FormatNV12Tiled:
        hal_format = HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED;
        break;
    case OMX_COLOR_Format32bitARGB8888:
        hal_format = HAL_PIXEL_FORMAT_ARGB888;
        break;
    default:
        hal_format = HAL_PIXEL_FORMAT_YCbCr_420_P;
        break;
    }
    return hal_format;
}
#endif

#ifdef __cplusplus
}
#endif
