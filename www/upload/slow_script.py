#!/usr/bin/env python3
# Test CGI script that runs for 10 seconds (should timeout after 5)

import time

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>Starting slow script...</h1>")

time.sleep(10)

print("<p>If you see this, the timeout didn't work!</p>")
print("</body></html>")
