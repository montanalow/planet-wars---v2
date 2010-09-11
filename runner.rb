#! /usr/local/bin/ruby
maps = Dir.entries "./maps"
bots = Dir.entries "./example_bots"
map_limit = 5
bot_limit = 10

maps = maps.select {|m| m[-4..-1] == ".txt"}[0...map_limit]
bots = bots.select {|b| b[-4..-1] == ".jar"}[0...bot_limit]

wins = 0
losses = 0
collisions = 0
draws = 0
m_results = Hash.new {|hash,key| hash[key] = {:wins => 0, :losses => 0, :draws => 0}}
maps.each do |m|
  bots.each do |b|
#    next if m == "huge-room.txt" || m.include?("test") || m.include?( "profile" )
    debug = `java -jar tools/PlayGame.jar maps/#{m} 1000 1000 log.txt build/planet_wars "java -jar example_bots/#{b}" 2>&1`
    result = debug.split("\n")[-2]
    print "1 vs #{b} @ #{m}: "
    if result == "Player 1 Wins!"
      m_results[m][:wins] += 1
      wins += 1
      puts "win"
    elsif result == "Player 2 Wins!"
      m_results[m][:losses] += 1
      losses += 1
      puts "loss"
    elsif result == "Draw!"
      m_results[m][:draws] += 1
      collisions += 1
      puts "draw"
    end

    debug = `java -jar tools/PlayGame.jar maps/#{m} 1000 1000 log.txt "java -jar example_bots/#{b}" build/planet_wars 2>&1`
    result = debug.split("\n")[-2]
    print "2 vs #{b} @ #{m}: "
    if result == "Player 2 Wins!"
      m_results[m][:wins] += 1
      wins += 1
      puts "win"
    elsif result == "Player 1 Wins!"
      m_results[m][:losses] += 1
      losses += 1
      puts "loss"
    elsif result == "Draw!"
      m_results[m][:draws] += 1
      collisions += 1
      puts "draw"
    end
  end
end

puts "-------------------------------"
puts "Wins: #{wins}"
puts "Losses: #{losses}"
puts "Draws: #{draws}"
puts "-------------------------------"
puts "Ratio: #{(wins.to_f + 0.5 * (draws + collisions)) / (wins + losses + draws + collisions)}"
m_results.each do |map, results|
  puts "================================"
  puts "@ #{map}"
  puts "Wins: #{results[:wins]}"
  puts "Losses: #{results[:losses]}"
  puts "Draws: #{results[:draws]}"
  puts "-------------------------------"
  puts "Ratio: #{(results[:wins].to_f + 0.5 * results[:draws]) / (results[:wins] + results[:losses] + results[:draws])}"
end
