#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#if RHI_VULKAN
    #if PLATFORM_WINDOWS
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif
    #include "vulkan/vulkan.h"
#else
    #error Vulkan support is required
#endif

/////////////
// H_FILES

#include "engine_defs.h"
#include "core/program_main.h"

#include "platform/platform.h"
#if PLATFORM_WINDOWS
    #include <windows.h>
    #include <windowsx.h>
    #include "platform/win32/platform_win32.h"
#else
    #error Platform not supported.
#endif

#include "core/types.h"
#include "core/mesh.h"

#include "math/math_core.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "memory/cpu_arena.h"
#include "containers/buffer.h"
#include "input/input.h"

#include "renderer/gpu_arena.h"
#include "renderer/renderer_defs.h"
#include "renderer/renderer_resources.h"
#include "renderer/renderer_swapchain.h"
#include "renderer/renderer_pipeline.h"
#include "renderer/renderer.h"

#include "parsers/parser_json.h"
#include "parsers/parser_gltf.h"

#include "core/world.h"

/////////////
// C_FILES

#include "platform/platform.c"
#if PLATFORM_WINDOWS
    #include "platform/win32/platform_win32.c"
#else
    #error Platform not supported.
#endif

#include "core/mesh.c"

#include "math/math_core.c"
#include "math/vector.c"
#include "math/matrix.c"
#include "math/quaternion.c"
#include "memory/cpu_arena.c"
#include "containers/buffer.c"
#include "input/input.c"

#include "renderer/gpu_arena.c"
#include "renderer/renderer_resources.c"
#include "renderer/renderer_swapchain.c"
#include "renderer/renderer_pipeline.c"
#include "renderer/renderer.c"

#include "parsers/parser_json.c"
#include "parsers/parser_gltf.c"

#include "core/world.c"
