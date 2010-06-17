#!/bin/env/ruby

def do_foo
  puts "Well, we have no choice"
  puts "Let's easte some time ... both human and CPU ..."
  do_bar
end

def do_bar
  puts "... still nothing ..."
  puts "... time goes to waste :("
end

do_foo
