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
		uint8[] data;
		try {
			FileUtils.get_data(data_path(name), out data);
		} catch (FileError e) {
			GLib.error("Failed to load file %s", name);
		}
		return (owned)data;
	}

	public static string data_path(string rel) {
		return "%s/%s".printf(resource_dir.get_path(), rel);
	}
#else
	public static void init(string arg0) {
	}

	[CCode (cname = "oortfs_lookup")]
	extern unowned string oortfs_lookup(string name);

	public static uint8[] load(string name) {
		print("loading resource %s\n", name);
		return oortfs_lookup(name).data;
	}
#endif
}
