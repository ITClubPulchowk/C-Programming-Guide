#include <Windows.h>
#include <avrt.h>
#include <stdint.h>
#include <stdbool.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <dxgi1_5.h>

#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Avrt.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//
//
//

typedef struct float2 {
	float x, y;
} float2;

#define ArrayCount(a) (size_t)(sizeof(a) / sizeof((a)[0]))

static WINDOWPLACEMENT	g_window_placement;
static bool				g_tearing_capable;

ID3D11Device *g_device;
ID3D11DeviceContext *g_device_context;
IDXGISwapChain1 *g_swap_chain;

ID3D11Texture2D *g_render_texture;
ID3D11RenderTargetView *g_render_target_view;

static char *g_string_buffer;
static size_t g_string_buffer_size;

static float g_zoom;
static float2 g_center;
static bool g_mouse_down;

static const float2 RectMin = { -2.5f, -2.0f };
static const float2 RectMax = { 1.0f, 2.0f };

static const float g_Vertices[] = {
	-1, -1, -1, 1, 1, +1,
	-1, -1, +1, 1, 1, -1
};

static bool g_take_screenshot = false;

void TakeScreenshot() {
	g_take_screenshot = true;
}

bool RecordFrame() {
	return g_take_screenshot;
}

void RecordFrameHandled() {
	g_take_screenshot = false;
}

//
//
//

void ToggleFullscreen(HWND hwnd) {
	DWORD dwStyle = GetWindowLongW(hwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(hwnd, &g_window_placement) &&
			GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
			SetWindowLongW(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP,
						 mi.rcMonitor.left, mi.rcMonitor.top,
						 mi.rcMonitor.right - mi.rcMonitor.left,
						 mi.rcMonitor.bottom - mi.rcMonitor.top,
						 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	} else {
		SetWindowLongW(hwnd, GWL_STYLE,
					   dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &g_window_placement);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
					 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
					 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

const wchar_t *DirectXGetErrorString(HRESULT code) {
	switch (code) {
		case DXGI_ERROR_ACCESS_DENIED:
			return L"You tried to use a resource to which you did not have the required access privileges. This error is most typically caused when you write to a shared resource with read-only access.";
		case DXGI_ERROR_ACCESS_LOST:
			return L"The desktop duplication interface is invalid. The desktop duplication interface typically becomes invalid when a different type of image is displayed on the desktop.";
		case DXGI_ERROR_ALREADY_EXISTS:
			return L"The desired element already exists. This is returned by DXGIDeclareAdapterRemovalSupport if it is not the first time that the function is called.";
		case DXGI_ERROR_CANNOT_PROTECT_CONTENT:
			return L"DXGI can't provide content protection on the swap chain. This error is typically caused by an older driver, or when you use a swap chain that is incompatible with content protection.";
		case DXGI_ERROR_DEVICE_HUNG:
			return L"The application's device failed due to badly formed commands sent by the application. This is an design-time issue that should be investigated and fixed.";
		case DXGI_ERROR_DEVICE_REMOVED:
			return L"The video card has been physically removed from the system, or a driver upgrade for the video card has occurred. The application should destroy and recreate the device. For help debugging the problem, call ID3D10Device::GetDeviceRemovedReason.";
		case DXGI_ERROR_DEVICE_RESET:
			return L"The device failed due to a badly formed command. This is a run-time issue; The application should destroy and recreate the device.";
		case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
			return L"The driver encountered a problem and was put into the device removed state.";
		case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
			return L"An event (for example, a power cycle) interrupted the gathering of presentation statistics.";
		case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
			return L"The application attempted to acquire exclusive ownership of an output, but failed because some other application (or device within the application) already acquired ownership.";
		case DXGI_ERROR_INVALID_CALL:
			return L"The application provided invalid parameter data, this must be debugged and fixed before the application is released.";
		case DXGI_ERROR_MORE_DATA:
			return L"The buffer supplied by the application is not big enough to hold the requested data.";
		case DXGI_ERROR_NAME_ALREADY_EXISTS:
			return L"The supplied name of a resource in a call to IDXGIResource1::CreateSharedHandle is already associated with some other resource.";
		case DXGI_ERROR_NONEXCLUSIVE:
			return L"A global counter resource is in use, and the Direct3D device can't currently use the counter resource.";
		case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
			return L"The resource or request is not currently available, but it might become available later.";
		case DXGI_ERROR_NOT_FOUND:
			return L"When calling IDXGIObject::GetPrivateData, the GUID passed in is not recognized as one previously passed to IDXGIObject::SetPrivateData or IDXGIObject::SetPrivateDataInterface."
				L"\nWhen calling IDXGIFactory::EnumAdapters or IDXGIAdapter::EnumOutputs, the enumerated ordinal is out of range.";
		case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:
			return L"Reserved";
		case DXGI_ERROR_REMOTE_OUTOFMEMORY:
			return L"Reserved";
		case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
			return L"The DXGI output(monitor) to which the swap chain content was restricted is now disconnected or changed.";
		case DXGI_ERROR_SDK_COMPONENT_MISSING:
			return L"The operation depends on an SDK component that is missing or mismatched.";
		case DXGI_ERROR_SESSION_DISCONNECTED:
			return L"The Remote Desktop Services session is currently disconnected.";
		case DXGI_ERROR_UNSUPPORTED:
			return L"The requested functionality is not supported by the device or the driver.";
		case DXGI_ERROR_WAIT_TIMEOUT:
			return L"The time - out interval elapsed before the next desktop frame was available.";
		case DXGI_ERROR_WAS_STILL_DRAWING:
			return L"The GPU was busy at the moment when a call was made to perform an operation, anddid not execute or schedule the operation.";
		case D3D11_ERROR_FILE_NOT_FOUND:
			return L"The file was not found.";
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			return L"There are too many unique instances of a particular type of state object.";
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			return L"There are too many unique instances of a particular type of view object.";
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
			return L"The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD.";
		case E_INVALIDARG:
			return L"Invalid Argument";
		case E_OUTOFMEMORY:
			return L"Out of Memory";
		case S_OK:
			return L"The method succeeded without an error.";
	}
	return L"-unknown-";
}

void DirectXHandleError(HRESULT res) {
	if (res != S_OK) {
		const wchar_t *message = DirectXGetErrorString(res);
		FatalAppExitW(0, message);
	}
}

void DirectXCreateResources() {
	HRESULT hresult;

	hresult = g_swap_chain->lpVtbl->GetBuffer(g_swap_chain, 0, &IID_ID3D11Texture2D, &g_render_texture);
	DirectXHandleError(hresult);

	hresult = g_device->lpVtbl->CreateRenderTargetView(g_device, (ID3D11Resource *)g_render_texture, 0, &g_render_target_view);
	DirectXHandleError(hresult);
}

void DirectXDestoryResources() {
	g_render_target_view->lpVtbl->Release(g_render_target_view);
	g_render_texture->lpVtbl->Release(g_render_texture);
	g_device_context->lpVtbl->ClearState(g_device_context);
	g_device_context->lpVtbl->Flush(g_device_context);
}

void DirectXInitialize(HWND window) {
	IDXGIFactory2 *factory;

	uint32_t factory_flags = 0;
	#if defined(_DEBUG) || defined(DEBUG)
	factory_flags = DXGI_CREATE_FACTORY_DEBUG;
	#endif

	DirectXHandleError(CreateDXGIFactory2(factory_flags, &IID_IDXGIFactory2, &factory));

	uint32_t flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	#if defined(_DEBUG) || defined(DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	IDXGIAdapter1 *adapter = 0;
	size_t max_dedicated_memory = 0;
	IDXGIAdapter1 *adapter_selection = 0;

	for (uint32_t adapter_index = 0;
		 factory->lpVtbl->EnumAdapters1(factory, adapter_index, &adapter_selection) != DXGI_ERROR_NOT_FOUND;
		 ++adapter_index) {
		DXGI_ADAPTER_DESC1 desc;
		adapter_selection->lpVtbl->GetDesc1(adapter_selection, &desc);

		auto device_creation_result = D3D11CreateDevice((IDXGIAdapter *)adapter_selection,
														D3D_DRIVER_TYPE_UNKNOWN,
														0, flags,
														feature_levels, ArrayCount(feature_levels),
														D3D11_SDK_VERSION, NULL, NULL, NULL);

		if (SUCCEEDED(device_creation_result) && desc.DedicatedVideoMemory > max_dedicated_memory) {
			max_dedicated_memory = desc.DedicatedVideoMemory;
			adapter = adapter_selection;
		} else {
			adapter_selection->lpVtbl->Release(adapter_selection);
		}
	}

	if (adapter == 0) {
		FatalAppExitW(0, L"DirectX supported adapter not found!");
	}

	HRESULT hresult;

	D3D_FEATURE_LEVEL feature;
	hresult = D3D11CreateDevice((IDXGIAdapter *)adapter,
								D3D_DRIVER_TYPE_UNKNOWN,
								NULL, flags,
								feature_levels, ArrayCount(feature_levels),
								D3D11_SDK_VERSION, &g_device, &feature, &g_device_context);
	DirectXHandleError(hresult);

	uint32_t swap_chain_flags = 0;

	IDXGIFactory5 *factory5;
	if (SUCCEEDED(factory->lpVtbl->GetParent(factory, &IID_IDXGIFactory5, &factory5))) {
		BOOL tearing = FALSE;
		if (SUCCEEDED(factory5->lpVtbl->CheckFeatureSupport(factory5, DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing, sizeof(tearing)))) {
			if (tearing == TRUE) {
				swap_chain_flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
				g_tearing_capable = true;
			}
		}
	}

	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.Width = 0;
	swap_chain_desc.Height = 0;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.Stereo = FALSE;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swap_chain_desc.Flags = swap_chain_flags;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	hresult = factory->lpVtbl->CreateSwapChainForHwnd(factory, (IUnknown *)g_device, window, 
													  &swap_chain_desc, NULL, NULL, &g_swap_chain);
	DirectXHandleError(hresult);

	#if defined(_DEBUG) || defined(DEBUG)
	ID3D11Debug *debug = 0;

	if (SUCCEEDED(g_device->lpVtbl->QueryInterface(g_device, &IID_ID3D11Debug, (void **)&debug))) {
		ID3D11InfoQueue *info_queue = NULL;
		if (SUCCEEDED(g_device->lpVtbl->QueryInterface(g_device, &IID_ID3D11InfoQueue, (void **)&info_queue))) {
			MessageBoxW(window, L"ID3D11Debug enabled.", L"DirectX", MB_ICONERROR | MB_OK);

			info_queue->lpVtbl->SetBreakOnSeverity(info_queue, D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			info_queue->lpVtbl->SetBreakOnSeverity(info_queue, D3D11_MESSAGE_SEVERITY_ERROR, true);

			D3D11_MESSAGE_ID hide[] = {
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
			};

			D3D11_INFO_QUEUE_FILTER filter;
			ZeroMemory(&filter, sizeof(filter));
			filter.DenyList.NumIDs = ArrayCount(hide);
			filter.DenyList.pIDList = hide;
			info_queue->lpVtbl->AddStorageFilterEntries(info_queue, &filter);

			info_queue->lpVtbl->Release(info_queue);
		}
	}
	debug->lpVtbl->Release(debug);
	#endif

	factory->lpVtbl->Release(factory);
	adapter->lpVtbl->Release(adapter);

	DirectXCreateResources();
}

void DirectXShutdown() {
	DirectXDestoryResources();
	g_swap_chain->lpVtbl->Release(g_swap_chain);
	g_device_context->lpVtbl->Release(g_device_context);
	g_device->lpVtbl->Release(g_device);
}

void DirectXWindowSizeChanged(uint32_t w, uint32_t h) {
	if (g_device && w > 0 && h > 0) {
		DirectXDestoryResources();
		g_swap_chain->lpVtbl->ResizeBuffers(g_swap_chain, 2, 0, 0, DXGI_FORMAT_UNKNOWN, g_tearing_capable ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
		DirectXCreateResources();
	}
}

LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	LRESULT result = 0;

	switch (msg) {
		case WM_DPICHANGED: {
			RECT *const suggested_rect = (RECT *)lparam;
			auto        left = suggested_rect->left;
			auto        top = suggested_rect->top;
			auto        width = suggested_rect->right - suggested_rect->left;
			auto        height = suggested_rect->bottom - suggested_rect->top;
			SetWindowPos(wnd, 0, left, top, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
		} break;

		case WM_SIZE: {
			uint32_t x = (uint32_t)LOWORD(lparam);
			uint32_t y = (uint32_t)HIWORD(lparam);
			DirectXWindowSizeChanged(x, y);
			result = DefWindowProcW(wnd, msg, wparam, lparam);
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
		} break;

		case WM_KEYDOWN: {
			if (wparam == VK_F11) {
				ToggleFullscreen(wnd);
			} else if (wparam == VK_F5 && ((lparam & (1 << 30)) != (1 << 30))) {
				TakeScreenshot();
			}
		} break;

		case WM_MOUSEWHEEL: {
			float factor = 1.1f;
			int amount = GET_WHEEL_DELTA_WPARAM(wparam);
			if (amount < 0) {
				g_zoom *= (factor) * (float)(-1 * amount) / (float)WHEEL_DELTA;
			} else if (amount > 0) {
				g_zoom /= (factor) * (float)amount / (float)WHEEL_DELTA;
			}
		} break;

		case WM_LBUTTONDOWN: {
			g_mouse_down = true;
		} break;

		case WM_LBUTTONUP: {
			g_mouse_down = false;
		} break;

		case WM_INPUT: {
			static RAWINPUT *input;
			static UINT input_size;

			if (g_mouse_down && GET_RAWINPUT_CODE_WPARAM(wparam) == RIM_INPUT) {
				UINT size;
				GetRawInputData((HRAWINPUT)lparam, RID_INPUT, 0, &size, sizeof(RAWINPUTHEADER));

				if (input_size < size) {
					input = (RAWINPUT *)realloc(input, size);
					if (input == NULL) {
						FatalAppExitW(0, L"realloc failed, out of memory");
					}
					input_size = size;
				}

				if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, input, &size, sizeof(RAWINPUTHEADER)) == size &&
					input->header.dwType == RIM_TYPEMOUSE) {
					MONITORINFO monitor_info;
					monitor_info.cbSize = sizeof(monitor_info);

					if (GetMonitorInfoW(MonitorFromWindow(wnd, MONITOR_DEFAULTTONEAREST), &monitor_info)) {
						LONG monitor_w = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
						LONG monitor_h = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

						LONG xrel = input->data.mouse.lLastX;
						LONG yrel = input->data.mouse.lLastY;

						float dx = (float)xrel / (float)monitor_w;
						float dy = (float)yrel / (float)monitor_h;

						g_center.x -= dx * (RectMax.x - RectMin.x) * g_zoom;
						g_center.y -= dy * (RectMax.y - RectMin.y) * g_zoom;
					}
				}
			}
			result = DefWindowProcW(wnd, msg, wparam, lparam);
		} break;

		default: {
			result = DefWindowProcW(wnd, msg, wparam, lparam);
		}
	}

	return result;
}

ID3DBlob *CompileHLSL(ID3DBlob *source, const char *identifier, const char *entry_point, const char *target) {
	ID3DBlob *code = 0, *error_messages = 0;
	HRESULT result = D3DCompile2(source->lpVtbl->GetBufferPointer(source), source->lpVtbl->GetBufferSize(source), 
								 identifier, NULL, NULL, entry_point, target, 0, 0, 0, NULL, 0, 
								 &code, &error_messages);

	bool success = true;
	if (FAILED(result)) {
		if (g_string_buffer_size < error_messages->lpVtbl->GetBufferSize(error_messages) + 1) {
			g_string_buffer_size = error_messages->lpVtbl->GetBufferSize(error_messages) + 1;
			g_string_buffer = (char *)realloc(g_string_buffer, g_string_buffer_size);
			if (g_string_buffer == NULL) {
				FatalAppExitW(0, L"realloc failed, out of memory");
			}
		}

		memcpy(g_string_buffer, 
			   error_messages->lpVtbl->GetBufferPointer(error_messages), 
			   error_messages->lpVtbl->GetBufferSize(error_messages));
		g_string_buffer[error_messages->lpVtbl->GetBufferSize(error_messages)] = 0;

		MessageBoxA(NULL, g_string_buffer, "Compile Error", MB_ICONERROR | MB_OK);
	}

	if (error_messages) {
		error_messages->lpVtbl->Release(error_messages);
	}

	return code;
}

ID3DBlob *ReadEntireFile(HANDLE handle) {
	SetFilePointer(handle, 0, NULL, FILE_BEGIN);

	ID3DBlob *result = NULL;

	LARGE_INTEGER file_size;
	GetFileSizeEx(handle, &file_size);
	HRESULT hres = D3DCreateBlob((size_t)file_size.QuadPart, &result);
	if (FAILED(hres)) return result;

	DWORD read_bytes;
	BOOL res = ReadFile(handle, 
						result->lpVtbl->GetBufferPointer(result), 
						(DWORD)result->lpVtbl->GetBufferSize(result),
						&read_bytes, 0);
	if (!res || read_bytes != file_size.QuadPart) {
		result->lpVtbl->Release(result);
		return NULL;
	}

	return result;
}

ULARGE_INTEGER GetFileAccessTime(HANDLE handle) {
	ULARGE_INTEGER r;
	r.QuadPart = 0;

	FILETIME write_time;
	if (GetFileTime(handle, NULL, NULL, &write_time)) {
		r.HighPart = write_time.dwHighDateTime;
		r.LowPart = write_time.dwLowDateTime;
	}

	return r;
}

bool LoadShader(HANDLE hfile, const char *identifier, 
				ID3D11VertexShader **vertex_shader, ID3D11PixelShader **pixel_shader, 
				ID3DBlob **layout) {
	const char *vertex_entry = "vs_main";
	const char *pixel_entry = "ps_main";

	ID3DBlob *blob = ReadEntireFile(hfile);
	if (blob) {
		HRESULT hres;

		ID3DBlob *vertex = CompileHLSL(blob, identifier, vertex_entry, "vs_5_0");
		if (vertex) {
			ID3DBlob *pixel = CompileHLSL(blob, identifier, pixel_entry, "ps_5_0");

			if (pixel) {
				hres = g_device->lpVtbl->CreateVertexShader(g_device,
															vertex->lpVtbl->GetBufferPointer(vertex), 
															vertex->lpVtbl->GetBufferSize(vertex), 
															NULL, vertex_shader);
				if (SUCCEEDED(hres)) {
					hres = g_device->lpVtbl->CreatePixelShader(g_device,
															   pixel->lpVtbl->GetBufferPointer(pixel), 
															   pixel->lpVtbl->GetBufferSize(pixel), 
															   NULL, pixel_shader);
					if (SUCCEEDED(hres)) {
						pixel->lpVtbl->Release(pixel);
						blob->lpVtbl->Release(blob);
						if (layout) {
							*layout = vertex;
						} else {
							vertex->lpVtbl->Release(vertex);
						}
						return true;
					}

					(*vertex_shader)->lpVtbl->Release(*vertex_shader);
				}

				pixel->lpVtbl->Release(pixel);
			}

			vertex->lpVtbl->Release(vertex);
		}

		blob->lpVtbl->Release(blob);
	}

	return false;
}

#pragma pack(push, 4)
typedef struct Constant_Layout {
	float2 Resolution;
	float2 RectMin;
	float2 RectMax;
	float2 Center;
	float Zoom;
	float AspectRatio;
	float Time;
	float padding;
} Constant_Layout;
#pragma pack(pop)

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, int show_cmd) {
	DWORD task_index = 0;
	HANDLE task_handle = AvSetMmThreadCharacteristicsW(L"Games", &task_index);

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	WNDCLASSEXW window_class = { sizeof(window_class) };
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = WindowProc;
	window_class.hInstance = instance;
	window_class.hIcon = LoadImageW(instance, MAKEINTRESOURCEW(101), IMAGE_ICON, 0, 0, 0);
	window_class.hCursor = LoadCursorW(NULL, IDC_ARROW);
	window_class.lpszClassName = L"ZeroWindowClass";

	RegisterClassExW(&window_class);

	HWND window = CreateWindowExW(0, L"ZeroWindowClass", L"Mandelbrot",
								  WS_OVERLAPPEDWINDOW,
								  CW_USEDEFAULT, CW_USEDEFAULT,
								  CW_USEDEFAULT, CW_USEDEFAULT,
								  NULL, NULL, instance, NULL);

	if (window == NULL) {
		FatalAppExitW(0, L"Window could not be created");
	}

	RAWINPUTDEVICE device;
	device.usUsagePage = 0x1;
	device.usUsage = 0x2;
	device.dwFlags = 0;
	device.hwndTarget = window;

	if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
		DestroyWindow(window);
		UnregisterClassW(L"ZeroWindowClass", instance);
		FatalAppExitW(0, L"Raw Input not supported");
	}

	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);

	g_window_placement.length = sizeof(g_window_placement);
	GetWindowPlacement(window, &g_window_placement);

	DirectXInitialize(window);

	ID3D11Buffer *vertex_buffer;
	UINT stride, offset;
	stride = sizeof(float) * 2;
	offset = 0;

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth = sizeof(g_Vertices);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initial_data;
	initial_data.pSysMem = g_Vertices;
	initial_data.SysMemPitch = 0;
	initial_data.SysMemSlicePitch = 0;

	HRESULT result = g_device->lpVtbl->CreateBuffer(g_device, &buffer_desc, &initial_data, &vertex_buffer);
	DirectXHandleError(result);

	const char *file = "mandelbrot.hlsl";

	HANDLE hfile = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		FatalAppExitW(0, L"Shader file not found");
	}
	
	ULARGE_INTEGER mod_time = GetFileAccessTime(hfile);

	ID3D11VertexShader *vertex_shader = NULL;
	ID3D11PixelShader *pixel_shader = NULL;
	ID3DBlob *layout = NULL;
	if (!LoadShader(hfile, file, &vertex_shader, &pixel_shader, &layout)) {
		FatalAppExitW(0, L"Failed to load shaders");
	}

	D3D11_INPUT_ELEMENT_DESC input_elements[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D11InputLayout *input_layout;
	result = g_device->lpVtbl->CreateInputLayout(g_device, input_elements, ArrayCount(input_elements), 
												 layout->lpVtbl->GetBufferPointer(layout), 
												 layout->lpVtbl->GetBufferSize(layout), 
												 &input_layout);
	DirectXHandleError(result);


	D3D11_BUFFER_DESC cbuffer_desc;
	cbuffer_desc.ByteWidth = sizeof(Constant_Layout);
	cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbuffer_desc.CPUAccessFlags = 0;
	cbuffer_desc.MiscFlags = 0;
	cbuffer_desc.StructureByteStride = 0;

	ID3D11Buffer *constant_buffer;
	result = g_device->lpVtbl->CreateBuffer(g_device, &cbuffer_desc, NULL, &constant_buffer);
	DirectXHandleError(result);

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	g_zoom = 1;

	bool running = true;

	while (running) {
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		// Reload shader if modified
		ULARGE_INTEGER new_mod_time = GetFileAccessTime(hfile);
		if (new_mod_time.QuadPart > mod_time.QuadPart) {
			ID3D11VertexShader *new_vertex_shader;
			ID3D11PixelShader *new_pixel_shader;
			if (LoadShader(hfile, file, &new_vertex_shader, &new_pixel_shader, NULL)) {
				vertex_shader->lpVtbl->Release(vertex_shader);
				pixel_shader->lpVtbl->Release(pixel_shader);
				vertex_shader = new_vertex_shader;
				pixel_shader = new_pixel_shader;
			}
		}
		mod_time = new_mod_time;

		RECT rect;
		GetClientRect(window, &rect);

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)(rect.right - rect.left);
		viewport.Height = (float)(rect.bottom - rect.top);
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;

		Constant_Layout params;

		params.Resolution.x = viewport.Width;
		params.Resolution.y = viewport.Height;

		params.RectMin = RectMin;
		params.RectMax = RectMax;

		params.Center = g_center;
		params.Zoom = g_zoom;
		params.AspectRatio = params.Resolution.x / params.Resolution.y;

		LARGE_INTEGER counts;
		QueryPerformanceCounter(&counts);
		params.Time = (float)((double)counts.QuadPart / (double)frequency.QuadPart);

		if (RecordFrame()) {
			RecordFrameHandled();

			ID3D11Texture2D *texture;
			g_swap_chain->lpVtbl->GetBuffer(g_swap_chain, 1, &IID_ID3D11Texture2D, &texture);

			D3D11_TEXTURE2D_DESC desc;
			texture->lpVtbl->GetDesc(texture, &desc);
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;

			ID3D11Texture2D *texture_to_save = NULL;
			g_device->lpVtbl->CreateTexture2D(g_device, &desc, NULL, &texture_to_save);

			if (texture_to_save) {
				g_device_context->lpVtbl->CopyResource(g_device_context, (ID3D11Resource *)texture_to_save, (ID3D11Resource *)texture);

				D3D11_MAPPED_SUBRESOURCE resource;
				UINT subresource = 0;
				HRESULT hr = g_device_context->lpVtbl->Map(g_device_context, (ID3D11Resource *)texture_to_save, subresource, D3D11_MAP_READ, 0, &resource);
				if (SUCCEEDED(hr)) {
					char filename[MAX_PATH];

					SYSTEMTIME time;
					GetLocalTime(&time);

					snprintf(filename, MAX_PATH, "Capture_%d-%d-%d_%d-%d-%d-%d.png", 
							 (int)time.wYear, (int)time.wMonth, (int)time.wDay, 
							 (int)time.wHour, (int)time.wMinute, (int)time.wSecond, (int)time.wMilliseconds);

					if (!stbi_write_png(filename, (int)desc.Width, (int)desc.Height, 4, resource.pData, (int)resource.RowPitch)) {
						MessageBoxW(window, L"Failed to capture screen", L"Error", MB_OK | MB_ICONEXCLAMATION);
					}
					g_device_context->lpVtbl->Unmap(g_device_context, (ID3D11Resource *)texture_to_save, subresource);
				} else {
					MessageBoxW(window, L"Failed to capture screen", L"Error", MB_OK | MB_ICONEXCLAMATION);
				}

				texture_to_save->lpVtbl->Release(texture_to_save);
			} else {
				MessageBoxW(window, L"Failed to capture screen", L"Error", MB_OK | MB_ICONEXCLAMATION);
			}

			texture->lpVtbl->Release(texture);
		}

		g_device_context->lpVtbl->UpdateSubresource(g_device_context, (ID3D11Resource *)constant_buffer, 0, NULL, &params, 0, 0);

		g_device_context->lpVtbl->RSSetViewports(g_device_context, 1, &viewport);

		g_device_context->lpVtbl->IASetPrimitiveTopology(g_device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		float color[] = { .2f, .2f, .2f, 1.f };
		g_device_context->lpVtbl->ClearRenderTargetView(g_device_context, g_render_target_view, color);
		g_device_context->lpVtbl->OMSetRenderTargets(g_device_context, 1, &g_render_target_view, NULL);

		g_device_context->lpVtbl->IASetVertexBuffers(g_device_context, 0, 1, &vertex_buffer, &stride, &offset);
		g_device_context->lpVtbl->IASetInputLayout(g_device_context, input_layout);

		g_device_context->lpVtbl->PSSetConstantBuffers(g_device_context, 0, 1, &constant_buffer);

		g_device_context->lpVtbl->VSSetShader(g_device_context, vertex_shader, NULL, 0);
		g_device_context->lpVtbl->PSSetShader(g_device_context, pixel_shader, NULL, 0);

		g_device_context->lpVtbl->Draw(g_device_context, 6, 0);

		g_swap_chain->lpVtbl->Present(g_swap_chain, 1, g_tearing_capable ? DXGI_PRESENT_ALLOW_TEARING : 0);

		g_device_context->lpVtbl->ClearState(g_device_context);
	}

	CloseHandle(hfile);

	DirectXShutdown();

	return 0;
}
