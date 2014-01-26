// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"
#include "IPolicyConfig.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IMMEndpoint = __uuidof(IMMEndpoint);
const size_t NO_DEFAULT_FOUND = 0xFFFFFFFF;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hr = CoInitialize(NULL);
    hr = CycleDefaultEndpoint();
    EXIT_ON_ERROR(hr);

Exit:
    CoUninitialize();

    return 0;
}

HRESULT ActivateEndpoint(LPWSTR szId)
{
    IPolicyConfigVista *pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
    if (SUCCEEDED(hr))
    {
        hr = pPolicyConfig->SetDefaultEndpoint(szId, reserved);
        pPolicyConfig->Release();
    }
    return hr;
}

HRESULT CycleDefaultEndpoint()
{
    LPWSTR szDefaultId = NULL;
    LPWSTR* rgIds = NULL;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDeviceCollection* pDevices = NULL;
    IMMDevice* pEndpoint = NULL;
    size_t count = 0;
    size_t iDefault = NO_DEFAULT_FOUND;
    HRESULT hr;

    IPolicyConfigVista *pPolicyConfig;
    ERole reserved = eConsole;

    hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
    EXIT_ON_ERROR(hr);


    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator,
        (void**)&pEnumerator);
    EXIT_ON_ERROR(hr);

    hr = pEnumerator->EnumAudioEndpoints(EDataFlow::eRender, DEVICE_STATE_ACTIVE, &pDevices);
    EXIT_ON_ERROR(hr);
    
    hr = pDevices->GetCount(&count);
    EXIT_ON_ERROR(hr);

    // get the current default endpoint
    hr = pEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &pEndpoint);
    EXIT_ON_ERROR(hr);

    // get the current default endpoint's id, to be used to find the next one in the list
    hr = pEndpoint->GetId(&szDefaultId);
    EXIT_ON_ERROR(hr);

    SAFE_RELEASE(pEndpoint);
    
    rgIds = (LPWSTR*)CoTaskMemAlloc(sizeof(LPWSTR)* count);
    SecureZeroMemory(rgIds, sizeof(LPWSTR)* count);

    for (size_t i = 0; i < count; i++)
    {
        hr = pDevices->Item(i, &pEndpoint);
        EXIT_ON_ERROR(hr);

        hr = pEndpoint->GetId(&rgIds[i]);
        EXIT_ON_ERROR(hr);

        SAFE_RELEASE(pEndpoint);

        if (iDefault == NO_DEFAULT_FOUND && 0 == wcscmp(rgIds[i], szDefaultId))
        {
            iDefault = i;
        }
    }

    // select the next endpoint's index
    iDefault = (iDefault + 1) % count;

    hr = pPolicyConfig->SetDefaultEndpoint(rgIds[iDefault], reserved);
    EXIT_ON_ERROR(hr);

Exit:
    SAFE_FREE_ARRAY(rgIds, count);
    SAFE_RELEASE(pPolicyConfig);
    SAFE_RELEASE(pEndpoint);
    SAFE_RELEASE(pDevices);
    SAFE_RELEASE(pEnumerator);
    return hr;
}