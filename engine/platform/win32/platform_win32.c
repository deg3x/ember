void platform_info_init()
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    QueryPerformanceFrequency((LARGE_INTEGER *)&g_platform_info.perf_cnt_freq);

    g_platform_info.page_size_large   = GetLargePageMinimum();
    g_platform_info.page_size         = system_info.dwPageSize;
    g_platform_info.processor_count   = system_info.dwNumberOfProcessors;
    g_platform_info.alloc_granularity = system_info.dwAllocationGranularity;
    g_platform_info.perf_cnt_freq_inv = 1.0 / (f64)g_platform_info.perf_cnt_freq;
}

void platform_abort(i32 exit_code)
{
    ExitProcess(exit_code);
}

void platform_console_init()
{
#if EMBER_BUILD_CONSOLE
    AllocConsole();

    g_program_state.h_stdin.hnd  = GetStdHandle(STD_INPUT_HANDLE);
    g_program_state.h_stdout.hnd = GetStdHandle(STD_OUTPUT_HANDLE);
    g_program_state.h_stderr.hnd = GetStdHandle(STD_ERROR_HANDLE);

    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
}

platform_timer platform_timer_init()
{
    platform_timer result;

    QueryPerformanceCounter((LARGE_INTEGER *)&result.start);

    result.last = result.start;
    result.now  = result.start;

    return result;
}

void platform_timer_update(platform_timer* timer)
{
    timer->last = timer->now;

    QueryPerformanceCounter((LARGE_INTEGER *)&timer->now);
}

f64 platform_timer_since_start(platform_timer timer)
{
    u64 time_diff = timer.now - timer.start;
    f64 time      = g_platform_info.perf_cnt_freq_inv * (f64)time_diff;

    return time;
}

f64 platform_timer_delta(platform_timer timer)
{
    u64 time_diff  = timer.now - timer.last;
    f64 delta_time = g_platform_info.perf_cnt_freq_inv * (f64)time_diff;

    return delta_time;
}

void* platform_mem_reserve(u64 size)
{
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);

    return result;
}

void* platform_mem_reserve_large(u64 size)
{
    // NOTE(KB): Windows requires large pages to be committed on reserve
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);

    return result;
}

b32 platform_mem_commit(void* ptr, u64 size)
{
    b32 result = (b32)(VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);

    return result;
}

b32 platform_mem_commit_large(void* ptr, u64 size)
{
    return EMBER_TRUE;
}

void platform_mem_release(void* ptr, u64 size)
{
    // NOTE(KB): Windows requires size to be 0 for release
    VirtualFree(ptr, 0, MEM_RELEASE);
}

void platform_mem_decommit(void* ptr, u64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

platform_hnd platform_file_open(const c8* file_path, platform_file_flags flags)
{
    DWORD access_flags = 0;
    DWORD share_flags  = 0;
    DWORD create_disp  = 0;

    if (flags & PLATFORM_FILE_FLAGS_read)
    {
        access_flags |= GENERIC_READ;
        create_disp   = OPEN_EXISTING;
    }
    if (flags & PLATFORM_FILE_FLAGS_exec)
    {
        access_flags |= GENERIC_EXECUTE;
        create_disp   = OPEN_EXISTING;
    }
    if (flags & PLATFORM_FILE_FLAGS_write)
    {
        access_flags |= GENERIC_WRITE;
        create_disp   = CREATE_ALWAYS;
    }
    if (flags & PLATFORM_FILE_FLAGS_append)
    {
        access_flags |= FILE_GENERIC_WRITE ^ FILE_WRITE_DATA;
        create_disp   = OPEN_ALWAYS;
    }
    if (flags & PLATFORM_FILE_FLAGS_share_r)
    {
        share_flags |= FILE_SHARE_READ;
    }
    if (flags & PLATFORM_FILE_FLAGS_share_w)
    {
        share_flags |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    }

    HANDLE file_handle = CreateFile(
        file_path,
        access_flags,
        share_flags,
        NULL,
        create_disp,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    platform_hnd result = {0};

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        return result;
    }

    result.hnd = file_handle;

    return result;
}

void platform_file_close(platform_hnd file_handle)
{
    if (file_handle.hnd == NULL)
    {
        return;
    }

    BOOL result = CloseHandle(file_handle.hnd);

    (void)result;
}

platform_file_info platform_file_info_get(platform_hnd file_handle)
{
    platform_file_info result = {0};

    if (file_handle.hnd == NULL)
    {
        return result;
    }

    BY_HANDLE_FILE_INFORMATION file_info;
    BOOL info_valid = GetFileInformationByHandle(file_handle.hnd, &file_info);

    if (!info_valid)
    {
        return result;
    }

    result.size |= ((u64)file_info.nFileSizeHigh) << 32;
    result.size |= (u64)file_info.nFileSizeLow;

    return result;
}

u64 platform_file_write(platform_hnd file_handle, void* data, u64 write_size)
{
    if (file_handle.hnd == NULL)
    {
        return 0;
    }

    u64 bytes_written_total = 0;

    while (bytes_written_total < write_size)
    {
        u64 bytes_to_write_ttl = write_size - bytes_written_total;
        DWORD bytes_to_write   = U32_FROM_U64_CLAMPED(bytes_to_write_ttl);
        DWORD bytes_written;

        BOOL write_success = WriteFile(file_handle.hnd, data, bytes_to_write, &bytes_written, NULL);
        if (!write_success)
        {
            break;
        }

        bytes_written_total += bytes_written;
    }

    return bytes_written_total;
}

// TODO(KB): Add reading from specified position (OVERLAPPED type)
u64 platform_file_read(platform_hnd file_handle, void* data, u64 read_size)
{
    if (file_handle.hnd == NULL)
    {
        return 0;
    }

    u64 file_size;
    BOOL size_success = GetFileSizeEx(file_handle.hnd, (PLARGE_INTEGER)&file_size);
    if (!size_success)
    {
        return 0;
    }

    u64 bytes_read_total  = 0;
    u64 read_size_clamped = MIN(read_size, file_size);

    while (bytes_read_total < read_size_clamped)
    {
        u64 bytes_to_read_ttl = read_size_clamped - bytes_read_total;
        DWORD bytes_to_read   = U32_FROM_U64_CLAMPED(bytes_to_read_ttl);
        DWORD bytes_read;

        BOOL read_success = ReadFile(file_handle.hnd, data, bytes_to_read, &bytes_read, NULL);
        if (!read_success)
        {
            break;
        }

        bytes_read_total += bytes_read;

        if (bytes_read != bytes_to_read)
        {
            break;
        }
    }

    return bytes_read_total;
}

platform_hnd platform_get_instance_handle()
{
    platform_hnd handle = {
        .hnd = GetModuleHandle(NULL)
    };

    return handle;
}

void platform_gfx_init()
{
    g_win32_gfx_state.instance = GetModuleHandle(NULL);

    WNDCLASSEX window_class    = {0};
    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc   = win32_wnd_msg_callback;
    window_class.cbClsExtra    = 0;
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = g_win32_gfx_state.instance;
    window_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName  = NULL;
    window_class.lpszClassName = "ember-window-class";
    window_class.hIconSm       = NULL;

    ATOM atom = RegisterClassEx(&window_class);
    (void)atom;
}

void platform_gfx_process_events()
{
    MSG msg = {0};

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            g_program_state.is_running = EMBER_FALSE;
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

platform_hnd platform_gfx_wnd_create(const c8* window_name)
{
    win32_wnd window = {0};

    HWND window_handle = CreateWindowEx(
        0,
        "ember-window-class",
        window_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        g_win32_gfx_state.instance,
        NULL
    );

    EMBER_ASSERT(window_handle != NULL);

    DragAcceptFiles(window_handle, 1);

    window.handle     = window_handle;
    window.device_ctx = GetDC(window_handle);

    platform_hnd handle = {window_handle};

    return handle;
}

platform_wnd_size platform_gfx_wnd_get_size(platform_hnd window_handle)
{
    platform_wnd_size result = {0};

    if (IsWindow(window_handle.hnd))
    {
        RECT window_rect;

        GetWindowRect(window_handle.hnd, &window_rect);

        result.width  = window_rect.right - window_rect.left;
        result.height = window_rect.bottom - window_rect.top;
    }

    return result;
}

platform_wnd_size platform_gfx_wnd_client_get_size(platform_hnd window_handle)
{
    platform_wnd_size result = {0};

    if (IsWindow(window_handle.hnd))
    {
        RECT client_rect;

        GetWindowRect(window_handle.hnd, &client_rect);

        result.width  = client_rect.right - client_rect.left;
        result.height = client_rect.bottom - client_rect.top;
    }

    return result;
}

b32 platform_gfx_wnd_is_minimized(platform_hnd window_handle)
{
    b32 result = EMBER_FALSE;

    if (IsWindow(window_handle.hnd))
    {
        result = !!(IsIconic(window_handle.hnd));
    }

    return result;
}

LRESULT CALLBACK win32_wnd_msg_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_ENTERSIZEMOVE:
        {
            g_window_state.is_resizing = EMBER_TRUE;
            return 0;
        }
        case WM_EXITSIZEMOVE:
        {
            g_window_state.is_resizing = EMBER_FALSE;
            return 0;
        }
        case WM_ACTIVATEAPP:
        {
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, i32 show_cmd)
{
    g_program_state.is_running = EMBER_TRUE;
    g_program_state.timer      = platform_timer_init();

    platform_info_init();
    platform_console_init();
    platform_gfx_init();

    platform_hnd window_handle = platform_gfx_wnd_create("Ember Engine");

    renderer_init(window_handle);

    ShowWindow((HWND)window_handle.hnd, SW_SHOW);

    for(;;)
    {
        platform_gfx_process_events();
        platform_timer_update(&g_program_state.timer);

        // f32_t delta_time = (f32_t)platform_timer_delta(&g_program_state.timer);

        if (!g_program_state.is_running)
        {
            break;
        }

        renderer_update(window_handle);
    }

    renderer_destroy();

    FreeConsole();

    return 0;
}

