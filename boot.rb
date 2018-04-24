
p "Ruby!"

channel = Channel.new("Global")

CLGM2::event_on(:connected) do |con|
  channel.join(con)
  p con.class
  con.hogehoge
  con.send("ええで")
end

CLGM2::event_on(:disconnected) do |con|
  channel.leave(con)
end

CLGM2::event_on(:message) do |con, msg|
  channel.send("echo: #{msg}")
end

class Hoge < Connection
  def hogehoge
    puts "hogehoge"
  end
end

CLGM2::connection_class = Hoge
