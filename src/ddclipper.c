#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "IDirectDrawClipper.h"
#include "ddclipper.h"
#include "debug.h"


HRESULT ddc_GetClipList(IDirectDrawClipperImpl* This, LPRECT lpRect, LPRGNDATA lpClipList, LPDWORD lpdwSiz)
{
    if (!This->region)
        return DDERR_NOCLIPLIST;

    if (!lpdwSiz)
        return DDERR_INVALIDPARAMS;

    HRGN region = NULL;

    if (lpRect)
    {
        region = CreateRectRgnIndirect(lpRect);

        if (!region)
            return DDERR_INVALIDPARAMS;

        if (CombineRgn(region, This->region, region, RGN_AND) == ERROR)
        {
            DeleteObject(region);
            return DDERR_GENERIC;
        }
    }
    else
    {
        region = This->region;
    }

    *lpdwSiz = GetRegionData(region, *lpdwSiz, lpClipList);

    if (lpRect)
        DeleteObject(region);

    return DD_OK;
}

HRESULT ddc_GetHWnd(IDirectDrawClipperImpl* This, HWND FAR* lphWnd)
{
    if (!lphWnd)
        return DDERR_INVALIDPARAMS;

    *lphWnd = This->hwnd;

    return DD_OK;
}

HRESULT ddc_SetClipList(IDirectDrawClipperImpl* This, LPRGNDATA lpClipList, DWORD dwFlags)
{
    if (This->hwnd)
        return DDERR_CLIPPERISUSINGHWND;

    if (This->region)
        DeleteObject(This->region);

    if (lpClipList)
    {
        This->region = ExtCreateRegion(NULL, 0, lpClipList);

        if (!This->region)
            return DDERR_INVALIDCLIPLIST;
    }
    else
    {
        This->region = NULL;
    }

    return DD_OK;
}

HRESULT ddc_SetHWnd(IDirectDrawClipperImpl* This, DWORD dwFlags, HWND hWnd)
{
    /* FIXME: need to set up This->region here (from hwnd region) */
    This->hwnd = hWnd;

    return DD_OK;
}

HRESULT dd_CreateClipper(DWORD dwFlags, IDirectDrawClipperImpl** lplpDDClipper, IUnknown FAR* pUnkOuter)
{
    if (!lplpDDClipper)
        return DDERR_INVALIDPARAMS;

    IDirectDrawClipperImpl* c =
        (IDirectDrawClipperImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IDirectDrawClipperImpl));

    TRACE("     clipper = %p\n", c);

    c->lpVtbl = &g_ddc_vtbl;
    IDirectDrawClipper_AddRef(c);

    *lplpDDClipper = c;

    return DD_OK;
}
