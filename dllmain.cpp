
/*Dumpbin /exports d3d12.dll:
104    0 00004910 D3D12CoreCreateLayeredDevice
105    1 000048D0 D3D12CoreGetLayeredDeviceSize
106    2 000047E0 D3D12CoreRegisterLayers
101    3 00006AF0 D3D12CreateDevice
107    4 00010150 D3D12CreateRootSignatureDeserializer
108    5 000101B0 D3D12CreateVersionedRootSignatureDeserializer
109    6 00019218 D3D12DeviceRemovedExtendedData
110    7 00010210 D3D12EnableExperimentalFeatures
102    8 00010270 D3D12GetDebugInterface
111    9 00008F40 D3D12GetInterface
112    A 00010940 D3D12PIXEventsReplaceBlock
113    B 00010950 D3D12PIXGetThreadInfo
114    C 00010960 D3D12PIXNotifyWakeFromFenceSignal
115    D 00010990 D3D12PIXReportCounter
116    E 000102B0 D3D12SerializeRootSignature
117    F 00010310 D3D12SerializeVersionedRootSignature
100   10 00006ED0 GetBehaviorValue
103   11 0000FF40 SetAppCompatStringPointer
*/

#include <Windows.h>
#include <stdio.h>
#include <d3d12.h>
#include <d3dcommon.h>


/*This is probably the noob way to do it, but wtf...*/
typedef HRESULT(__cdecl* D3D12CreateDeviceFnc)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**);

typedef HRESULT(__cdecl* D3D12GetInterfaceFnc)(REFCLSID, REFIID, void**);

/*Global stuff...*/
HMODULE hL = 0;// , hDXGI = 0, hDX11 = 0;
D3D12GetInterfaceFnc D3D12GetInterfaceOrg = NULL;
D3D12CreateDeviceFnc D3D12CreateDeviceOrg = NULL;


/*Main Entry point*/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        /*Load original function dll*/
        //hDXGI = LoadLibraryW(L".\\dxgi.dll");
        //hDX11 = LoadLibraryW(L".\\d3d11.dll");
        hL = LoadLibraryW(L".\\d3d12_original.dll");

        if (hL == NULL)
            return FALSE; /*Exit on Error*/

        /*Get original function addr from original dll*/
        D3D12CreateDeviceOrg = (D3D12CreateDeviceFnc)GetProcAddress(hL, "D3D12CreateDevice");
        if (D3D12CreateDeviceOrg == NULL)
            return FALSE; /*Exit on Error*/

        D3D12GetInterfaceOrg = (D3D12GetInterfaceFnc)GetProcAddress(hL, "D3D12GetInterface");
        if (D3D12GetInterfaceOrg == NULL)
            return FALSE; /*Exit on Error*/

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        //FreeLibrary(hDX11);
        FreeLibrary(hL);
        //FreeLibrary(hDXGI);
        break;
    }
    return TRUE;
}

/*Proxy D3D12CreateDevice function*/
HRESULT Proxy_D3D12CreateDevice(IUnknown* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void** ppDevice)
{
    return D3D12CreateDeviceOrg(pAdapter, D3D_FEATURE_LEVEL_11_0, riid, ppDevice);
}

HRESULT Proxy_D3D12GetInterface(REFCLSID rclsid, REFIID riid, void** ppDevice)
{
    return D3D12GetInterfaceOrg(rclsid, riid, ppDevice);
}
