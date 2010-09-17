#! /usr/local/bin/ruby
running = true
trap "INT" do
  running = false
end

if ARGV[0] == "tcp"
  while running do
    puts `./tools/tcp 213.3.30.106 9999 montanalow ./build/planet_wars`
    print "next match in"
    (1...10).each do |i|
      print " #{i}"
      sleep 1
    end
  end
end

maps = Dir.entries "./maps"
bots = Dir.entries "./example_bots"
map_limit = 100
bot_limit = 15

maps = maps.select {|m| m[-4..-1] == ".txt"}.sort{|a,b| a[3..-1].to_i <=> b[3..-1].to_i}[0...map_limit]
bots = bots.select {|b| b[-4..-1] == ".jar"}[0...bot_limit]

wins = 0
losses = 0
collisions = 0
draws = 0
win_moves = 0

m_results = Hash.new {|hash,key| hash[key] = {:wins => 0, :win_moves => 0, :losses => 0, :draws => 0}}
maps.each do |m|
  bots.each do |b|
    next unless running
    `java -jar tools/PlayGame-1.2.jar maps/#{m} 1000 1000 log.txt build/planet_wars "java -jar example_bots/#{b}" 2> runner.log`
    next unless running
    debug = File.read("runner.log").split("\n").select{|line| (line =~ /Turn \d+/) == 0}
    File.delete "runner.log"
    result = debug[-1]
    print "1 vs #{b} @ #{m}: "
    if result == "Player 1 Wins!"
      m_results[m][:wins] += 1
      m_results[m][:win_moves] += debug.size
      wins += 1
      win_moves += debug.size
      puts "!!! win !!! (#{debug.size - 1} moves)"
    elsif result == "Player 2 Wins!"
      m_results[m][:losses] += 1
      losses += 1
      puts "@@@ loss @@@ (#{debug.size - 1} moves)"
    elsif result == "Draw! #{debug.size}"
      m_results[m][:draws] += 1
      collisions += 1
      puts "draw (#{debug.size - 1} moves)"
    end
    `java -jar tools/PlayGame-1.2.jar maps/#{m} 1000 1000 log.txt "java -jar example_bots/#{b}" build/planet_wars 2> runner.log`
    next unless running
    debug = File.read("runner.log").split("\n").select{|line| (line =~ /Turn \d+/) == 0}
    File.delete "runner.log"
    result = debug[-1]
    print "2 vs #{b} @ #{m}: "
    if result == "Player 2 Wins!"
      m_results[m][:wins] += 1
      m_results[m][:win_moves] += debug.size
      wins += 1
      win_moves += debug.size
      puts "!!! win !!! (#{debug.size - 1} moves)"
    elsif result == "Player 1 Wins!"
      m_results[m][:losses] += 1
      losses += 1
      puts "@@@ loss @@@ (#{debug.size - 1} moves)"
    elsif result == "Draw!"
      m_results[m][:draws] += 1
      collisions += 1
      puts "draw (#{debug.size - 1} moves)"
    end
  end
end

m_results.each do |map, results|
  puts "================================"
  puts "@ #{map}"
  puts "Wins: #{results[:wins]} (#{results[:win_moves] / (bots.size * 2)} avg moves)"
  puts "Losses: #{results[:losses]}"
  puts "Draws: #{results[:draws]}"
  puts "-------------------------------"
  puts "Ratio: #{(results[:wins].to_f + 0.5 * results[:draws]) / (results[:wins] + results[:losses] + results[:draws])}"
end
puts "-------------------------------"
puts "Wins: #{wins} (#{win_moves / (bots.size * maps.size * 2)} avg moves)"
puts "Losses: #{losses}"
puts "Draws: #{draws}"
puts "-------------------------------"
puts "Ratio: #{(wins.to_f + 0.5 * (draws + collisions)) / (wins + losses + draws + collisions)}"
