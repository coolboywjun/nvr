
#ifndef _CTRL_DATA_H_
#define _CTRL_DATA_H_

#if 0
gal_pixel UserDefineIndex [17];

/* 用户自定义颜色 后期可以使用 */
const RGB UserDefineIndex [] =
{
    {0x00, 0x00, 0x00},     /* transparent   --0 */
    {0x00, 0x00, 0x80},     /* dark blue     --1 */
    {0x00, 0x80, 0x00},     /* dark green    --2 */
    {0x00, 0x80, 0x80},     /* dark cyan     --3 */
    {0x80, 0x00, 0x00},     /* dark red      --4 */
    {0x80, 0x00, 0x80},     /* dark magenta  --5 */
    {0x80, 0x80, 0x00},     /* dark yellow   --6 */
    {0x80, 0x80, 0x80},     /* dark gray     --7 */
    {0xC0, 0xC0, 0xC0},     /* light gray    --8 */
    {0x00, 0x00, 0xFF},     /* blue          --9 */
    {0x00, 0xFF, 0x00},     /* green         --10 */
    {0x00, 0xFF, 0xFF},     /* cyan          --11 */
    {0xFF, 0x00, 0x00},     /* red           --12 */
    {0xFF, 0x00, 0xFF},     /* magenta       --13 */
    {0xFF, 0xFF, 0x00},     /* yellow        --14 */
    {0xFF, 0xFF, 0xFF},     /* light white   --15 */
    {0x00, 0x00, 0x00}      /* black         --16 */
};

#endif

#define BORDER_NORMAL_COLOR  PIXEL_black
#define BORDER_FOCUS_COLOR     PIXEL_darkgray//PIXEL_cyan

#define FONT_NORMAL_COLOR    PIXEL_black
#define FONT_SELECT_COLOR   PIXEL_black//PIXEL_cyan

#define Disable_color  PIXEL_darkgray



#endif
