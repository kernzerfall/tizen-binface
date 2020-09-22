#pragma once

#include <tizen.h>
#include <watch_app.h>
#include <watch_app_efl.h>
#include <Elementary.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "binface"

#if !defined(PACKAGE)
#define PACKAGE "kzf.binface"
#endif

#define COLOR_ON 		41,98,255,255
#define COLOR_OFF 		0,0,0,0

#define TEXT_BUF_SIZE 256
