#!/usr/bin/env ruby
require 'json'

$scenario = {
  'description' => "No description",
  'author' => "Unknown",
  'teams' => []
}
$cur_team = nil
$cur_ship = nil

def description text
  $scenario['description'] = text
end

def author name
  $scenario['author'] = name
end

def team name
  team = {
    'name' => name,
    'color' => { 'red' => 1, 'green' => 1, 'blue' => 1 },
    'ships' => []
  }
  $cur_team = team
  yield team
  $cur_team = nil
  $scenario['teams'] << team
end

def color r, g, b
  fail "no current team" unless $cur_team
  $cur_team['color'] = { 'red' => r, 'green' => g, 'blue' => b }
end

def ship klass
  fail "no current team" unless $cur_team
  ship = { 'klass' => klass, 'p' => [0,0], 'v' => [0,0], 'h' => 0 }
  $cur_ship = ship
  yield ship
  $cur_ship = nil
  $cur_team['ships'] << ship
end

def position x, y
  fail "no current ship" unless $cur_ship
  $cur_ship['p'] = [x,y]
end

def velocity vx, vy
  fail "no current ship" unless $cur_ship
  $cur_ship['v'] = [vx,vy]
end

def heading h
  fail "no current ship" unless $cur_ship
  $cur_ship['h'] = h
end

eval(ARGF.read, binding, ARGF.filename)

#puts JSON.dump($scenario)
jj $scenario
