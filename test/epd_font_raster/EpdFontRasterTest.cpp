#include <gtest/gtest.h>

#include "lib/EpdFont/EpdFontFamily.h"

namespace {
const uint8_t kPrimaryBitmap[] = {0xC0};
const uint8_t kMonoBitmap[] = {0x80};
const EpdGlyph kPrimaryGlyphs[] = {{1, 1, 16, 0, 1, 1, 0}};
const EpdGlyph kMonoGlyphs[] = {{1, 1, 16, 1, 2, 1, 0}};
const EpdUnicodeInterval kIntervals[] = {{'A', 'A', 0}};

EpdFontData makeData(const uint8_t* bitmap, const EpdGlyph* glyphs, bool is2Bit,
                     const EpdFontData* mono = nullptr) {
  EpdFontData data{};
  data.bitmap = bitmap;
  data.glyph = glyphs;
  data.intervals = kIntervals;
  data.intervalCount = 1;
  data.advanceY = 10;
  data.ascender = 8;
  data.descender = -2;
  data.is2Bit = is2Bit;
  data.monoVariant = mono;
  return data;
}
}  // namespace

TEST(EpdFontRaster, SelectsIndependentMonoMetricsAndRestoresPrimary) {
  const EpdFontData mono = makeData(kMonoBitmap, kMonoGlyphs, false);
  const EpdFontData primary = makeData(kPrimaryBitmap, kPrimaryGlyphs, true, &mono);
  EpdFont font(&primary);
  EpdFontFamily family(&font);

  EXPECT_EQ(family.getData(), &primary);
  EXPECT_EQ(family.getGlyph('A')->left, 0);
  family.setRasterMode(FontRasterMode::Mono);
  EXPECT_EQ(family.getData(), &mono);
  EXPECT_EQ(family.getGlyph('A')->left, 1);
  EXPECT_EQ(family.getGlyph('A')->advanceX, primary.glyph[0].advanceX);
  family.setRasterMode(FontRasterMode::Primary);
  EXPECT_EQ(family.getData(), &primary);
}

TEST(EpdFontRaster, MissingMonoFallsBackToPrimaryAndStyleFallbackStillWorks) {
  const EpdFontData primary = makeData(kPrimaryBitmap, kPrimaryGlyphs, true);
  EpdFont regular(&primary);
  EpdFontFamily family(&regular);

  family.setRasterMode(FontRasterMode::Mono);
  EXPECT_EQ(family.getData(EpdFontFamily::BOLD_ITALIC), &primary);
  EXPECT_EQ(family.getGlyph('A', EpdFontFamily::SUP)->top, 1);
}

TEST(EpdFontRaster, CompactMonoMetadataUsesPrimaryAdvance) {
  static const EpdCompactGlyph compactGlyphs[] = {{2, 3, -1, 4, 1}};
  EpdFontData mono = makeData(nullptr, nullptr, false);
  mono.compactGlyph = compactGlyphs;
  mono.layoutGlyph = kPrimaryGlyphs;
  EpdFontData primary = makeData(kPrimaryBitmap, kPrimaryGlyphs, true, &mono);
  EpdFont font(&primary);

  font.setRasterMode(FontRasterMode::Mono);
  const EpdGlyph* glyph = font.getGlyph('A');
  ASSERT_NE(glyph, nullptr);
  EXPECT_EQ(glyph->width, 2);
  EXPECT_EQ(glyph->height, 3);
  EXPECT_EQ(glyph->left, -1);
  EXPECT_EQ(glyph->top, 4);
  EXPECT_EQ(glyph->dataLength, 1);
  EXPECT_EQ(glyph->dataOffset, 0u);
  EXPECT_EQ(glyph->advanceX, kPrimaryGlyphs[0].advanceX);
}
