png_viewer: png_program.c png_reader.c png_xwindow.c png_structs.c
	gcc -lX11 -lpng png_program.c png_reader.c png_xwindow.c png_structs.c -o png_view