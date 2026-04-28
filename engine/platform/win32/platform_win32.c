internal void platform_init()
{
    //////////////////////////////////////////
    // NOTE(KB): Platform info initialization
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    QueryPerformanceFrequency((LARGE_INTEGER *)&g_platform_info.perf_cnt_freq);

    g_platform_info.page_size_large   = GetLargePageMinimum();
    g_platform_info.page_size         = system_info.dwPageSize;
    g_platform_info.processor_count   = system_info.dwNumberOfProcessors;
    g_platform_info.alloc_granularity = system_info.dwAllocationGranularity;
    g_platform_info.perf_cnt_freq_inv = 1.0 / (f64)g_platform_info.perf_cnt_freq;

    //////////////////////////////////////////
    // NOTE(KB): GFX initialization
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

    //////////////////////////////////////////
    // NOTE(KB): Input map initialization
    g_platform_input_map[VK_LBUTTON]    = INPUT_KEY_lmb;
    g_platform_input_map[VK_RBUTTON]    = INPUT_KEY_rmb;
    g_platform_input_map[VK_MBUTTON]    = INPUT_KEY_mmb;
    g_platform_input_map[VK_XBUTTON1]   = INPUT_KEY_xb1;
    g_platform_input_map[VK_XBUTTON2]   = INPUT_KEY_xb2;
    g_platform_input_map[VK_BACK]       = INPUT_KEY_bspace;
    g_platform_input_map[VK_TAB]        = INPUT_KEY_tab;
    g_platform_input_map[VK_RETURN]     = INPUT_KEY_enter;
    g_platform_input_map[VK_SHIFT]      = INPUT_KEY_shift;
    g_platform_input_map[VK_CONTROL]    = INPUT_KEY_ctrl;
    g_platform_input_map[VK_MENU]       = INPUT_KEY_alt;
    g_platform_input_map[VK_PAUSE]      = INPUT_KEY_pause;
    g_platform_input_map[VK_CAPITAL]    = INPUT_KEY_caps;
    g_platform_input_map[VK_ESCAPE]     = INPUT_KEY_esc;
    g_platform_input_map[VK_SPACE]      = INPUT_KEY_space;
    g_platform_input_map[VK_PRIOR]      = INPUT_KEY_pgup;
    g_platform_input_map[VK_NEXT]       = INPUT_KEY_pgdn;
    g_platform_input_map[VK_END]        = INPUT_KEY_end;
    g_platform_input_map[VK_HOME]       = INPUT_KEY_home;
    g_platform_input_map[VK_LEFT]       = INPUT_KEY_left;
    g_platform_input_map[VK_UP]         = INPUT_KEY_up;
    g_platform_input_map[VK_RIGHT]      = INPUT_KEY_right;
    g_platform_input_map[VK_DOWN]       = INPUT_KEY_down;
    g_platform_input_map[VK_INSERT]     = INPUT_KEY_ins;
    g_platform_input_map[VK_DELETE]     = INPUT_KEY_del;
    g_platform_input_map[VK_MULTIPLY]   = INPUT_KEY_mul;
    g_platform_input_map[VK_ADD]        = INPUT_KEY_add;
    g_platform_input_map[VK_SEPARATOR]  = INPUT_KEY_separator;
    g_platform_input_map[VK_SUBTRACT]   = INPUT_KEY_sub;
    g_platform_input_map[VK_DECIMAL]    = INPUT_KEY_decimal;
    g_platform_input_map[VK_DIVIDE]     = INPUT_KEY_div;
    g_platform_input_map[VK_NUMLOCK]    = INPUT_KEY_numlck;
    g_platform_input_map[VK_SCROLL]     = INPUT_KEY_scrlck;
    g_platform_input_map[VK_LSHIFT]     = INPUT_KEY_lshift;
    g_platform_input_map[VK_RSHIFT]     = INPUT_KEY_rshift;
    g_platform_input_map[VK_LCONTROL]   = INPUT_KEY_lctrl;
    g_platform_input_map[VK_RCONTROL]   = INPUT_KEY_rctrl;
    g_platform_input_map[VK_LMENU]      = INPUT_KEY_lalt;
    g_platform_input_map[VK_RMENU]      = INPUT_KEY_ralt;
    g_platform_input_map[VK_OEM_1]      = INPUT_KEY_colon;
    g_platform_input_map[VK_OEM_PLUS]   = INPUT_KEY_plus;
    g_platform_input_map[VK_OEM_COMMA]  = INPUT_KEY_comma;
    g_platform_input_map[VK_OEM_MINUS]  = INPUT_KEY_minus;
    g_platform_input_map[VK_OEM_PERIOD] = INPUT_KEY_period;
    g_platform_input_map[VK_OEM_2]      = INPUT_KEY_fslash;
    g_platform_input_map[VK_OEM_3]      = INPUT_KEY_tilde;
    g_platform_input_map[VK_OEM_4]      = INPUT_KEY_lbrace;
    g_platform_input_map[VK_OEM_5]      = INPUT_KEY_bslash;
    g_platform_input_map[VK_OEM_6]      = INPUT_KEY_rbrace;
    g_platform_input_map[VK_OEM_7]      = INPUT_KEY_quote;

    for (i32 i = '0'; i <= '9'; i++)
    {
        i32 offset = i - (i32)('0');

        g_platform_input_map[i] = INPUT_KEY_0 + offset;
    }

    for (i32 i = 'A'; i <= 'Z'; i++)
    {
        i32 offset = i - (i32)('A');

        g_platform_input_map[i] = INPUT_KEY_a + offset;
    }

    for (i32 i = VK_NUMPAD0; i <= VK_NUMPAD9; i++)
    {
        i32 offset = i - VK_NUMPAD0;

        g_platform_input_map[i] = INPUT_KEY_numpad_0 + offset;
    }

    for (i32 i = VK_F1; i <= VK_F24; i++)
    {
        i32 offset = i - VK_F1;

        g_platform_input_map[i] = INPUT_KEY_f1 + offset;
    }

    for (i32 i = VK_F1; i <= VK_F24; i++)
    {
        i32 offset = i - VK_F1;

        g_platform_input_map[i] = INPUT_KEY_f1 + offset;
    }
}

internal void platform_abort(i32 exit_code)
{
    ExitProcess(exit_code);
}


internal void platform_console_init()
{
    AllocConsole();

    g_program_state.h_stdin.hnd  = GetStdHandle(STD_INPUT_HANDLE);
    g_program_state.h_stdout.hnd = GetStdHandle(STD_OUTPUT_HANDLE);
    g_program_state.h_stderr.hnd = GetStdHandle(STD_ERROR_HANDLE);

    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

internal void platform_console_destroy()
{
    FreeConsole();
}

internal platform_timer_t platform_timer_init()
{
    platform_timer_t result;

    QueryPerformanceCounter((LARGE_INTEGER *)&result.start);

    result.last = result.start;
    result.now  = result.start;

    return result;
}

internal void platform_timer_update(platform_timer_t* timer)
{
    timer->last = timer->now;

    QueryPerformanceCounter((LARGE_INTEGER *)&timer->now);
}

internal f64 platform_timer_since_start(platform_timer_t timer)
{
    u64 time_diff = timer.now - timer.start;
    f64 time      = g_platform_info.perf_cnt_freq_inv * (f64)time_diff;

    return time;
}

internal f64 platform_timer_delta(platform_timer_t timer)
{
    u64 time_diff  = timer.now - timer.last;
    f64 delta_time = g_platform_info.perf_cnt_freq_inv * (f64)time_diff;

    return delta_time;
}

internal void* platform_mem_reserve(u64 size)
{
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);

    return result;
}

internal void* platform_mem_reserve_large(u64 size)
{
    // NOTE(KB): Windows requires large pages to be committed on reserve
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);

    return result;
}

internal b32 platform_mem_commit(void* ptr, u64 size)
{
    b32 result = (b32)(VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);

    return result;
}

internal b32 platform_mem_commit_large(void* ptr, u64 size)
{
    (void)ptr;
    (void)size;

    return EMBER_TRUE;
}

internal void platform_mem_release(void* ptr, u64 size)
{
    (void)size;

    // NOTE(KB): Windows requires size to be 0 for release
    VirtualFree(ptr, 0, MEM_RELEASE);
}

internal void platform_mem_decommit(void* ptr, u64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal platform_hnd_t platform_file_open(const c8* file_path, platform_file_flags_t flags)
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

    platform_hnd_t result = {0};

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        return result;
    }

    result.hnd = file_handle;

    return result;
}

internal void platform_file_close(platform_hnd_t file_handle)
{
    if (file_handle.hnd == NULL)
    {
        return;
    }

    BOOL result = CloseHandle(file_handle.hnd);

    (void)result;
}

internal platform_file_info_t platform_file_info_get(platform_hnd_t file_handle)
{
    platform_file_info_t result = {0};

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

internal u64 platform_file_write(platform_hnd_t file_handle, void* data, u64 write_size)
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
internal u64 platform_file_read(platform_hnd_t file_handle, void* data, u64 read_size)
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

internal platform_hnd_t platform_get_instance_handle()
{
    platform_hnd_t handle = {
        .hnd = GetModuleHandle(NULL)
    };

    return handle;
}

internal void platform_gfx_process_events()
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

internal platform_hnd_t platform_gfx_wnd_create(const c8* window_name)
{
    win32_wnd_t window = {0};

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

    platform_hnd_t handle = {window_handle};

    return handle;
}

internal void platform_gfx_wnd_show(platform_hnd_t window_handle)
{
    ShowWindow((HWND)window_handle.hnd, SW_SHOW);
}

internal platform_wnd_size_t platform_gfx_wnd_get_size(platform_hnd_t window_handle)
{
    platform_wnd_size_t result = {0};

    if (IsWindow(window_handle.hnd))
    {
        RECT window_rect;

        GetWindowRect(window_handle.hnd, &window_rect);

        result.width  = window_rect.right - window_rect.left;
        result.height = window_rect.bottom - window_rect.top;
    }

    return result;
}

internal platform_wnd_size_t platform_gfx_wnd_client_get_size(platform_hnd_t window_handle)
{
    platform_wnd_size_t result = {0};

    if (IsWindow(window_handle.hnd))
    {
        RECT client_rect;

        GetWindowRect(window_handle.hnd, &client_rect);

        result.width  = client_rect.right - client_rect.left;
        result.height = client_rect.bottom - client_rect.top;
    }

    return result;
}

internal b32 platform_gfx_wnd_is_minimized(platform_hnd_t window_handle)
{
    b32 result = EMBER_FALSE;

    if (IsWindow(window_handle.hnd))
    {
        result = !!(IsIconic(window_handle.hnd));
    }

    return result;
}

internal LRESULT CALLBACK win32_wnd_msg_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
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
        case WM_LBUTTONUP:
        {
            g_input_state.keys[INPUT_KEY_lmb] = 0;

            return 0;
        }
        case WM_RBUTTONUP:
        {
            g_input_state.keys[INPUT_KEY_rmb] = 0;

            return 0;
        }
        case WM_MBUTTONUP:
        {
            g_input_state.keys[INPUT_KEY_mmb] = 0;

            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            g_input_state.keys[INPUT_KEY_lmb] = 1;

            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            g_input_state.keys[INPUT_KEY_rmb] = 1;

            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            g_input_state.keys[INPUT_KEY_mmb] = 1;

            return 0;
        }
        case WM_MOUSEMOVE:
        {
            g_input_state.mouse_pos.x = (f32)GET_X_LPARAM(l_param);
            g_input_state.mouse_pos.y = (f32)GET_Y_LPARAM(l_param);

            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            g_input_state.mouse_scroll.y = (f32)GET_WHEEL_DELTA_WPARAM(w_param);

            return 0;
        }
        case WM_MOUSEHWHEEL:
        {
            g_input_state.mouse_scroll.x = (f32)GET_WHEEL_DELTA_WPARAM(w_param);

            return 0;
        }
        //case WM_SYSKEYDOWN:
        //case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            i32 key_code = g_platform_input_map[w_param];
            b8 key_state = !(l_param & (1 << 31));

            g_input_state.keys[key_code] = key_state;

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, i32 show_cmd)
{
    (void)instance;
    (void)prev_instance;
    (void)cmd_line;
    (void)show_cmd;

    g_program_state.is_running = EMBER_TRUE;
    g_program_state.timer      = platform_timer_init();

    platform_init();

#if EMBER_BUILD_CONSOLE
    platform_console_init();
#endif

    program_main();

#if EMBER_BUILD_CONSOLE
    platform_console_destroy();
#endif

    return 0;
}

