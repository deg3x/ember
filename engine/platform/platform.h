#ifndef PLATFORM_H
#define PLATFORM_H

typedef struct platform_info_t platform_info_t;
struct platform_info_t
{
    u64 page_size;
    u64 page_size_large;
    u64 alloc_granularity;
    u64 processor_count;
    u64 perf_cnt_freq;
    f64 perf_cnt_freq_inv;
    f64 inv_freq;
};

typedef struct platform_hnd_t platform_hnd_t;
struct platform_hnd_t
{
    void* hnd;
};

typedef struct platform_wnd_size_t platform_wnd_size_t;
struct platform_wnd_size_t
{
    u32 width;
    u32 height;
};

typedef struct platform_wnd_state_t platform_wnd_state_t;
struct platform_wnd_state_t
{
    b32 is_resizing;
};

typedef struct platform_timer_t platform_timer_t;
struct platform_timer_t
{
    u64 start;
    u64 last;
    u64 now;
};

typedef struct platform_app_state_t platform_app_state_t;
struct platform_app_state_t
{
    platform_hnd_t   h_stdin;
    platform_hnd_t   h_stdout;
    platform_hnd_t   h_stderr;
    platform_timer_t timer;
    b32              is_running;
};

typedef u32 platform_file_flags_t;
enum
{
    PLATFORM_FILE_FLAGS_read    = (1 << 0),
    PLATFORM_FILE_FLAGS_write   = (1 << 1),
    PLATFORM_FILE_FLAGS_exec    = (1 << 2),
    PLATFORM_FILE_FLAGS_append  = (1 << 3),
    PLATFORM_FILE_FLAGS_share_r = (1 << 4),
    PLATFORM_FILE_FLAGS_share_w = (1 << 5),
};

#define FILE_READ_ALL U64_MAX

typedef struct platform_file_info_t platform_file_info_t;
struct platform_file_info_t
{
    u64 size;
};

global platform_info_t      g_platform_info;
global platform_app_state_t g_program_state;
global platform_wnd_state_t g_window_state;
global i32                  g_platform_input_map[256];

internal void platform_init();
internal void platform_abort(i32 exit_code);

internal void platform_console_init();
internal void platform_console_destroy();

internal platform_timer_t platform_timer_init();
internal void             platform_timer_update(platform_timer_t* timer);
internal f64              platform_timer_since_start(platform_timer_t timer);
internal f64              platform_timer_delta(platform_timer_t timer);

internal b32  platform_cursor_set_pos(platform_hnd_t window_handle, i32 x, i32 y);
internal void platform_cursor_set_vis(b32 visible);

internal void* platform_mem_reserve(u64 size);
internal void* platform_mem_reserve_large(u64 size);
internal b32   platform_mem_commit(void* ptr, u64 size);
internal b32   platform_mem_commit_large(void* ptr, u64 size);
internal void  platform_mem_release(void* ptr, u64 size);
internal void  platform_mem_decommit(void* ptr, u64 size);

internal platform_hnd_t       platform_file_open(const c8* file_path, platform_file_flags_t flags);
internal void                 platform_file_close(platform_hnd_t file_handle);
internal platform_file_info_t platform_file_info_get(platform_hnd_t file_handle);
internal u64                  platform_file_write(platform_hnd_t file_handle, void* data, u64 write_size);
internal u64                  platform_file_read(platform_hnd_t file_handle, void* data, u64 read_size);
internal u64                  platform_file_data(const c8* file_path, void* data);

internal b32            platform_handle_equal(platform_hnd_t lhs, platform_hnd_t rhs);
internal platform_hnd_t platform_get_instance_handle();

internal void                platform_gfx_process_events();
internal platform_hnd_t      platform_gfx_wnd_create(const c8* title);
internal void                platform_gfx_wnd_show(platform_hnd_t window_handle);
internal platform_wnd_size_t platform_gfx_wnd_get_size(platform_hnd_t window_handle);
internal platform_wnd_size_t platform_gfx_wnd_client_get_size(platform_hnd_t window_handle);
internal b32                 platform_gfx_wnd_is_minimized(platform_hnd_t window_handle);

#endif // PLATFORM_H
