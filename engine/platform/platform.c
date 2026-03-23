u64 platform_file_data(const c8* file_path, void* data)
{
    platform_file_flags read_flags = PLATFORM_FILE_FLAGS_read;
    platform_hnd file_handle       = platform_file_open(file_path, read_flags);
    platform_file_info props       = platform_file_info_get(file_handle);

    u64 result = platform_file_read(file_handle, data, props.size);
    platform_file_close(file_handle);

    return result;
}

b32 platform_handle_equal(platform_hnd lhs, platform_hnd rhs)
{
    return lhs.hnd == rhs.hnd;
}
