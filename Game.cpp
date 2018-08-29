// Game.cpp : Defines the entry point of the application
//

#include "stdafx.h"
// Lib made using objconv
#pragma comment(lib, "ruby.lib")

int main(int argc, char **argv)
{
	// Set console encoding
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	// Adding the new arguments to argc
	int argc2 = argc + 3;
	// creating a new argv
	char** argv2 = new char*[argc + 4];
	// Copying the old argv at the end of the new argv
	for (int i = 1; i < argc; i++)
		argv2[i + 3] = argv[i];
	// Copying program name
	argv2[0] = argv[0];
	// Telling ruby to use UTF-8 as internal and external encoding
	argv2[1] = const_cast<char*>("--encoding");
	argv2[2] = const_cast<char*>("utf-8:utf-8");
	// Telling ruby to load Game.rb
	argv2[3] = const_cast<char*>("Game.rb");
	// Ensure there's no buffer overflow
	argv2[argc2] = NULL;
	// Setting up the return value
	int return_value = EXIT_FAILURE;
	// Loading ruby
	ruby_sysinit(&argc, &argv);
	{
		RUBY_INIT_STACK;
		ruby_init();
		// Adding . and ./plugins to ruby load path
		VALUE load_path = rb_gv_get("$LOAD_PATH");
		rb_ary_push(load_path, rb_str_new(".", 1));
		rb_ary_push(load_path, rb_str_new_cstr("./plugins"));
		// Adding the lib of ruby in the current path to load path, paths encoded in UTF-8 (otherwise the game crashes)
		rb_eval_string("curr_path = File.expand_path('.'.force_encoding('UTF-8'));%w(lib/ruby/site_ruby/<ver> lib/ruby/site_ruby/<ver>/i386-msvcrt lib/ruby/site_ruby lib/ruby/vendor_ruby/<ver> lib/ruby/vendor_ruby/<ver>/i386-msvcrt lib/ruby/vendor_ruby lib/ruby/<ver> lib/ruby/<ver>/i386-mingw32).each { |str| $LOAD_PATH << File.join(curr_path, str.sub('<ver>', RUBY_VERSION)).force_encoding('UTF-8') };nil");
		// Starting ruby
		return_value = ruby_run_node(ruby_options(argc2, argv2));
	}
	// freeing argv
	delete[] argv2;
	// Return ruby's return value
	return return_value;
}
