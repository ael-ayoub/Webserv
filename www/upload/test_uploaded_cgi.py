#!/usr/bin/env python3
"""
Test CGI script that can be uploaded and executed
"""
print("Content-Type: text/html")
print()
print("<!DOCTYPE html>")
print("<html><head><title>Upload CGI Test</title></head>")
print("<body style='font-family: monospace; padding: 2rem;'>")
print("<h1>✅ CGI Execution from Upload Directory Works!</h1>")
print("<p>This Python script was uploaded via the upload form and executed as CGI.</p>")
print("<p><strong>Location:</strong> /www/upload/test_uploaded_cgi.py</p>")
print("<hr>")
print("<h2>Server Info:</h2>")
import os
import sys
print(f"<p>Python Version: {sys.version}</p>")
print(f"<p>Working Directory: {os.getcwd()}</p>")
print(f"<p>Script Path: {os.path.abspath(__file__)}</p>")
print("</body></html>")
