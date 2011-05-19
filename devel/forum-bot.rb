#!/usr/bin/env ruby
require 'mechanize'

DB_FILENAME = "db.yaml"
TMP_FILENAME = ".db.yaml.tmp"
SHA1_REGEX = /\b[0-9a-fA-F]{20}\b/
URL = 'http://oort.lefora.com/'

USERNAME = ARGV[0] or fail "username required"
PASSWORD = ARGV[1] or fail "password required"

if File.exists? DB_FILENAME
  db = File.open(DB_FILENAME) { |io| YAML.load io }
else
  db = {
    'seen' => [],
    'ais' => {},
  }
end

agent = Mechanize.new
page = agent.get(URL)
page = agent.page.link_with(:text => 'login').click
form = page.forms.first
form.email = USERNAME
form.password = PASSWORD
page = agent.submit form, form.buttons.first
page = agent.page.link_with(:text => 'messages').click
page.links.each do |l|
  l.href =~ %r|/mailbox/view/(\d+)| or next
  msgid = $1.to_i
  next if db['seen'].member? msgid
  db['seen'] << msgid
  puts "processing message #{msgid}"
  begin
    page = l.click
    sender = page.search("//div[@id='mailbox_content']/div[1]/div[1]/b/text()").map(&:to_s).map(&:strip).reject(&:empty?).first
    subject = page.search("//div[@id='content']/div[@class='section header-section']/h1[@class='item_title']/text()").map(&:to_s).map(&:strip).reject(&:empty?).first
    site = page.search("//div[@id='content']/div[@class='section header-section']/div[@class='item_subtitle']/text()").map(&:to_s).map(&:strip).reject(&:empty?).first
    body = page.search("//div[@id='mailbox_content']/div[1]/div[2]/p/text()").map(&:to_s).map(&:strip).reject(&:empty?).join("\n")

    puts "wrong site" unless site == 'from: oort.lefora.com'

    puts "no sha1 found" unless body =~ SHA1_REGEX
    sha1 = $&

    puts "no name found" unless subject =~ /Message: (\w+)$/
    name = $1

    fail "name conflict" if db['ais'].member? name

    puts "#{sender.inspect} registered #{name} as #{sha1}"
    db['ais'][name] = {
      'user' => sender,
      'sha1' => sha1,
    }
  rescue
    puts "failed to process message #{msgid}: #{$!.message}"
    #puts page.send :html_body
  end
end

File.open(TMP_FILENAME, 'w') do |io|
  YAML.dump db, io
end

File.rename TMP_FILENAME, DB_FILENAME
