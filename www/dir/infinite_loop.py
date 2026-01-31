#!/usr/bin/env python3
# Test CGI script with infinite loop

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>This script will run forever...</h1>")

# Infinite loop
while True:
    pass
