#pragma once
#include "EpdFontData.h"

enum class FontRasterMode : uint8_t { Primary = 0, Mono = 1 };

class EpdFont {
  void getTextBounds(const char* string, int startX, int startY, int* minX, int* minY, int* maxX, int* maxY) const;

 public:
  const EpdFontData* data;
  explicit EpdFont(const EpdFontData* data) : data(data) {}
  ~EpdFont() = default;
  void getTextDimensions(const char* string, int* w, int* h) const;

  const EpdGlyph* getGlyph(uint32_t cp) const;

  /// Returns true if this font covers `cp`: either via its in-RAM interval
  /// table or, for SD card fonts, via the coverageHandler that consults the
  /// full RAM-resident coverage index. Unlike getGlyph(), it never performs
  /// storage I/O and never falls back to the replacement glyph — it reports
  /// only what this font can render. Used by the CJK UI font fallback to
  /// decide whether a string needs to be routed to another font.
  bool hasCodepoint(uint32_t cp) const;

  /// Returns the kerning adjustment (4.4 fixed-point in pixels) between two codepoints.
  /// Returns 0 if no kerning data exists for the pair.
  int8_t getKerning(uint32_t leftCp, uint32_t rightCp) const;

  /// Returns the ligature codepoint for a pair, or 0 if no ligature exists.
  uint32_t getLigature(uint32_t leftCp, uint32_t rightCp) const;

  /// Greedily applies ligature substitutions starting from cp, consuming
  /// as many following codepoints from text as possible. Returns the
  /// (possibly substituted) codepoint; advances text past consumed chars.
  uint32_t applyLigatures(uint32_t cp, const char*& text) const;

  void setRasterMode(FontRasterMode mode) const { rasterMode_ = mode; }
  FontRasterMode getRasterMode() const { return rasterMode_; }
  const EpdFontData* getData() const;

  // SD fonts prepare one raster variant at a time into a page-sized cache.
  void setPreparedData(const EpdFontData* prepared, FontRasterMode mode) const {
    preparedData_ = prepared;
    preparedMode_ = mode;
  }
  void clearPreparedData() const { preparedData_ = nullptr; }

 private:
  const EpdGlyph* glyphAt(const EpdFontData* selected, uint32_t glyphIndex) const;

  mutable FontRasterMode rasterMode_ = FontRasterMode::Primary;
  mutable FontRasterMode preparedMode_ = FontRasterMode::Primary;
  mutable const EpdFontData* preparedData_ = nullptr;
  mutable EpdGlyph glyphScratch_{};
};
