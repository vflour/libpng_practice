png_viewer: png_program.c png_reader.c png_xwindow.c
	gcc -lX11 -lpng png_program.c png_reader.c png_xwindow.c -o png_view