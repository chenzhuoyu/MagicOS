/*
 * ui.c
 *
 *  Created on: 2012-12-11
 *      Author: desperedo
 *
 *	stupid text-based ui.....
 */

#include <ui.h>

#include <thread.h>
#include <console.h>
#include <colors.h>
#include <string.h>
#include <stdlib.h>
#include <interrupt.h>
#include <keyboard.h>
#include <event.h>
#include <slab.h>

#define LT						201
#define LB						200
#define RT						187
#define RB						188

#define HR						205
#define VR						186

#define BACKGROUND				MAKE_COLOR(BLUE, BLUE)

#define WINDOW_TEXT				MAKE_COLOR(BLACK, LIGHT_GRAY)
#define WINDOW_BORDER			MAKE_COLOR(WHITE, LIGHT_GRAY)
#define WINDOW_SHADOW			MAKE_COLOR(BLACK, BLACK)

#define HIGHLIGHT_TEXT			MAKE_COLOR(RED, LIGHT_GRAY)

typedef struct _window_t
{
	int x, y, w, h;
} window_t;

static k_event_t *g_status = NULL;
static k_event_t *g_status_ok = NULL;

static k_event_t *g_done[4] = {0};
static k_event_t *g_event[4] = {0};

static uchar g_show[4] = {0};

static ushort g_screen[80 * 25] = {0};
static uchar g_order[4] = { 3, 2, 1, 0 };

static window_t g_windows[4];

static inline void s_update()
{
	k_setxy(0xFF, 0xFF);

	memcpy(k_video_mem, g_screen, sizeof(g_screen));
}

static inline void s_clear(uchar attr)
{
	for (int i = 0; i < 80 * 25; i++) g_screen[i] = attr << 8;
}

static inline void s_putchar(uchar c, int x, int y, uchar attr)
{
	if (x < 80 && y < 25) g_screen[x + y * 80] = c | (attr << 8);
}

static inline void draw_rect(int x, int y, int w, int h, uchar attr)
{
	s_putchar(LT, x, y, attr);
	s_putchar(LB, x, y + h - 1, attr);
	s_putchar(RT, x + w - 1, y, attr);
	s_putchar(RB, x + w - 1, y + h - 1, attr);

	for (int i = 0; i < w - 2; i++)
	{
		s_putchar(HR, x + i + 1, y, attr);
		s_putchar(HR, x + i + 1, y + h - 1, attr);
	}

	for (int i = 0; i < h - 2; i++)
	{
		s_putchar(VR, x,         y + i + 1, attr);
		s_putchar(VR, x + w - 1, y + i + 1, attr);

		for (int j = 0; j < w - 2; j++) s_putchar(' ', x + j + 1, y + i + 1, attr);
	}
}

static inline void draw_text(const char *s, int x, int y, uchar attr)
{
	while (*s) s_putchar(*s++, x++, y, attr);
}

static void draw_status(const char *text)
{
	int x = 1;

	while (*text)
	{
		uchar attr = WINDOW_TEXT;

		if (*text == '$')
		{
			text++;
			attr = HIGHLIGHT_TEXT;

			if (*text == '$') attr = WINDOW_TEXT;
		}

		s_putchar(*text++, x++, 24, attr);
	}
}

static void draw_menu(const char *text, int x)
{
	while (*text)
	{
		uchar attr = WINDOW_TEXT;

		if (*text == '$')
		{
			text++;
			attr = HIGHLIGHT_TEXT;

			if (*text == '$') attr = WINDOW_TEXT;
		}

		s_putchar(*text++, x++, 0, attr);
	}
}

static void draw_window(const char *title, int index)
{
	int len = strlen(title);

	draw_rect(g_windows[index].x, g_windows[index].y, g_windows[index].w, g_windows[index].h, WINDOW_BORDER);
	draw_text(title, g_windows[index].x + (g_windows[index].w - len) / 2, g_windows[index].y, WINDOW_BORDER);

	s_putchar(' ', g_windows[index].x + (g_windows[index].w - len) / 2 - 1, g_windows[index].y, WINDOW_BORDER);
	s_putchar(' ', g_windows[index].x + (g_windows[index].w - len) / 2 + len, g_windows[index].y, WINDOW_BORDER);

	for (int i = 0; i < g_windows[index].w; i++)
		s_putchar(' ', g_windows[index].x + i + 1, g_windows[index].y + g_windows[index].h, WINDOW_SHADOW);

	for (int i = 0; i < g_windows[index].h; i++)
	{
		s_putchar(' ', g_windows[index].x + g_windows[index].w,		g_windows[index].y + i + 1, WINDOW_SHADOW);
		s_putchar(' ', g_windows[index].x + g_windows[index].w + 1,	g_windows[index].y + i + 1, WINDOW_SHADOW);
	}
}

static void draw_label(const char *s, int window, int x, int y)
{
	x += g_windows[window].x + 1;
	y += g_windows[window].y + 1;

	draw_text(s, x, y, WINDOW_TEXT);
}

static void set_window(int index, int x, int y, int w, int h)
{
	g_windows[index].x = x;
	g_windows[index].y = y;
	g_windows[index].w = w;
	g_windows[index].h = h;
}

static int application_1(void *param)
{
	int n = 0;
	int p = 0;

	set_window(0, 5, 2, 30, 6);

	for (;;)
	{
		k_event_wait(g_event[0], INFINITE);

		draw_window("hello world", 0);
		draw_label("hello, world", 0, p + 3, 2);

		if (++n == 10)
		{
			n = 0;

			if (++p == 10) p = 0;
		}

		k_event_signal(g_done[0]);
	}

	return 0;
}

static int application_2(void *param)
{
	set_window(1, 10, 4, 30, 6);

	for (;;)
	{
		char buffer[16];

		k_event_wait(g_event[1], INFINITE);

		sprintf(buffer, "%-8d, %d", rand(), k_sys_tick);

		draw_window("random and ticks", 1);
		draw_label(buffer, 1, 5, 2);

		k_event_signal(g_done[1]);
	}

	return 0;
}

static int application_3(void *param)
{
	set_window(2, 15, 6, 30, 8);

	for (;;)
	{
		char *strs[] =
		{
			"      tcb :",
			"     wait :",
			"    event :",
			"semaphore :"
		};

		k_event_wait(g_event[2], INFINITE);

		draw_window("slab usage", 2);

		for (int i = 0; i < SLAB_CACHE_TYPES; i++)
		{
			char buffer[16];

			sprintf(buffer, "%d", k_slab_usage[i]);

			draw_label(strs[i], 2, 3, i + 1);
			draw_label(buffer, 2, 15, i + 1);
		}

		k_event_signal(g_done[2]);
	}

	return 0;
}

static int application_4(void *param)
{
	set_window(3, 20, 8, 40, 10);

	for (;;)
	{
		k_event_wait(g_event[3], INFINITE);

		draw_window("about", 3);
		draw_label("This is just a demostration", 3, 5, 2);
		draw_label("that we designed for months ;)", 3, 4, 3);
		draw_label("Oxygen and Sun, MagicBox", 3, 10, 6);

		k_event_signal(g_done[3]);
	}

	return 0;
}

static int about_status_bar(void *param)
{
	char about[] = "Magix demostration kernel, produced by Oxygen/Sun.                            ";

	for (int i = 1, n = strlen(about);; i++, i %= 10)
	{
		k_event_wait(g_status, INFINITE);

		draw_status(about);

		if (i == 0)
		{
			char c = about[0];

			memcpy(about, about + 1, n - 1);

			about[n - 1] = c;
		}

		k_event_signal(g_status_ok);
	}

	return 0;
}

static void put_top(int app)
{
	g_show[app] = 1;

	for (int i = 0; i < 4; i++)
	{
		if (g_order[i] == app)
		{
			for (int j = i - 1; j >= 0; j--) g_order[j + 1] = g_order[j];

			g_order[0] = app;

			return;
		}
	}
}

static int uid(void *param)
{
	k_thread_new(about_status_bar, NULL);

	for (;;)
	{
		s_clear(BACKGROUND);

		for (int i = 3; i >= 0; i--)
		{
			if (g_show[g_order[i]])
			{
				k_event_signal(g_event[g_order[i]]);
				k_event_wait(g_done[g_order[i]], INFINITE);
			}
		}

		for (int i = 0; i < 80; i++)
		{
			s_putchar(' ', i, 0, MAKE_COLOR(BLACK, LIGHT_GRAY));
			s_putchar(' ', i, 24, MAKE_COLOR(BLACK, LIGHT_GRAY));
		}

		draw_menu("$F$1 hello, world app"		, 2);
		draw_menu("$F$2 random number app"		, 24);
		draw_menu("$F$3 show SLAB usage"		, 47);
		draw_menu("$F$4 about us"				, 67);

		k_event_signal(g_status);
		k_event_wait(g_status_ok, INFINITE);

		s_update();
	}

	return 0;
}

static int inputd(void *param)
{
	srand(k_sys_tick);

	g_status = k_event_new();
	g_status_ok = k_event_new();

	for (int i = 0; i < 4; i++)
	{
		g_done[i] = k_event_new();
		g_event[i] = k_event_new();
	}

	k_thread_new(application_1, NULL);
	k_thread_new(application_2, NULL);
	k_thread_new(application_3, NULL);
	k_thread_new(application_4, NULL);

	k_thread_new(uid, NULL);

	for (;;)
	{
		switch (k_getch())
		{
			case VK_F1: put_top(0); break;
			case VK_F2: put_top(1); break;
			case VK_F3: put_top(2); break;
			case VK_F4: put_top(3); break;

			case VK_UP:		g_windows[g_order[0]].y = Max(1, g_windows[g_order[0]].y - 1); break;
			case VK_LEFT:	g_windows[g_order[0]].x = Max(0, g_windows[g_order[0]].x - 1); break;

			case VK_DOWN:	g_windows[g_order[0]].y = Min(24 - g_windows[g_order[0]].h, g_windows[g_order[0]].y + 1); break;
			case VK_RIGHT:	g_windows[g_order[0]].x = Min(80 - g_windows[g_order[0]].w, g_windows[g_order[0]].x + 1); break;

			case VK_ESCAPE:
			{
				int i = 0;

				g_show[g_order[0]] = 0;

				while (i < 4 && !g_show[g_order[i]]) i++;

				if (i < 4) put_top(g_order[i]);

				break;
			}
		}
	}

	return 0;
}

void ui_startup()
{
	k_cls(BACKGROUND);
	k_setattr(BACKGROUND);

	k_thread_new(inputd, NULL);
}