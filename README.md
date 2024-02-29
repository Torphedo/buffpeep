<img src="https://1.bp.blogspot.com/-Ld-lrlqxxzk/XoV3q_9CY1I/AAAAAAAACNg/TKaivQl-0i0WEsNMdGvFK6j9aXlYdD_7QCLcBGAsYHQ/s1600/peep%2Bbuff%2Banime.png" alt="drawing" width="300"/>

# buffpeep
This is a simple image viewer intended for raw image buffers. This means it targets image data with no formatting header, where the image format/size is unknown.
It's mainly intended for use with the `resource_x.bin` image files dumped from my [alr](https://github.com/Torphedo/Esper-RE/releases) tool. (But you can use it
on any raw or block-compressed image data with no header.

`buffpeep` lets me try out lots of resolution and format settings, without needing to manually hex edit DDS files. The main benefit is not having to manually
recalculate pitch/linear size or fix DDS header flags when switching formats.

# Planned Changes
- The panning and zooming controls are still a bit awkward, so I want to fix these
- Switching to compressed mode when the resolution isn't a multiple of 4 spams GL debug messages in the log
- I want some way to distinguish the edge of a black texture from the black pixels where there is no data available

# Controls
## Mouse
Click and drag to pan around the image. The cursor won't hit the monitor borders and can pan infinitely.
For some reason the vertical movement is much slower than horizontal in some situations,and doesn't move linearly (even though the math is the same).
Scroll to zoom. This is implemented by scaling the image, so it's relative to the image center and not the window center (which can be awkward to use).

## Up/Down (or K/J)
Increase / decrease height by 1 pixel at a time. If in compressed mode, increase/decrease 4 pixels at a time (compressed size must be a multiple of 4).
## Left/Right (or H/L)
Same as up and down, but for width.

## Space
In compressed mode, cycle through compressed texture formats. Available options are DXT1 (BC1), DXT3 (BC2), and DXT5 (BC3).     
In raw mode, cycle the number of channels from 1 to 4. Channel combinations are R, RG, RGB, and RGBA.

## Shift + Space
Raw mode only, cycles between 8-bit and 16-bit mode. This can be used to view single-channel 16-bit images, for example.

## C
Toggle compressed/raw mode.

## Q
Exit.

# Why a buff Peep?
It lets you peep on unknown image buffers. I was going to name it `bufview`, but my roommate suggested `Peeping Tom`, so I combined them.
