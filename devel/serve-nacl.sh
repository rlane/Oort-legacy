#!/bin/sh
exec ruby -rwebrick -e 'WEBrick::HTTPServer.new(:Port=>5103,:DocumentRoot=>"build-nacl/www").start'
