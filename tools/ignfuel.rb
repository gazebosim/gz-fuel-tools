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

require 'json'
require 'net/http'
require 'optparse'
require 'yaml'

conf_file = '/home/caguero/.ignition/fuel/.sources.yaml'
conf_file = ENV['IGN_FUEL_CONFIG_PATH'] if ENV.key?('IGN_FUEL_CONFIG_PATH')

# Sanity check: The configuration file should exist.
unless File.exist?(conf_file)
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
raise 'No servers found' if servers.empty?

# puts "#{servers}"
raise "['install_dir:'] not found" unless yml.key?('install_dir')

# Parse the command line arguments.
options = {}
option_parser = OptionParser.new do |opts|
  opts.on('--search RESOURCE", "-s RESOURCE') do |resource|
    options[:resource] = resource
    options['command'] = 'search'
  end

  # opts.on("--type TYPE", [:models, :worlds], "Select type") do |type|
  #  options[:type] = type
  #  options['command'] = 'type'
  # end
end

option_parser.parse!

# type = ARGV.pop
# raise "No type specified" unless type
#
# Command: search
if options.key?('command') && options['command'] == 'search'
  query_string = 'q=' + options[:resource] + '&mode=text&types=["folder"]'
  uri_string = 'http://localhost:8080/api/v1/resource/search?' + query_string
  uri = URI.parse(uri_string)
  http = Net::HTTP.new(uri.host, uri.port)
  request = Net::HTTP::Get.new(uri.request_uri)
  response = http.request(request)
  if response.code != '200'
    puts 'Error in request'
    exit(-1)
  end
  json_output = JSON.parse(response.body)
  json_output['folder'].each do |resource|
    id = resource['_id']
    # Query Girder to get the path of each resource.
    uri_string = 'http://localhost:8080/api/v1/resource/' + id +
                 '/path?type=folder'
    uri = URI(uri_string)
    response = Net::HTTP.get(uri)
    # Remove the quotes around the returned paths.
    puts response.delete('"')
  end
end
