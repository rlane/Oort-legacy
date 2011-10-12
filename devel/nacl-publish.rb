#!/usr/bin/env ruby
require 'aws/s3'
include AWS

BUCKET_NAME = 'oort-nacl'

sha1 = ARGV[0] or fail "sha1 arg required"

files = {
  'oort.html' => 'nacl/oort.html',
  'oort.nmf' => 'nacl/oort.nmf',
  'oort_nacl32' => "oort_nacl.#{sha1}.32.nexe",
  'oort_nacl64' => "oort_nacl.#{sha1}.64.nexe",
}

files.each { |remote,local| fail "file #{local} not found" unless File.exists? local }

index_html = <<EOS
<html>
<head>
<meta http-equiv="Refresh" content="0;url=#{sha1}/oort.html" />
</head>
</html>
EOS

if !ENV['AMAZON_ACCESS_KEY_ID'] or !ENV['AMAZON_SECRET_ACCESS_KEY']
  fail "must export AMAZON_ACCESS_KEY_ID and AMAZON_SECRET_ACCESS_KEY"
end

S3::Base.establish_connection!(:access_key_id => ENV['AMAZON_ACCESS_KEY_ID'],
                               :secret_access_key => ENV['AMAZON_SECRET_ACCESS_KEY'])

files.each do |remote,local|
  full_remote = "#{sha1}/#{remote}"
  puts "publishing #{local} -> #{full_remote}..."
  File.open local, 'r' do |io|
    S3::S3Object.store(full_remote, io, BUCKET_NAME, :access => :public_read)
  end
end

puts "publishing index.html..."
S3::S3Object.store("index.html", index_html, BUCKET_NAME, :access => :public_read)
