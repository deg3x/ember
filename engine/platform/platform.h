#ifndef PLATFORM_H
#define PLATFORM_H

typedef struct platform_info platform_info;
struct platform_info
{
    u64 page_size;
    u64 page_size_large;
    u64 alloc_granularity;
    u64 processor_count;
    u64 perf_cnt_freq;
    f64 perf_cnt_freq_inv;
    f64 inv_freq;
};

typedef struct platform_hnd platform_hnd;
struct platform_hnd
{
    void* hnd;
};

typedef struct platform_wnd_size platform_wnd_size;
struct platform_wnd_size
{
    u32 width;
    u32 height;
};

typedef struct platform_wnd_state platform_wnd_state;
struct platform_wnd_state
{
    b32 is_resizing;
};

typedef struct platform_timer platform_timer;
struct platform_timer
{
    u64 start;
    u64 last;
    u64 now;
};

typedef struct platform_app_state platform_app_state;
struct platform_app_state
{
    platform_hnd   h_stdin;
    platform_hnd   h_stdout;
    platform_hnd   h_stderr;
    platform_timer timer;
    b32            is_running;
};

typedef u32 platform_file_flags;
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

typedef struct platform_file_info platform_file_info;
struct platform_file_info
{
    u64 size;
};

global platform_info      g_platform_info;
global platform_app_state g_program_state;
global platform_wnd_state g_window_state;

void platform_info_init();
void platform_abort(i32 exit_code);

void platform_console_init();

platform_timer platform_timer_init();
void           platform_timer_update(platform_timer* timer);
f64            platform_timer_since_start(platform_timer timer);
f64            platform_timer_delta(platform_timer timer);

void* platform_mem_reserve(u64 size);
void* platform_mem_reserve_large(u64 size);
b32   platform_mem_commit(void* ptr, u64 size);
b32   platform_mem_commit_large(void* ptr, u64 size);
void  platform_mem_release(void* ptr, u64 size);
void  platform_mem_decommit(void* ptr, u64 size);

platform_hnd       platform_file_open(const c8* file_path, platform_file_flags flags);
void               platform_file_close(platform_hnd file_handle);
platform_file_info platform_file_info_get(platform_hnd file_handle);
u64                platform_file_write(platform_hnd file_handle, void* data, u64 write_size);
u64                platform_file_read(platform_hnd file_handle, void* data, u64 read_size);
u64                platform_file_data(const c8* file_path, void* data);

b32          platform_handle_equal(platform_hnd lhs, platform_hnd rhs);
platform_hnd platform_get_instance_handle();

void              platform_gfx_init();
void              platform_gfx_process_events();
platform_hnd      platform_gfx_wnd_create(const c8* title);
platform_wnd_size platform_gfx_wnd_get_size(platform_hnd window_handle);
platform_wnd_size platform_gfx_wnd_client_get_size(platform_hnd window_handle);
b32               platform_gfx_wnd_is_minimized(platform_hnd window_handle);

#endif // PLATFORM_H
