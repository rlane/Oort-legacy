#!/usr/bin/env ruby
require 'json'
require 'pp'

SCALE = 80

ARGV.each do |filename|
  scn = JSON.load(File.read(filename))
  scn['teams'].each do |team|
    team['ships'].each do |ship|
      ship['x'] *= SCALE
      ship['y'] *= SCALE
    end
  end
  File.open(filename, 'w') { |io| JSON.dump scn, io }
end
