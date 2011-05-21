require 'mechanize'

class OortForum
  URL = 'http://oort.lefora.com'

  attr_reader :agent

  def initialize
    @agent = Mechanize.new
    @home = agent.get(URL)
  end

  def login username, password
    page = @home.link_with(:text => 'login').click
    form = page.forms.first
    form.email = username
    form.password = password
    @home = agent.submit form, form.buttons.first
  end

  def list_messages
    page = @home.link_with(:text => 'messages').click
    page.links.map { |l| l.href =~ %r|/mailbox/view/(\d+)| && $1.to_i }.compact
  end

  def get_message id
    Message.new self, id
  end

  class Message
    attr_reader :sender, :subject, :site, :body

    def initialize forum, id
      @forum = forum
      @page = @forum.agent.get "#{URL}/mailbox/view/#{id}"
      @sender = @page.search("//div[@id='mailbox_content']/div[1]/div[1]/b/text()").map(&:to_s).map(&:strip).reject(&:empty?).first
      @subject = @page.search("//div[@id='content']/div[@class='section header-section']/h1[@class='item_title']/text()").map(&:to_s).map(&:strip).reject(&:empty?).first
      @site = @page.search("//div[@id='content']/div[@class='section header-section']/div[@class='item_subtitle']/text()").map(&:to_s).map(&:strip).reject(&:empty?).first
      @body = @page.search("//div[@id='mailbox_content']/div[1]/div[2]/p/text()").map(&:to_s).map(&:strip).reject(&:empty?).join("\n")
    end

    def reply text
      form = @page.forms.first
      form.body = text
      @forum.agent.submit form, form.buttons.first
    end
  end
end
