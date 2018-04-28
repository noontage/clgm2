#
# Sample
#
include CLGM2

@robby = Channel.new("Robby")

CLGM2::event_on(:connected) do |con|
  @robby.join(con)
  con.connected_date = Time.now
end

CLGM2::event_on(:disconnected) do |con|
  @robby.leave(con)
end

CLGM2::event_on(:message) do |con, msg|
  @robby.send(msg)
end

class User < Connection
  attr_accessor :connected_date
end

ConnectionManager::connection_class = User
ConnectionManager::listen(3000)
