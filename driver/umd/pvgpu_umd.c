#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef LONG NTSTATUS;
#include <d3d10umddi.h>
#include "pvgpu_umd.h"

static void PvgpuCreateShaderInternal(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ D3D10DDI_HSHADER hShader, _In_ PVGPU_SHADER_TYPE shaderType) {
    UNREFERENCED_PARAMETER(hDevice);
    UNREFERENCED_PARAMETER(pCode);
    UNREFERENCED_PARAMETER(hShader);
    UNREFERENCED_PARAMETER(shaderType);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpvReserved);
    return TRUE;
}

HRESULT APIENTRY OpenAdapter10_2(_Inout_ D3D10DDIARG_OPENADAPTER* pOpenData) {
    PVGPU_UMD_ADAPTER* pAdapter = (PVGPU_UMD_ADAPTER*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PVGPU_UMD_ADAPTER));
    pAdapter->MaxTextureWidth = 8192;
    pOpenData->hAdapter.pDrvPrivate = pAdapter;
    pOpenData->pAdapterFuncs->pfnCalcPrivateDeviceSize = PvgpuCalcPrivateDeviceSize;
    pOpenData->pAdapterFuncs->pfnCreateDevice = PvgpuCreateDevice;
    pOpenData->pAdapterFuncs->pfnCloseAdapter = PvgpuCloseAdapter;
    pOpenData->pAdapterFuncs_2->pfnGetCaps = PvgpuGetCaps;
    pOpenData->pAdapterFuncs_2->pfnGetSupportedVersions = PvgpuGetSupportedVersions;
    return S_OK;
}

SIZE_T APIENTRY PvgpuCalcPrivateDeviceSize(_In_ D3D10DDI_HADAPTER hAdapter, _In_ CONST D3D10DDIARG_CALCPRIVATEDEVICESIZE* pCalcPrivateDeviceSize) { return sizeof(PVGPU_UMD_DEVICE); }
HRESULT APIENTRY PvgpuCloseAdapter(_In_ D3D10DDI_HADAPTER hAdapter) { if (hAdapter.pDrvPrivate) HeapFree(GetProcessHeap(), 0, hAdapter.pDrvPrivate); return S_OK; }
HRESULT APIENTRY PvgpuGetCaps(_In_ D3D10DDI_HADAPTER hAdapter, _In_ CONST D3D10_2DDIARG_GETCAPS* pData) { return E_NOTIMPL; }
HRESULT APIENTRY PvgpuGetSupportedVersions(_In_ D3D10DDI_HADAPTER hAdapter, _Inout_ UINT32* puEntries, _Out_writes_opt_(*puEntries) UINT64* pSupportedDDIInterfaceVersions) {
    static const UINT64 v[] = { D3D10_1_DDI_INTERFACE_VERSION };
    if (pSupportedDDIInterfaceVersions == NULL) { *puEntries = 1; return S_OK; }
    pSupportedDDIInterfaceVersions[0] = v[0]; *puEntries = 1; return S_OK;
}

HRESULT APIENTRY PvgpuCreateDevice(_In_ D3D10DDI_HADAPTER hAdapter, _Inout_ D3D10DDIARG_CREATEDEVICE* pCreateData) {
    PVGPU_UMD_DEVICE* pDevice = (PVGPU_UMD_DEVICE*)pCreateData->hDrvDevice.pDrvPrivate;
    ZeroMemory(pDevice, sizeof(PVGPU_UMD_DEVICE));
    pDevice->pKTCallbacks = pCreateData->pKTCallbacks;

    D3D10DDI_DEVICEFUNCS* p = pCreateData->pDeviceFuncs;
    p->pfnDestroyDevice = PvgpuDestroyDevice;
    p->pfnFlush = PvgpuFlush;
    p->pfnCalcPrivateResourceSize = PvgpuCalcPrivateResourceSize;
    p->pfnCreateResource = PvgpuCreateResource;
    p->pfnDestroyResource = PvgpuDestroyResource;
    p->pfnOpenResource = PvgpuOpenResource;
    p->pfnCalcPrivateShaderSize = PvgpuCalcPrivateShaderSize;
    p->pfnCreateVertexShader = PvgpuCreateVertexShader;
    p->pfnCreatePixelShader = PvgpuCreatePixelShader;
    p->pfnCreateGeometryShader = PvgpuCreateGeometryShader;
    p->pfnDestroyShader = PvgpuDestroyShader;
    p->pfnIaSetInputLayout = PvgpuIaSetInputLayout;
    p->pfnIaSetVertexBuffers = PvgpuIaSetVertexBuffers;
    p->pfnIaSetIndexBuffer = PvgpuIaSetIndexBuffer;
    p->pfnIaSetTopology = PvgpuIaSetTopology;
    p->pfnVsSetShader = PvgpuVsSetShader;
    p->pfnPsSetShader = PvgpuPsSetShader;
    p->pfnGsSetShader = PvgpuGsSetShader;
    p->pfnSetRenderTargets = PvgpuSetRenderTargets;
    p->pfnSetBlendState = PvgpuSetBlendState;
    p->pfnSetDepthStencilState = PvgpuSetDepthStencilState;
    p->pfnSetViewports = PvgpuSetViewports;
    p->pfnSetScissorRects = PvgpuSetScissorRects;
    p->pfnSetRasterizerState = PvgpuSetRasterizerState;
    p->pfnDraw = PvgpuDraw;
    p->pfnDrawIndexed = PvgpuDrawIndexed;
    p->pfnDrawInstanced = PvgpuDrawInstanced;
    p->pfnDrawIndexedInstanced = PvgpuDrawIndexedInstanced;
    p->pfnDrawAuto = PvgpuDrawAuto;
    p->pfnClearRenderTargetView = PvgpuClearRenderTargetView;
    p->pfnClearDepthStencilView = PvgpuClearDepthStencilView;
    p->pfnResourceCopy = PvgpuResourceCopy;
    p->pfnResourceCopyRegion = PvgpuResourceCopyRegion;
    p->pfnResourceUpdateSubresourceUP = PvgpuResourceUpdateSubresourceUP;
    p->pfnResourceMap = PvgpuResourceMap;
    p->pfnResourceUnmap = PvgpuResourceUnmap;
    p->pfnCalcPrivateBlendStateSize = PvgpuCalcPrivateBlendStateSize;
    p->pfnCreateBlendState = PvgpuCreateBlendState;
    p->pfnDestroyBlendState = PvgpuDestroyBlendState;
    p->pfnCalcPrivateRasterizerStateSize = PvgpuCalcPrivateRasterizerStateSize;
    p->pfnCreateRasterizerState = PvgpuCreateRasterizerState;
    p->pfnDestroyRasterizerState = PvgpuDestroyRasterizerState;
    p->pfnCalcPrivateDepthStencilStateSize = PvgpuCalcPrivateDepthStencilStateSize;
    p->pfnCreateDepthStencilState = PvgpuCreateDepthStencilState;
    p->pfnDestroyDepthStencilState = PvgpuDestroyDepthStencilState;
    p->pfnCalcPrivateSamplerSize = PvgpuCalcPrivateSamplerSize;
    p->pfnCreateSampler = PvgpuCreateSampler;
    p->pfnDestroySampler = PvgpuDestroySampler;
    p->pfnCalcPrivateElementLayoutSize = PvgpuCalcPrivateElementLayoutSize;
    p->pfnCreateElementLayout = PvgpuCreateElementLayout;
    p->pfnDestroyElementLayout = PvgpuDestroyElementLayout;
    p->pfnCalcPrivateRenderTargetViewSize = PvgpuCalcPrivateRenderTargetViewSize;
    p->pfnCreateRenderTargetView = PvgpuCreateRenderTargetView;
    p->pfnDestroyRenderTargetView = PvgpuDestroyRenderTargetView;
    p->pfnCalcPrivateDepthStencilViewSize = PvgpuCalcPrivateDepthStencilViewSize;
    p->pfnCreateDepthStencilView = PvgpuCreateDepthStencilView;
    p->pfnDestroyDepthStencilView = PvgpuDestroyDepthStencilView;
    p->pfnCalcPrivateShaderResourceViewSize = PvgpuCalcPrivateShaderResourceViewSize;
    p->pfnCreateShaderResourceView = PvgpuCreateShaderResourceView;
    p->pfnDestroyShaderResourceView = PvgpuDestroyShaderResourceView;
    p->pfnVsSetConstantBuffers = PvgpuVsSetConstantBuffers;
    p->pfnPsSetConstantBuffers = PvgpuPsSetConstantBuffers;
    p->pfnGsSetConstantBuffers = PvgpuGsSetConstantBuffers;
    p->pfnVsSetShaderResources = PvgpuVsSetShaderResources;
    p->pfnPsSetShaderResources = PvgpuPsSetShaderResources;
    p->pfnGsSetShaderResources = PvgpuGsSetShaderResources;
    p->pfnVsSetSamplers = PvgpuVsSetSamplers;
    p->pfnPsSetSamplers = PvgpuPsSetSamplers;
    p->pfnGsSetSamplers = PvgpuGsSetSamplers;

    if (pCreateData->Interface >= D3D11_0_DDI_INTERFACE_VERSION) {
        D3D11DDI_DEVICEFUNCS* p11 = (D3D11DDI_DEVICEFUNCS*)p;
        p11->pfnCreateHullShader = PvgpuCreateHullShader;
        p11->pfnCreateDomainShader = PvgpuCreateDomainShader;
        p11->pfnHsSetShader = PvgpuHsSetShader;
        p11->pfnDsSetShader = PvgpuDsSetShader;
        p11->pfnHsSetConstantBuffers = PvgpuHsSetConstantBuffers;
        p11->pfnDsSetConstantBuffers = PvgpuDsSetConstantBuffers;
        p11->pfnHsSetShaderResources = PvgpuHsSetShaderResources;
        p11->pfnDsSetShaderResources = PvgpuDsSetShaderResources;
        p11->pfnHsSetSamplers = PvgpuHsSetSamplers;
        p11->pfnDsSetSamplers = PvgpuDsSetSamplers;
        p11->pfnCsSetShader = PvgpuCsSetShader;
        p11->pfnCsSetConstantBuffers = PvgpuCsSetConstantBuffers;
        p11->pfnCsSetShaderResources = PvgpuCsSetShaderResources;
        p11->pfnCsSetSamplers = PvgpuCsSetSamplers;
        p11->pfnCsSetUnorderedAccessViews = PvgpuCsSetUnorderedAccessViews;
        p11->pfnDispatch = PvgpuDispatch;
        p11->pfnDispatchIndirect = PvgpuDispatchIndirect;
        p11->pfnCalcPrivateUnorderedAccessViewSize = PvgpuCalcPrivateUnorderedAccessViewSize;
        p11->pfnCreateUnorderedAccessView = PvgpuCreateUnorderedAccessView;
        p11->pfnDestroyUnorderedAccessView = PvgpuDestroyUnorderedAccessView;
    }
    return S_OK;
}

void APIENTRY PvgpuDestroyDevice(_In_ D3D10DDI_HDEVICE hDevice) {}
void APIENTRY PvgpuFlush(_In_ D3D10DDI_HDEVICE hDevice) {}
SIZE_T APIENTRY PvgpuCalcPrivateResourceSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATERESOURCE* pCreateResource) { return sizeof(PVGPU_UMD_RESOURCE); }
void APIENTRY PvgpuCreateResource(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATERESOURCE* pCreateResource, _In_ D3D10DDI_HRESOURCE hResource, _In_ D3D10DDI_HRTRESOURCE hRTResource) {}
void APIENTRY PvgpuDestroyResource(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hResource) {}
void APIENTRY PvgpuOpenResource(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_OPENRESOURCE* pOpenResource, _In_ D3D10DDI_HRESOURCE hResource, _In_ D3D10DDI_HRTRESOURCE hRTResource) {}
SIZE_T APIENTRY PvgpuCalcPrivateShaderSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures) { return sizeof(PVGPU_UMD_SHADER); }
void APIENTRY PvgpuCreateVertexShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ D3D10DDI_HSHADER hShader, _In_ D3D10DDI_HRTSHADER hRTShader, _In_ CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures) { PvgpuCreateShaderInternal(hDevice, pCode, hShader, PVGPU_SHADER_VERTEX); }
void APIENTRY PvgpuCreatePixelShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ D3D10DDI_HSHADER hShader, _In_ D3D10DDI_HRTSHADER hRTShader, _In_ CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures) { PvgpuCreateShaderInternal(hDevice, pCode, hShader, PVGPU_SHADER_PIXEL); }
void APIENTRY PvgpuCreateGeometryShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ D3D10DDI_HSHADER hShader, _In_ D3D10DDI_HRTSHADER hRTShader, _In_ CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures) { PvgpuCreateShaderInternal(hDevice, pCode, hShader, PVGPU_SHADER_GEOMETRY); }
void APIENTRY PvgpuCreateHullShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ D3D10DDI_HSHADER hShader, _In_ D3D10DDI_HRTSHADER hRTShader, _In_ CONST D3D11_1DDIARG_TESSELLATION_IO_SIGNATURES* pSignatures) { PvgpuCreateShaderInternal(hDevice, pCode, hShader, PVGPU_SHADER_HULL); }
void APIENTRY PvgpuCreateDomainShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST UINT* pCode, _In_ D3D10DDI_HSHADER hShader, _In_ D3D10DDI_HRTSHADER hRTShader, _In_ CONST D3D11_1DDIARG_TESSELLATION_IO_SIGNATURES* pSignatures) { PvgpuCreateShaderInternal(hDevice, pCode, hShader, PVGPU_SHADER_DOMAIN); }
void APIENTRY PvgpuDestroyShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuIaSetInputLayout(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HELEMENTLAYOUT hInputLayout) {}
void APIENTRY PvgpuIaSetVertexBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers, _In_reads_(NumBuffers) CONST UINT* pStrides, _In_reads_(NumBuffers) CONST UINT* pOffsets) {}
void APIENTRY PvgpuIaSetIndexBuffer(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hBuffer, _In_ DXGI_FORMAT Format, _In_ UINT Offset) {}
void APIENTRY PvgpuIaSetTopology(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10_DDI_PRIMITIVE_TOPOLOGY PrimitiveTopology) {}
void APIENTRY PvgpuVsSetShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuPsSetShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuGsSetShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuHsSetShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuDsSetShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuSetRenderTargets(_In_ D3D10DDI_HDEVICE hDevice, _In_reads_(NumViews) CONST D3D10DDI_HRENDERTARGETVIEW* phRenderTargetView, _In_ UINT NumViews, _In_ UINT ClearSlots, _In_ D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView) {}
void APIENTRY PvgpuSetViewports(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT NumViewports, _In_ UINT ClearViewports, _In_reads_(NumViewports) CONST D3D10_DDI_VIEWPORT* pViewports) {}
void APIENTRY PvgpuSetScissorRects(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT NumRects, _In_ UINT ClearRects, _In_reads_(NumRects) CONST D3D10_DDI_RECT* pRects) {}
void APIENTRY PvgpuSetBlendState(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HBLENDSTATE hBlendState, _In_reads_(4) CONST FLOAT BlendFactor[4], _In_ UINT SampleMask) {}
void APIENTRY PvgpuSetDepthStencilState(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState, _In_ UINT StencilRef) {}
void APIENTRY PvgpuSetRasterizerState(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRASTERIZERSTATE hRasterizerState) {}
void APIENTRY PvgpuDraw(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT VertexCount, _In_ UINT StartVertexLocation) {}
void APIENTRY PvgpuDrawIndexed(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT IndexCount, _In_ UINT StartIndexLocation, _In_ INT BaseVertexLocation) {}
void APIENTRY PvgpuDrawInstanced(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT VertexCountPerInstance, _In_ UINT InstanceCount, _In_ UINT StartVertexLocation, _In_ UINT StartInstanceLocation) {}
void APIENTRY PvgpuDrawIndexedInstanced(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT IndexCountPerInstance, _In_ UINT InstanceCount, _In_ UINT StartIndexLocation, _In_ INT BaseVertexLocation, _In_ UINT StartInstanceLocation) {}
void APIENTRY PvgpuDrawAuto(_In_ D3D10DDI_HDEVICE hDevice) {}
void APIENTRY PvgpuClearRenderTargetView(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRENDERTARGETVIEW hRenderTargetView, _In_reads_(4) CONST FLOAT ColorRGBA[4]) {}
void APIENTRY PvgpuClearDepthStencilView(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView, _In_ UINT ClearFlags, _In_ FLOAT Depth, _In_ UINT8 Stencil) {}
void APIENTRY PvgpuResourceCopy(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hDstResource, _In_ D3D10DDI_HRESOURCE hSrcResource) {}
void APIENTRY PvgpuResourceCopyRegion(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hDstResource, _In_ UINT DstSubresource, _In_ UINT DstX, _In_ UINT DstY, _In_ UINT DstZ, _In_ D3D10DDI_HRESOURCE hSrcResource, _In_ UINT SrcSubresource, _In_opt_ CONST D3D10_DDI_BOX* pSrcBox) {}
void APIENTRY PvgpuResourceUpdateSubresourceUP(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hDstResource, _In_ UINT DstSubresource, _In_opt_ CONST D3D10_DDI_BOX* pDstBox, _In_ CONST VOID* pSysMemUP, _In_ UINT RowPitch, _In_ UINT DepthPitch) {}
void APIENTRY PvgpuResourceMap(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hResource, _In_ UINT Subresource, _In_ D3D10_DDI_MAP MapType, _In_ UINT MapFlags, _Out_ D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource) {}
void APIENTRY PvgpuResourceUnmap(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hResource, _In_ UINT Subresource) {}
SIZE_T APIENTRY PvgpuCalcPrivateBlendStateSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_BLEND_DESC* pBlendDesc) { return sizeof(PVGPU_UMD_BLEND_STATE); }
void APIENTRY PvgpuCreateBlendState(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_BLEND_DESC* pBlendDesc, _In_ D3D10DDI_HBLENDSTATE hBlendState, _In_ D3D10DDI_HRTBLENDSTATE hRTBlendState) {}
void APIENTRY PvgpuDestroyBlendState(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HBLENDSTATE hBlendState) {}
SIZE_T APIENTRY PvgpuCalcPrivateRasterizerStateSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_RASTERIZER_DESC* pRasterizerDesc) { return sizeof(PVGPU_UMD_RASTERIZER_STATE); }
void APIENTRY PvgpuCreateRasterizerState(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_RASTERIZER_DESC* pRasterizerDesc, _In_ D3D10DDI_HRASTERIZERSTATE hRasterizerState, _In_ D3D10DDI_HRTRASTERIZERSTATE hRTRasterizerState) {}
void APIENTRY PvgpuDestroyRasterizerState(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRASTERIZERSTATE hRasterizerState) {}
SIZE_T APIENTRY PvgpuCalcPrivateDepthStencilStateSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_DEPTH_STENCIL_DESC* pDepthStencilDesc) { return sizeof(PVGPU_UMD_DEPTH_STENCIL_STATE); }
void APIENTRY PvgpuCreateDepthStencilState(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_DEPTH_STENCIL_DESC* pDepthStencilDesc, _In_ D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState, _In_ D3D10DDI_HRTDEPTHSTENCILSTATE hRTDepthStencilState) {}
void APIENTRY PvgpuDestroyDepthStencilState(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState) {}
SIZE_T APIENTRY PvgpuCalcPrivateSamplerSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_SAMPLER_DESC* pSamplerDesc) { return sizeof(PVGPU_UMD_SAMPLER); }
void APIENTRY PvgpuCreateSampler(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10_DDI_SAMPLER_DESC* pSamplerDesc, _In_ D3D10DDI_HSAMPLER hSampler, _In_ D3D10DDI_HRTSAMPLER hRTSampler) {}
void APIENTRY PvgpuDestroySampler(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSAMPLER hSampler) {}
SIZE_T APIENTRY PvgpuCalcPrivateElementLayoutSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATEELEMENTLAYOUT* pCreateElementLayout) { return sizeof(PVGPU_UMD_INPUT_LAYOUT); }
void APIENTRY PvgpuCreateElementLayout(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATEELEMENTLAYOUT* pCreateElementLayout, _In_ D3D10DDI_HELEMENTLAYOUT hElementLayout, _In_ D3D10DDI_HRTELEMENTLAYOUT hRTElementLayout) {}
void APIENTRY PvgpuDestroyElementLayout(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HELEMENTLAYOUT hElementLayout) {}
SIZE_T APIENTRY PvgpuCalcPrivateRenderTargetViewSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetView) { return sizeof(PVGPU_UMD_RENDER_TARGET_VIEW); }
void APIENTRY PvgpuCreateRenderTargetView(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetView, _In_ D3D10DDI_HRENDERTARGETVIEW hRenderTargetView, _In_ D3D10DDI_HRTRENDERTARGETVIEW hRTRenderTargetView) {}
void APIENTRY PvgpuDestroyRenderTargetView(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRENDERTARGETVIEW hRenderTargetView) {}
SIZE_T APIENTRY PvgpuCalcPrivateDepthStencilViewSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW* pCreateDepthStencilView) { return sizeof(PVGPU_UMD_DEPTH_STENCIL_VIEW); }
void APIENTRY PvgpuCreateDepthStencilView(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW* pCreateDepthStencilView, _In_ D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView, _In_ D3D10DDI_HRTDEPTHSTENCILVIEW hRTDepthStencilView) {}
void APIENTRY PvgpuDestroyDepthStencilView(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView) {}
SIZE_T APIENTRY PvgpuCalcPrivateShaderResourceViewSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView) { return sizeof(PVGPU_UMD_SHADER_RESOURCE_VIEW); }
void APIENTRY PvgpuCreateShaderResourceView(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView, _In_ D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView, _In_ D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView) {}
void APIENTRY PvgpuDestroyShaderResourceView(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView) {}
void APIENTRY PvgpuVsSetConstantBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers) {}
void APIENTRY PvgpuPsSetConstantBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers) {}
void APIENTRY PvgpuGsSetConstantBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers) {}
void APIENTRY PvgpuVsSetShaderResources(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews) {}
void APIENTRY PvgpuPsSetShaderResources(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews) {}
void APIENTRY PvgpuGsSetShaderResources(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews) {}
void APIENTRY PvgpuVsSetSamplers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumSamplers, _In_reads_(NumSamplers) CONST D3D10DDI_HSAMPLER* phSamplers) {}
void APIENTRY PvgpuPsSetSamplers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumSamplers, _In_reads_(NumSamplers) CONST D3D10DDI_HSAMPLER* phSamplers) {}
void APIENTRY PvgpuGsSetSamplers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumSamplers, _In_reads_(NumSamplers) CONST D3D10DDI_HSAMPLER* phSamplers) {}
void APIENTRY PvgpuHsSetConstantBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers) {}
void APIENTRY PvgpuDsSetConstantBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers) {}
void APIENTRY PvgpuHsSetShaderResources(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews) {}
void APIENTRY PvgpuDsSetShaderResources(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews) {}
void APIENTRY PvgpuHsSetSamplers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumSamplers, _In_reads_(NumSamplers) CONST D3D10DDI_HSAMPLER* phSamplers) {}
void APIENTRY PvgpuDsSetSamplers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumSamplers, _In_reads_(NumSamplers) CONST D3D10DDI_HSAMPLER* phSamplers) {}
void APIENTRY PvgpuCsSetShader(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HSHADER hShader) {}
void APIENTRY PvgpuCsSetConstantBuffers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT StartBuffer, _In_ UINT NumBuffers, _In_reads_(NumBuffers) CONST D3D10DDI_HRESOURCE* phBuffers) {}
void APIENTRY PvgpuCsSetShaderResources(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews) {}
void APIENTRY PvgpuCsSetSamplers(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumSamplers, _In_reads_(NumSamplers) CONST D3D10DDI_HSAMPLER* phSamplers) {}
void APIENTRY PvgpuCsSetUnorderedAccessViews(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT Offset, _In_ UINT NumViews, _In_reads_(NumViews) CONST D3D11DDI_HUNORDEREDACCESSVIEW* phUnorderedAccessViews, _In_reads_(NumViews) CONST UINT* pUAVInitialCounts) {}
void APIENTRY PvgpuDispatch(_In_ D3D10DDI_HDEVICE hDevice, _In_ UINT ThreadGroupCountX, _In_ UINT ThreadGroupCountY, _In_ UINT ThreadGroupCountZ) {}
void APIENTRY PvgpuDispatchIndirect(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D10DDI_HRESOURCE hBufferForArgs, _In_ UINT AlignedByteOffsetForArgs) {}
SIZE_T APIENTRY PvgpuCalcPrivateUnorderedAccessViewSize(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW* pCreateUnorderedAccessView) { return sizeof(PVGPU_UMD_SHADER_RESOURCE_VIEW); }
void APIENTRY PvgpuCreateUnorderedAccessView(_In_ D3D10DDI_HDEVICE hDevice, _In_ CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW* pCreateUnorderedAccessView, _In_ D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView, _In_ D3D11DDI_HRTUNORDEREDACCESSVIEW hRTUnorderedAccessView) {}
void APIENTRY PvgpuDestroyUnorderedAccessView(_In_ D3D10DDI_HDEVICE hDevice, _In_ D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView) {}
