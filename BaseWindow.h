#pragma once
#include <Windows.h>

// Reference: https://learn.microsoft.com/en-us/windows/win32/learnwin32/managing-application-state-

template <class DERIVED_TYPE>
class BaseWindow
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	    DERIVED_TYPE *pThis = NULL;

	    if (uMsg == WM_NCCREATE)
	    {
	        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
	        pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
	        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

	        pThis->m_hWnd = hwnd;
	    }
	    else
	    {
	        pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	    }
	    if (pThis)
	    {
	        return pThis->HandleMessage(uMsg, wParam, lParam);
	    }
	    else
	    {
	        return DefWindowProc(hwnd, uMsg, wParam, lParam);
	    }
	}

	BaseWindow() : m_hWnd(NULL) { }

	BOOL Create(
	    PCWSTR lpWindowName,
	    DWORD dwStyle,
	    DWORD dwExStyle = 0,
	    int x = CW_USEDEFAULT,
	    int y = CW_USEDEFAULT,
	    int nWidth = CW_USEDEFAULT,
	    int nHeight = CW_USEDEFAULT,
	    HWND hWndParent = 0,
	    HMENU hMenu = 0
	    )
	{
		WNDCLASSEX wc{};
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName  = NULL;

	    wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
	    wc.hInstance     = GetModuleHandle(NULL);
			
	    wc.lpszClassName = ClassName();

	    RegisterClassEx(&wc);

	    m_hWnd = CreateWindowEx(
	        dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
	        nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
	        );

	    return (m_hWnd ? TRUE : FALSE);
	}

	HWND Window() const { return m_hWnd; }

protected:

	virtual PCWSTR  ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_hWnd;
};
