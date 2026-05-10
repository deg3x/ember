#ifndef EDITOR_MAIN_H
#define EDITOR_MAIN_H

#define EMBER_EDITOR 1

inline void editor_main();
inline void editor_camera_update(transform_t* camera_trs);

platform_hnd_t g_editor_window = {0};

#endif // EDITOR_MAIN_H
