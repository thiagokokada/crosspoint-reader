"""Bitmap packing helpers shared by the built-in and SD font converters."""


def pack_mono_rows(buffer, width, rows, pitch):
    """Pack a pitch-aligned FreeType MONO bitmap into one continuous 1-bit stream.

    FreeType rows are MSB-first and padded to ``abs(pitch)`` bytes. A negative
    pitch means the first buffer row is the visual bottom row. Firmware glyphs
    have no row padding: bits continue directly across row boundaries.
    """
    if width == 0 or rows == 0:
        return b""
    stride = abs(pitch)
    if stride < (width + 7) // 8:
        raise ValueError("monochrome bitmap pitch is shorter than its pixel width")
    if len(buffer) < stride * rows:
        raise ValueError("monochrome bitmap buffer is truncated")

    packed = bytearray((width * rows + 7) // 8)
    out_pos = 0
    for y in range(rows):
        source_y = y if pitch >= 0 else rows - 1 - y
        row_offset = source_y * stride
        for x in range(width):
            if buffer[row_offset + x // 8] & (0x80 >> (x & 7)):
                packed[out_pos // 8] |= 0x80 >> (out_pos & 7)
            out_pos += 1
    return bytes(packed)


def pack_freetype_mono(bitmap, expected_pixel_mode):
    if bitmap.pixel_mode != expected_pixel_mode:
        raise ValueError(
            f"FreeType returned pixel mode {bitmap.pixel_mode}, expected MONO ({expected_pixel_mode})"
        )
    return pack_mono_rows(bitmap.buffer, bitmap.width, bitmap.rows, bitmap.pitch)
