#!/usr/bin/ruby

# Copyright (C) 2017 Open Source Robotics Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

require 'open3'
require 'optparse'
require 'yaml'

conf_file = '/home/caguero/.ignition/fuel/.sources.yaml'
conf_file = ENV['IGN_FUEL_CONFIG_PATH'] if ENV.key?('IGN_FUEL_CONFIG_PATH')

# Sanity check: The configuration file should exist.
if not File.exist?(conf_file)
  puts "Unable to find configuration file in [#{conf_file}]\n"
  exit(-1)
end

# Read the configuration file.
yml = YAML.load_file(conf_file)

# Sanity check: Verify that the content of the file is YAML.
unless yml && yml.is_a?(Hash)
  puts "File [#{conf_file}] does not contain a valid YAML format\n"
  exit(-1)
end

# Read configuration.
raise "['servers:'] not found" unless yml.key?('servers')
servers = yml['servers']
raise "No servers found" unless not servers.empty?
#puts "#{servers}"
raise "['install_dir:'] not found" unless yml.key?('install_dir')

# Parse the command line arguments.
options = {}
option_parser = OptionParser.new do |opts|
  opts.on("--install RESOURCE", "-i RESOURCE") do |resource|
    options[:resource] = resource
    options['command'] = 'install'
  end

  opts.on("--type TYPE", [:models, :worlds], "Select type") do |type|
    options[:type] = type
    options['command'] = 'type'
  end
end

option_parser.parse!
#
#type = ARGV.pop
#raise "No type specified" unless type
#
## Command: install
#if options.key?('command') and options['command'] == 'install'
#
#  # Check if the server contains the requested asset.
#  command = "girder-cli --api-url #{server}/api/v1 download /collection/"\
#            "#{options[:asset]} #{options[:asset]}"
#  puts "#{type}"
#  #stdout_str, stderr_str, status = Open3.capture3(command)
#
#  #unless status.exitstatus == 0
#  #  puts "[#{options[:asset]}] Nonexisten asset"
#  #  exit -1
#  #end
#end
