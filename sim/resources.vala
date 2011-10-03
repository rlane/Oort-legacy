namespace Oort.Resources {
#if !NACL
	public File resource_dir;

	public static void init(string arg0) {
#if WIN32
		string path = GLib.Win32.get_package_installation_directory_of_module(null);
		resource_dir = File.new_for_path(path);
#else
		File exec_file = File.new_for_path(Environment.find_program_in_path(arg0));
		File bin_dir = File.new_for_path(Config.PACKAGE_BINDIR);
		File data_dir = File.new_for_path(Config.PACKAGE_DATADIR);
		if (exec_file.has_prefix(bin_dir)) {
			resource_dir = data_dir;
		} else {
			resource_dir = exec_file.get_parent().get_parent();
			if (!resource_dir.get_child("runtime.lua").query_exists(null)) {
				resource_dir = resource_dir.get_parent();
			}
		}

		if (!resource_dir.get_child("runtime.lua").query_exists(null)) {
			error("Could not find resource directory, tried %s", resource_dir.get_path());
		}
#endif
		print("using data from %s\n", resource_dir.get_path());
	}

	public static uint8[] load(string name) {
		var filename = path(name);
		uint8[] data;
		try {
			FileUtils.get_data(filename, out data);
		} catch (FileError e) {
			GLib.error("Failed to load resource %s from %s", name, filename);
		}
		return (owned)data;
	}

	public static string path(string name) {
		return "%s/%s".printf(resource_dir.get_path(), name);
	}
#else
	public static void init(string arg0) {
	}

	public static unowned uint8[] load(string name) {
		return "".data;
	}
#endif
}
