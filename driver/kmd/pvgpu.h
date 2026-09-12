/*
 * PVGPU Kernel-Mode Driver Header
 *
 * WDDM 2.0 display miniport driver for paravirtualized GPU.
 * Restricted to WDDM 1.1 / DirectX 10.1 capabilities.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PVGPU_KMD_H
#define PVGPU_KMD_H

#include <ntddk.h>
#include <dispmprt.h>
#include <d3dkmddi.h>
#include <d3dkmdt.h>

#include "../../protocol/pvgpu_protocol.h"

#define PVGPU_POOL_TAG          'UPGV'
#define PVGPU_DRIVER_VERSION    0x0001

#define PVGPU_HEAP_BLOCK_SIZE   0x1000
#define PVGPU_HEAP_MAX_BLOCKS   4096

#define PVGPU_MAX_DISPLAY_MODES     16
#define PVGPU_DEFAULT_REFRESH_RATE  60

typedef struct _PVGPU_DISPLAY_MODE {
    ULONG   Width;
    ULONG   Height;
    ULONG   RefreshRate;
    BOOLEAN Active;
} PVGPU_DISPLAY_MODE, *PPVGPU_DISPLAY_MODE;

static const PVGPU_DISPLAY_MODE g_DisplayModes[] = {
    { 1280,  720,  60, FALSE },
    { 1280,  720, 120, FALSE },
    { 1920, 1080,  60, FALSE },
    { 1920, 1080, 120, FALSE },
    { 1920, 1080, 144, FALSE },
    { 2560, 1440,  60, FALSE },
    { 2560, 1440, 120, FALSE },
    { 2560, 1440, 144, FALSE },
    { 3840, 2160,  60, FALSE },
    { 3840, 2160, 120, FALSE },
    { 1920, 1200,  60, FALSE },
    { 2560, 1600,  60, FALSE },
    { 1024,  768,  60, FALSE },
    { 1600, 1200,  60, FALSE },
};

#define PVGPU_NUM_DISPLAY_MODES (sizeof(g_DisplayModes) / sizeof(g_DisplayModes[0]))

typedef struct _PVGPU_HEAP_ALLOCATOR {
    ULONG       BlockSize;
    ULONG       NumBlocks;
    ULONG       FreeBlocks;
    ULONG       HeapOffset;
    ULONG       HeapSize;
    RTL_BITMAP  Bitmap;
    PULONG      BitmapBuffer;
    KSPIN_LOCK  Lock;
} PVGPU_HEAP_ALLOCATOR, *PPVGPU_HEAP_ALLOCATOR;

typedef struct _PVGPU_DEVICE_CONTEXT {
    HANDLE                      DeviceHandle;
    DXGKRNL_INTERFACE           DxgkInterface;
    DXGK_START_INFO             StartInfo;
    
    PHYSICAL_ADDRESS            Bar0PhysAddr;
    ULONG                       Bar0Length;
    volatile ULONG*             Bar0VirtAddr;
    
    PHYSICAL_ADDRESS            Bar2PhysAddr;
    ULONG                       Bar2Length;
    volatile UCHAR*             Bar2VirtAddr;
    
    volatile PvgpuControlRegion* ControlRegion;
    volatile UCHAR*             CommandRing;
    ULONG                       CommandRingSize;
    volatile UCHAR*             ResourceHeap;
    ULONG                       ResourceHeapSize;
    
    PVGPU_HEAP_ALLOCATOR        HeapAllocator;
    ULONG                       InterruptMessageNumber;
    BOOLEAN                     InterruptEnabled;
    ULONG                       DisplayWidth;
    ULONG                       DisplayHeight;
    ULONG                       DisplayRefresh;
    KSPIN_LOCK                  CommandLock;
} PVGPU_DEVICE_CONTEXT, *PPVGPU_DEVICE_CONTEXT;

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

NTSTATUS PvgpuAddDevice(_In_ DEVICE_OBJECT* PhysicalDeviceObject, _Outptr_ PVOID* MiniportDeviceContext);
NTSTATUS PvgpuStartDevice(_In_ CONST PVOID MiniportDeviceContext, _In_ DXGK_START_INFO* DxgkStartInfo, _In_ DXGKRNL_INTERFACE* DxgkInterface, _Out_ PULONG NumberOfVideoPresentSources, _Out_ PULONG NumberOfChildren);
NTSTATUS PvgpuStopDevice(_In_ PVOID MiniportDeviceContext);
NTSTATUS PvgpuRemoveDevice(_In_ PVOID MiniportDeviceContext);

BOOLEAN PvgpuInterruptRoutine(_In_ PVOID MiniportDeviceContext, _In_ ULONG MessageNumber);
VOID PvgpuDpcRoutine(_In_ PVOID MiniportDeviceContext);

NTSTATUS PvgpuQueryAdapterInfo(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_QUERYADAPTERINFO* QueryAdapterInfo);
NTSTATUS PvgpuQueryChildRelations(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGK_CHILD_DESCRIPTOR* ChildRelations, _In_ ULONG ChildRelationsSize);
NTSTATUS PvgpuQueryChildStatus(_In_ PVOID MiniportDeviceContext, _Inout_ DXGK_CHILD_STATUS* ChildStatus, _In_ BOOLEAN NonDestructiveOnly);
NTSTATUS PvgpuQueryDeviceDescriptor(_In_ CONST PVOID MiniportDeviceContext, _In_ ULONG ChildUid, _Inout_ DXGK_DEVICE_DESCRIPTOR* DeviceDescriptor);
NTSTATUS PvgpuSetPowerState(_In_ PVOID MiniportDeviceContext, _In_ ULONG HardwareUid, _In_ DEVICE_POWER_STATE DevicePowerState, _In_ POWER_ACTION ActionType);

NTSTATUS PvgpuBuildPagingBuffer(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGKARG_BUILDPAGINGBUFFER* BuildPagingBuffer);
NTSTATUS PvgpuSubmitCommand(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_SUBMITCOMMAND* SubmitCommand);
NTSTATUS PvgpuPreemptCommand(_In_ PVOID MiniportDeviceContext, _In_ CONST DXGKARG_PREEMPTCOMMAND* PreemptCommand);
NTSTATUS PvgpuPatch(_In_ PVOID MiniportDeviceContext, _In_ CONST DXGKARG_PATCH* Patch);

NTSTATUS PvgpuCreateDevice(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGKARG_CREATEDEVICE* CreateDevice);
NTSTATUS PvgpuDestroyDevice(_In_ PVOID MiniportDeviceContext, _In_ HANDLE DeviceHandle);
NTSTATUS PvgpuCreateContext(_In_ PVOID MiniportDeviceContext, _Inout_ DXGKARG_CREATECONTEXT* CreateContext);
NTSTATUS PvgpuDestroyContext(_In_ PVOID MiniportDeviceContext, _In_ HANDLE ContextHandle);

NTSTATUS PvgpuCreateAllocation(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGKARG_CREATEALLOCATION* CreateAllocation);
NTSTATUS PvgpuDestroyAllocation(_In_ PVOID MiniportDeviceContext, _In_ CONST DXGKARG_DESTROYALLOCATION* DestroyAllocation);
NTSTATUS PvgpuDescribeAllocation(_In_ PVOID MiniportDeviceContext, _Inout_ DXGKARG_DESCRIBEALLOCATION* DescribeAllocation);
NTSTATUS PvgpuGetStandardAllocationDriverData(_In_ PVOID MiniportDeviceContext, _Inout_ DXGKARG_GETSTANDARDALLOCATIONDRIVERDATA* StandardAllocation);

NTSTATUS PvgpuPresent(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGKARG_PRESENT* Present);
NTSTATUS PvgpuRender(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGKARG_RENDER* Render);
NTSTATUS PvgpuEscape(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_ESCAPE* Escape);

NTSTATUS PvgpuIsSupportedVidPn(_In_ CONST PVOID MiniportDeviceContext, _Inout_ DXGKARG_ISSUPPORTEDVIDPN* IsSupportedVidPn);
NTSTATUS PvgpuRecommendFunctionalVidPn(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_RECOMMENDFUNCTIONALVIDPN* RecommendFunctionalVidPn);
NTSTATUS PvgpuEnumVidPnCofuncModality(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_ENUMVIDPNCOFUNCMODALITY* EnumCofuncModality);
NTSTATUS PvgpuSetVidPnSourceAddress(_In_ PVOID MiniportDeviceContext, _In_ CONST DXGKARG_SETVIDPNSOURCEADDRESS* SetVidPnSourceAddress);
NTSTATUS PvgpuSetVidPnSourceVisibility(_In_ PVOID MiniportDeviceContext, _In_ CONST DXGKARG_SETVIDPNSOURCEVISIBILITY* SetVidPnSourceVisibility);
NTSTATUS PvgpuCommitVidPn(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_COMMITVIDPN* CommitVidPn);
NTSTATUS PvgpuUpdateActiveVidPnPresentPath(_In_ PVOID MiniportDeviceContext, _In_ CONST DXGKARG_UPDATEACTIVEVIDPNPRESENTPATH* UpdateActiveVidPnPresentPath);
NTSTATUS PvgpuRecommendMonitorModes(_In_ CONST PVOID MiniportDeviceContext, _In_ CONST DXGKARG_RECOMMENDMONITORMODES* RecommendMonitorModes);

FORCEINLINE ULONG PvgpuReadBar0(_In_ PPVGPU_DEVICE_CONTEXT Context, _In_ ULONG Offset) { return READ_REGISTER_ULONG(&Context->Bar0VirtAddr[Offset / sizeof(ULONG)]); }
FORCEINLINE VOID PvgpuWriteBar0(_In_ PPVGPU_DEVICE_CONTEXT Context, _In_ ULONG Offset, _In_ ULONG Value) { WRITE_REGISTER_ULONG(&Context->Bar0VirtAddr[Offset / sizeof(ULONG)], Value); }

NTSTATUS PvgpuSubmitToRing(_In_ PPVGPU_DEVICE_CONTEXT Context, _In_ PVOID CommandData, _In_ ULONG CommandSize);
FORCEINLINE VOID PvgpuRingDoorbell(_In_ PPVGPU_DEVICE_CONTEXT Context) { PvgpuWriteBar0(Context, PVGPU_REG_DOORBELL, 1); }

#endif /* PVGPU_KMD_H */