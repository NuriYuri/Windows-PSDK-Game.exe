// Game.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
// Lib made using objconv
#pragma comment(lib, "ruby.lib")

void set_console_encoding()
{
#ifndef NO_CONSOLE
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
}

#ifdef NO_CONSOLE
VALUE ruby_void_print(int argc, const VALUE *argv)
{
	return Qnil;
}
#define _rbf (VALUE(*)(...))
#endif // NO_CONSOLE

void define_ruby_void_print()
{
#ifdef NO_CONSOLE
    rb_define_global_function("printf", _rbf ruby_void_print, -1);
    rb_define_global_function("print", _rbf ruby_void_print, -1);
    rb_define_global_function("putc", _rbf ruby_void_print, -1);
    rb_define_global_function("puts", _rbf ruby_void_print, -1);
    rb_define_global_function("gets", _rbf ruby_void_print, -1);
    rb_define_global_function("readline", _rbf ruby_void_print, -1);
	rb_define_global_function("readlines", _rbf ruby_void_print, -1);
	rb_define_global_function("p", _rbf ruby_void_print, -1);
#endif
}

char** init_argv2(int& argc2, int argc, char **argv)
{
	argc2 = argc + 3;
	// creating a new argv array
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

	return argv2;
}


int main(int argc, char **argv)
{
	int argc2;
	// Set console encoding
	set_console_encoding();
	// creating a new argv
	char** argv2 = init_argv2(argc2, argc, argv);
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
		define_ruby_void_print();
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


#ifdef NO_CONSOLE
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int value;
	int w_argc = 0;
	LPWSTR* w_argv = CommandLineToArgvW(GetCommandLineW(), &w_argc);
	if (w_argv)
	{
		char** my_argv = new char*[w_argc];
		int my_argc = 0;

		for (int i = 0; i < w_argc; ++i)
		{
			int w_len = lstrlenW(w_argv[i]);
			int len = WideCharToMultiByte(CP_ACP, 0, w_argv[i], w_len, NULL, 0, NULL, NULL);
			my_argv[my_argc] = new char[len + 1];
			WideCharToMultiByte(CP_ACP, 0, w_argv[i], w_len, my_argv[my_argc], len + 1, NULL, NULL);
			++my_argc;
		}

		value = main(my_argc, my_argv);

		for (int i = 0; i < my_argc; ++i)
			delete[] my_argv[i];
		delete[] my_argv;

		LocalFree(w_argv);
	}
	/*char* argv = const_cast<char*>("Game-noconsole.exe");
	int value = main(1, &argv);*/
	/*if (value != 0)
		MessageBoxA(0, 0, 0, 0);*/
	return value;
}
#endif
