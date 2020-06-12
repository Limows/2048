#include "2048.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <string.h>

#define VALUE_MAX_SIZE 5 // "2048\0"

static const int WIDTH  = 340;
static const int HEIGHT = 400;
static const int TILE_SIZE = 64;
static const int TILE_MARGIN = 16;

static int *board = NULL;
static XFontStruct *font_small = NULL;
static XFontStruct *font_middle = NULL;
static XFontStruct *font_normal = NULL;
static XFontStruct *font_large = NULL;

static inline int offset_coords(int coord) { return coord * (TILE_MARGIN + TILE_SIZE) + TILE_MARGIN; }
static inline unsigned fade_color(unsigned rgb) { return rgb - COLOR_FADE; }

static void draw_tile(Display *display, int screen, Window window, int value, int x, int y, int win, int lose) {
	const unsigned bkg = e_background(value), frg = e_foreground(value);
	const int xOffset = offset_coords(x), yOffset = offset_coords(y);
	XSetForeground(display, DefaultGC(display, screen), (win || lose) ? fade_color(bkg) : bkg);
	// HACK: Emulating XFillRoundedRectangle() method in 9 calls, similar to GTK+1 version.
	const int w = TILE_SIZE / 2, dw = w * 2, qw = w / 4, rad = qw * 2, rw = rad * 3, rect = w - rad;
	XFillArc(display, window, DefaultGC(display, screen), xOffset, yOffset, rect, rect, -64*180, -64*90);
	XFillArc(display, window, DefaultGC(display, screen), xOffset + w + rad, yOffset, rect, rect, 64*90, -64*90);
	XFillArc(display, window, DefaultGC(display, screen), xOffset, yOffset + w + rad, rect, rect, -64*90, -64*90);
	XFillArc(display, window, DefaultGC(display, screen), xOffset + w + rad, yOffset + w + rad, rect, rect, 0, -64*90);
	XFillRectangle(display, window, DefaultGC(display, screen),
		xOffset + qw, yOffset + qw, TILE_SIZE - rad + 1, TILE_SIZE - rad + 1);
	XFillRectangle(display, window, DefaultGC(display, screen), xOffset, yOffset + qw, qw, rw);
	XFillRectangle(display, window, DefaultGC(display, screen), xOffset + dw - qw + 1, yOffset + qw, qw, rw);
	XFillRectangle(display, window, DefaultGC(display, screen), xOffset + qw, yOffset, rw, qw);
	XFillRectangle(display, window, DefaultGC(display, screen), xOffset + qw, yOffset + dw - qw + 1, rw, qw);
	if (value) {
		XFontStruct *current_font = NULL;
		if (font_small && font_middle && font_large)
			current_font = (value < 100) ? font_large : (value < 1000) ? font_middle : font_small;
		const int size = (value < 100) ? 24 : (value < 1000) ? 18 : 14;
		char str_value[VALUE_MAX_SIZE];
		snprintf(str_value, VALUE_MAX_SIZE, "%d", value);
		if (current_font)
			XSetFont(display, DefaultGC(display, screen), current_font->fid);
		const int w = (current_font) ? XTextWidth(current_font, str_value, strlen(str_value)) : 10, h = size + 4;
		XSetForeground(display, DefaultGC(display, screen), (win || lose) ? fade_color(frg) : frg);
		XDrawString(display, window, DefaultGC(display, screen),
			xOffset + (TILE_SIZE - w) / 2, yOffset + TILE_SIZE - (TILE_SIZE - h) / 2 - 2,
			str_value, strlen(str_value));
	}
}

static void draw_final(Display *display, int screen, Window window, int win, int lose) {
	if (win || lose) {
		XSetForeground(display, DefaultGC(display, screen), COLOR_FINAL);
		if (font_large)
			XSetFont(display, DefaultGC(display, screen), font_large->fid);
		const char *center = ((win) ? "You won!" : (lose) ? "Game Over!" : "");
		const int w = (font_large) ? XTextWidth(font_large, center, strlen(center)) : 50;
		XDrawString(display, window, DefaultGC(display, screen), WIDTH / 2 - (w - 3) / 2, HEIGHT / 2,
			center, strlen(center));
	}
	XSetForeground(display, DefaultGC(display, screen), (win || lose) ? fade_color(COLOR_TEXT) : COLOR_TEXT);
	const char *strReset = "ESC to Restart!";
	if (font_normal)
		XSetFont(display, DefaultGC(display, screen), font_normal->fid);
	const int h = 20;
	XDrawString(display, window, DefaultGC(display, screen), TILE_MARGIN, HEIGHT - h,
		strReset, strlen(strReset));
	char strScore[16];
	snprintf(strScore, 16, "Score: %d", e_score());
	const int w = (font_normal) ? XTextWidth(font_normal, strScore, strlen(strScore)) : 50;
	XDrawString(display, window, DefaultGC(display, screen), WIDTH - (w - 3) - TILE_MARGIN, HEIGHT - h,
		strScore, strlen(strScore));
}

static void draw(Display *display, int screen, Window window) {
	const int win = e_win(), lose = e_lose();
	XSetForeground(display, DefaultGC(display, screen), (win || lose) ? fade_color(COLOR_BOARD) : COLOR_BOARD);
	XFillRectangle(display, window, DefaultGC(display, screen), 0, 0, WIDTH, HEIGHT);
	int y = 0;
	for (; y < VERTICAL; ++y) {
		int x = 0;
		for (; x < HORIZONTAL; ++x)
			draw_tile(display, screen, window, board[x + y * 4], x, y, win, lose);
	}
	draw_final(display, screen, window, win, lose);
}

int main(void) {
	board = e_init_board(XK_Escape, XK_Left, XK_Right, XK_Up, XK_Down);

	Display *display = XOpenDisplay(NULL);
	const int screen = DefaultScreen(display);
	const Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 50, 50, WIDTH, HEIGHT, 1,
		BlackPixel(display, screen), WhitePixel(display, screen));
	XStoreName(display, window, "2048-Xlib");
	XSelectInput(display, window, ExposureMask | KeyPressMask);
	XMapWindow(display, window);

	Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	font_small = XLoadQueryFont(display, "-adobe-helvetica-bold-r-normal--20-140-100-100-p-105-iso8859-1");
	font_middle = XLoadQueryFont(display, "-adobe-helvetica-bold-r-normal--25-180-100-100-p-138-iso8859-1");
	font_normal = XLoadQueryFont(display, "-adobe-helvetica-medium-r-normal--20-140-100-100-p-100-iso8859-1");
	font_large = XLoadQueryFont(display, "-adobe-helvetica-bold-r-normal--34-240-100-100-p-182-iso8859-1");

	XEvent event;
	while (1) {
		XNextEvent(display, &event);
		if (event.type == Expose) {
			draw(display, screen, window);
		}
		if (event.type == KeyPress) {
			KeySym key = XLookupKeysym(&event.xkey, 0);
			if (key == XK_q)
				break;
			e_key_event(key);
			event.type = Expose;
			XSendEvent(display, window, True, ExposureMask, &event);
		}
		// Detect closing window by [X] button in the title bar.
		// https://stackoverflow.com/a/1186544
		if (event.type == ClientMessage && (Atom)event.xclient.data.l[0] == wm_delete_window) {
			break;
		}
	}

	XCloseDisplay(display);
	return 0;
}