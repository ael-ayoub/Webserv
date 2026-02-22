#!/usr/bin/env python3
import sys
import os

request_method = os.environ.get('REQUEST_METHOD', 'GET')
query_string   = os.environ.get('QUERY_STRING', '')
content_type   = os.environ.get('CONTENT_TYPE', '')

content_length = 0
try:
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
except (ValueError, TypeError):
    content_length = 0

body = ""
if content_length > 0:
    body = sys.stdin.read(content_length)

def escape_html(s):
    return s.replace('&','&amp;').replace('<','&lt;').replace('>','&gt;')

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>CGI Echo</title>
  <style>
    * {{ box-sizing: border-box; margin: 0; padding: 0; }}
    body {{ font-family: 'Courier New', monospace; background: #111; color: #eee;
           padding: 2rem; }}
    h2   {{ font-size: 1.4rem; text-transform: uppercase; letter-spacing: 2px;
           margin-bottom: 1.5rem; border-bottom: 2px solid #f80; padding-bottom: .5rem; }}
    .block {{ margin-bottom: 1.5rem; }}
    .label {{ color: #f80; font-weight: bold; font-size: .85rem;
             text-transform: uppercase; letter-spacing: 1px; margin-bottom: .4rem; display: block; }}
    pre  {{ background: #1e1e1e; padding: 1rem; border: 1px solid #333;
           white-space: pre-wrap; word-break: break-all; font-size: .9rem;
           color: #7ec8e3; min-height: 2.5rem; }}
    .empty {{ color: #555; font-style: italic; }}
  </style>
</head>
<body>
  <h2>CGI Echo &#8212; Response</h2>

  <div class="block">
    <span class="label">REQUEST_METHOD</span>
    <pre>{escape_html(request_method)}</pre>
  </div>

  <div class="block">
    <span class="label">QUERY_STRING</span>
    <pre>{escape_html(query_string) if query_string else '<span class="empty">(empty)</span>'}</pre>
  </div>

  <div class="block">
    <span class="label">CONTENT_TYPE</span>
    <pre>{escape_html(content_type) if content_type else '<span class="empty">(empty)</span>'}</pre>
  </div>

  <div class="block">
    <span class="label">POST BODY</span>
    <pre>{escape_html(body) if body else '<span class="empty">(empty)</span>'}</pre>
  </div>
</body>
</html>""")
