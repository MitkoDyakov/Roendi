#!env ruby
# Copyright (c) 2018(-2024) STMicroelectronics.
# All rights reserved.
#
# This file is part of the TouchGFX 4.23.2 distribution.
#
# This software is licensed under terms that can be found in the LICENSE file in
# the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#
###############################################################################/

$:.unshift File.dirname(__FILE__)

require 'rubygems/version'
require 'lib/text_database_upgrader'
require 'lib/text_database_validator'
require 'lib/version'

def root_dir
  # Get the dirname of this (upgrader.rb) file:
  @root_dir ||= File.dirname(__FILE__)
end

class Main
  def self.banner
    <<-BANNER
Upgrade the text database to a specific version or the latest TouchGFX version #{TOUCHGFX_VERSION}

Usage: #{File.basename($0)} path/to/text_database [version]

Note! If no version is specified the text database will be upgraded to TouchGFX version #{TOUCHGFX_VERSION}
BANNER
  end

  abort self.banner if ARGV.count < 1

  file_name = ARGV.shift # texts.xml or texts.xlsx
  version = ARGV.shift # Version to upgrade to, if not the latest TouchGFX version

  version ||= TOUCHGFX_VERSION
  fail "ERROR: Version #{version} is greater than latest TouchGFX version #{TOUCHGFX_VERSION}" if Gem::Version.new(version) > Gem::Version.new(TOUCHGFX_VERSION)

  begin
    TextDatabaseValidator.new.validate(file_name)
    TextDatabaseUpgrader.new(file_name, version).run

  rescue SystemExit => e

  rescue Exception => e
    STDERR.puts e
    STDERR.puts e.backtrace if ENV['DEBUG']
    abort "An error occurred during text database upgrading!"
  end
end