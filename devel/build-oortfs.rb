#!/usr/bin/env ruby
require 'tempfile'

prefix = 'oortfs'
manifest = ARGF.readlines.map(&:chomp)
tmp = Tempfile.new prefix
keywords = []

tmp.puts  '%{'
tmp.puts "#include <string.h>"
tmp.puts "static const char * const #{prefix}_data[] = {"
manifest.each_with_index do |path,idx|
  tmp.puts "#{File.read(path).inspect},"
  keywords << [path, idx]
end
tmp.puts "};"
tmp.puts  '%}'

tmp.puts
tmp.puts "%define lookup-function-name #{prefix}_lookup_int"
tmp.puts "%define hash-function-name #{prefix}_hash"
tmp.puts "%struct-type"
tmp.puts "struct #{prefix}_file { char *name; int idx; };"
tmp.puts "%%"

keywords.each do |path,idx|
  tmp.puts [path, idx]*', '
end

tmp.puts "%%"
tmp.puts <<EOS
const char *#{prefix}_lookup(const char *name)
{
  struct #{prefix}_file *x = #{prefix}_lookup_int(name, strlen(name));
  if (x != NULL) {
    return #{prefix}_data[x->idx];
  } else {
    return NULL;
  }
}
EOS

tmp.close
system("gperf #{tmp.path}")
