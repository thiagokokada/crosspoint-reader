add_executable(epd_font_raster_tests
  EpdFontRasterTest.cpp
  ${REPO_ROOT}/lib/EpdFont/EpdFont.cpp
  ${REPO_ROOT}/lib/EpdFont/EpdFontFamily.cpp
  ${REPO_ROOT}/lib/Utf8/Utf8.cpp
)

target_link_libraries(epd_font_raster_tests PRIVATE crosspoint_test_common GTest::gtest_main)
target_include_directories(epd_font_raster_tests PRIVATE
  ${REPO_ROOT}/lib/EpdFont
  ${REPO_ROOT}/lib/Utf8
)
gtest_discover_tests(epd_font_raster_tests)
