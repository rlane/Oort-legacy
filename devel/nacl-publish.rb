#!/usr/bin/env ruby
require 'pp'
require 'json'
require 'tempfile'
require 'aws/s3'
include AWS

BUCKET_NAME = 'oort-nacl'

sha1 = ARGV[0] or fail "sha1 arg required"

files = {
  'oort.html' => 'build-nacl/www/oort.html',
  'oort.nmf' => 'build-nacl/www/oort.nmf',
  'oort_nacl64' => "build-nacl/x86_64-nacl/oort_nacl",
  'oort_nacl32' => "build-nacl/i686-nacl/oort_nacl",
}

files.each { |remote,local| fail "file #{local} not found" unless File.exists? local }

local_lib_prefix = 'build-nacl/www'
lib_archs = %w(lib32 lib64)
lib_names = %w(libc.so.e59bca84 libdl.so.e59bca84 libgcc_s.so.1 libm.so.e59bca84 libppapi_cpp.so libppapi_gles2.so libpthread.so.e59bca84 libstdc++.so.6 runnable-ld.so)
local_libs = lib_archs.map { |arch| lib_names.map { |name| "#{local_lib_prefix}/#{arch}/#{name}" } }.flatten
remote_libs = lib_archs.map { |arch| lib_names.map { |name| "#{arch}/#{name}" } }.flatten

local_libs.each do |filename|
  fail "file #{filename} not found" unless File.exists? filename
end

index_html = <<EOS
<html>
<head>
<meta http-equiv="Refresh" content="0;url=#{sha1}/oort.html" />
</head>
</html>
EOS

nmf = JSON.load File.read(files['oort.nmf'])
nmf['program'].each { |k,v| nmf['program'][k]['url'] = "../#{v['url']}" }
nmf['files'].select { |k,v| k =~ /^lib/ }.each do |k,v|
  v.each do |k2,v2|
    v2['url'] = "../#{v2['url']}".gsub('+', '%2B')
  end
end
nmf_file = Tempfile.new 'oort'
nmf_file.write JSON.dump(nmf)
nmf_file.close
files['oort.nmf'] = nmf_file.path

if !ENV['AMAZON_ACCESS_KEY_ID'] or !ENV['AMAZON_SECRET_ACCESS_KEY']
  fail "must export AMAZON_ACCESS_KEY_ID and AMAZON_SECRET_ACCESS_KEY"
end

S3::Base.establish_connection!(:access_key_id => ENV['AMAZON_ACCESS_KEY_ID'],
                               :secret_access_key => ENV['AMAZON_SECRET_ACCESS_KEY'])

existing_remote_libs = S3::Bucket.objects(BUCKET_NAME, :prefix => 'lib').map(&:key)
upload_libs = remote_libs - existing_remote_libs

upload_libs.each do |remote|
  local = "#{local_lib_prefix}/#{remote}"
  puts "publishing #{local} -> #{remote}..."
  File.open local, 'r' do |io|
    S3::S3Object.store(remote, io, BUCKET_NAME, :access => :public_read)
  end
end

remote_mod_times = Hash[S3::Bucket.objects(BUCKET_NAME, :prefix => "#{sha1}/").map { |x| [x.key, Time.parse(x.about["last-modified"])] }]

files.each do |remote,local|
  full_remote = "#{sha1}/#{remote}"
  remote_mod_time = remote_mod_times[full_remote]
  local_mod_time = File.mtime local
  next unless local_mod_time >= remote_mod_time
  puts "publishing #{local} -> #{full_remote}..."
  File.open local, 'r' do |io|
    S3::S3Object.store(full_remote, io, BUCKET_NAME, :access => :public_read)
  end
end

puts "publishing index.html..."
S3::S3Object.store("index.html", index_html, BUCKET_NAME, :access => :public_read)
