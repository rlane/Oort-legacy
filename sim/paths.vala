extern const string PACKAGE_BINDIR;
extern const string PACKAGE_DATADIR;

namespace RISC.Paths {
	public File resource_dir;

	public static void init(string arg0) {
		File exec_file = File.new_for_path(Environment.find_program_in_path(arg0));
    File bin_dir = File.new_for_path(PACKAGE_BINDIR);
    File data_dir = File.new_for_path(PACKAGE_DATADIR);
    if (exec_file.has_prefix(bin_dir)) {
			resource_dir = data_dir;
		} else {
			resource_dir = exec_file.get_parent().get_parent().get_parent();
		}
	}
}

namespace RISC {
	public static string data_path(string rel) {
		return "%s/%s".printf(Paths.resource_dir.get_path(), rel);
	}
}
