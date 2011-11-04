
extern int tstfltk_main(int argc, char *argv[]);

int fltk_main(int argc, char *argv[]) {
	//call c++
   tstfltk_main(argc, argv);
	return 0;
}
