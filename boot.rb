
p "Ruby!"

channel = Channel.new("Global")

CLGM2::event_on(:connected) do |con|
  channel.join(con)
end

CLGM2::event_on(:disconnected) do |con|
  channel.leave(con)
end

CLGM2::event_on(:message) do |con, msg|
  channel.send("echo: #{msg}")
end

loop do
  3.times do
    puts "sec"
    CLGM2::sleep(1)
  end

  3.times do
    puts "1000ms"
    CLGM2::sleep(1000, :ms)
  end

  3.times do
    puts "1000000us"
    CLGM2::sleep(1000000, :us)
  end
end
