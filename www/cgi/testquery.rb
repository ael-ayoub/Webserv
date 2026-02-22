#!/usr/bin/env ruby
require 'cgi'

# Read environment
request_method = ENV['REQUEST_METHOD'] || 'GET'
query_string   = ENV['QUERY_STRING']   || ''
content_length = (ENV['CONTENT_LENGTH'] || '0').to_i
content_type   = ENV['CONTENT_TYPE']   || ''

# Read POST body from stdin
body = ''
if content_length > 0
  body = $stdin.read(content_length) rescue ''
end

# Parse a urlencoded key=value&... string into a hash of key => [values]
def parse_params(str)
  params = {}
  return params if str.nil? || str.empty?
  str.split('&').each do |pair|
    key, value = pair.split('=', 2)
    next unless key
    key   = CGI.unescape(key)
    value = CGI.unescape(value || '')
    (params[key] ||= []) << value
  end
  params
end

def render_params(params)
  if params.empty?
    puts '    <p class="empty">(none)</p>'
  else
    params.each do |key, values|
      values.each do |value|
        puts "    <p><span class=\"key\">#{CGI.escapeHTML(key)}</span> = <span class=\"val\">#{CGI.escapeHTML(value)}</span></p>"
      end
    end
  end
end

query_params = parse_params(query_string)
body_params  = parse_params(body)

puts "Content-Type: text/html; charset=utf-8\r\n\r\n"
puts <<~HTML
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <title>CGI Query Test</title>
    <style>
      * { box-sizing: border-box; margin: 0; padding: 0; }
      body { font-family: 'Courier New', monospace; background: #111; color: #eee; padding: 2rem; }
      h2   { font-size: 1.3rem; text-transform: uppercase; letter-spacing: 2px;
             border-bottom: 2px solid #f80; padding-bottom: .4rem; margin-bottom: 1rem; }
      .block { margin-bottom: 1.8rem; }
      .meta  { font-size: .8rem; color: #888; margin-bottom: 1.2rem;
               background: #1e1e1e; padding: .6rem 1rem; border: 1px solid #333; }
      .meta span { color: #7ec8e3; }
      p    { padding: .35rem 0; border-bottom: 1px solid #222; }
      .key { color: #f80; font-weight: bold; }
      .val { color: #a8ff78; }
      .empty { color: #555; font-style: italic; }
    </style>
  </head>
  <body>
HTML

puts '<div class="meta">'
puts "  METHOD: <span>#{CGI.escapeHTML(request_method)}</span> &nbsp;|&nbsp; "
puts "  CONTENT-TYPE: <span>#{CGI.escapeHTML(content_type.empty? ? '(none)' : content_type)}</span> &nbsp;|&nbsp; "
puts "  CONTENT-LENGTH: <span>#{content_length}</span>"
puts '</div>'

puts '<div class="block">'
puts '<h2>Query String Params</h2>'
render_params(query_params)
puts '</div>'

puts '<div class="block">'
puts '<h2>Body Params (POST)</h2>'
render_params(body_params)
puts '</div>'

puts '</body></html>'
