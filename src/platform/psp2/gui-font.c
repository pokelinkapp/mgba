/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "util/gui/font.h"
#include "util/gui/font-metrics.h"
#include "font.h"

#include <vita2d.h>

#define CELL_HEIGHT 16
#define CELL_WIDTH 16
#define GLYPH_HEIGHT 12

struct GUIFont {
	vita2d_texture* tex;
};

struct GUIFont* GUIFontCreate(void) {
	struct GUIFont* guiFont = malloc(sizeof(struct GUIFont));
	if (!guiFont) {
		return 0;
	}
	guiFont->tex = vita2d_load_PNG_buffer(font);
	return guiFont;
}

void GUIFontDestroy(struct GUIFont* font) {
	vita2d_free_texture(font->tex);
	free(font);
}

unsigned GUIFontHeight(const struct GUIFont* font) {
	UNUSED(font);
	return GLYPH_HEIGHT;
}

unsigned GUIFontGlyphWidth(const struct GUIFont* font, uint32_t glyph) {
	UNUSED(font);
	if (glyph > 0x7F) {
		glyph = 0;
	}
	return defaultFontMetrics[glyph].width;
}

void GUIFontDrawGlyph(const struct GUIFont* font, int x, int y, uint32_t color, uint32_t glyph) {
	if (glyph > 0x7F) {
		glyph = 0;
	}
	struct GUIFontGlyphMetric metric = defaultFontMetrics[glyph];
	vita2d_draw_texture_tint_part(font->tex, x, y - GLYPH_HEIGHT + metric.padding.top,
		                          (glyph & 15) * CELL_WIDTH + metric.padding.left,
		                          (glyph >> 4) * CELL_HEIGHT + metric.padding.top,
		                          CELL_WIDTH - (metric.padding.left + metric.padding.right),
		                          CELL_HEIGHT - (metric.padding.top + metric.padding.bottom),
		                          color);
}
