CD "C:\Users\chenhuihu\Documents\source_code\motion_editor\result_picture"
ffmpeg -r 30 -i %%05d.png -c:v libx264 -qp 1 -profile:v high444 -preset fast -pix_fmt yuv420p result.mp4
@ECHO OFF
EXIT
