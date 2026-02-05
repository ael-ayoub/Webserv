#!/usr/bin/env python3
"""
Test script for upload functionality and file listing
"""

import requests
import os
import time

BASE_URL = "http://localhost:1033"

def test_upload_listing():
    """Test that uploaded files can be listed"""
    print("\n" + "="*60)
    print("TEST: Upload Directory Listing")
    print("="*60)
    
    try:
        response = requests.get(f"{BASE_URL}/upload/")
        print(f"Status: {response.status_code}")
        
        if response.status_code == 200:
            print("✓ Upload directory listing works!")
            
            # Check if our sample files are listed
            content = response.text
            if "test_uploaded_cgi.py" in content:
                print("✓ Python CGI script is listed")
            if "sample.html" in content:
                print("✓ HTML file is listed")
            if "sample.txt" in content:
                print("✓ Text file is listed")
            if "DIRECTORIES" in content and "FILES" in content:
                print("✓ Styled listing page is working")
        else:
            print(f"✗ Failed with status {response.status_code}")
    except Exception as e:
        print(f"✗ Error: {e}")

def test_file_download():
    """Test that files can be downloaded/viewed"""
    print("\n" + "="*60)
    print("TEST: File Download/View")
    print("="*60)
    
    files_to_test = [
        ("sample.txt", "text/plain"),
        ("sample.html", "text/html"),
        ("README.txt", "text/plain")
    ]
    
    for filename, expected_type in files_to_test:
        try:
            response = requests.get(f"{BASE_URL}/upload/{filename}")
            print(f"\n{filename}:")
            print(f"  Status: {response.status_code}")
            print(f"  Content-Type: {response.headers.get('Content-Type', 'N/A')}")
            
            if response.status_code == 200:
                print(f"  ✓ File can be accessed")
            else:
                print(f"  ✗ Failed with status {response.status_code}")
        except Exception as e:
            print(f"  ✗ Error: {e}")

def test_cgi_execution():
    """Test that uploaded CGI scripts can be executed"""
    print("\n" + "="*60)
    print("TEST: CGI Script Execution from Upload Directory")
    print("="*60)
    
    try:
        response = requests.get(f"{BASE_URL}/upload/test_uploaded_cgi.py")
        print(f"Status: {response.status_code}")
        print(f"Content-Type: {response.headers.get('Content-Type', 'N/A')}")
        
        if response.status_code == 200:
            content = response.text
            if "CGI Execution" in content and "Works!" in content:
                print("✓ CGI script executed successfully!")
                print("✓ Python CGI from upload directory works!")
            else:
                print("✗ CGI script returned unexpected content")
        else:
            print(f"✗ Failed with status {response.status_code}")
    except Exception as e:
        print(f"✗ Error: {e}")

def test_file_upload():
    """Test file upload via form"""
    print("\n" + "="*60)
    print("TEST: File Upload")
    print("="*60)
    
    # Create a test file
    test_content = f"Test file uploaded at {time.strftime('%Y-%m-%d %H:%M:%S')}"
    test_filename = f"test_upload_{int(time.time())}.txt"
    
    try:
        files = {
            'file': (test_filename, test_content, 'text/plain')
        }
        response = requests.post(f"{BASE_URL}/uploads", files=files)
        print(f"Status: {response.status_code}")
        
        if response.status_code == 200:
            print(f"✓ File uploaded successfully!")
            
            # Verify file appears in listing
            time.sleep(0.5)
            list_response = requests.get(f"{BASE_URL}/upload/")
            if test_filename in list_response.text:
                print(f"✓ Uploaded file appears in directory listing")
                
                # Verify file can be retrieved
                get_response = requests.get(f"{BASE_URL}/upload/{test_filename}")
                if test_content in get_response.text:
                    print(f"✓ Uploaded file content matches")
            else:
                print(f"✗ Uploaded file not found in listing")
        else:
            print(f"✗ Upload failed with status {response.status_code}")
    except Exception as e:
        print(f"✗ Error: {e}")

def main():
    print("\n" + "="*60)
    print("WEBSERV UPLOAD & FILE LISTING TEST SUITE")
    print("="*60)
    print("\nMake sure Webserv is running on http://localhost:1033")
    print("Press Ctrl+C to cancel")
    
    try:
        # Test if server is running
        response = requests.get(BASE_URL, timeout=2)
        print("✓ Server is running\n")
    except:
        print("✗ Server is not reachable. Please start ./Webserv first\n")
        return
    
    test_upload_listing()
    test_file_download()
    test_cgi_execution()
    test_file_upload()
    
    print("\n" + "="*60)
    print("TEST SUITE COMPLETE")
    print("="*60)
    print("\nManual Testing:")
    print(f"  • View upload page: {BASE_URL}/upload.html")
    print(f"  • Browse files:     {BASE_URL}/upload/")
    print(f"  • Run CGI:          {BASE_URL}/upload/test_uploaded_cgi.py")
    print()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nTest interrupted by user")
    except Exception as e:
        print(f"\n✗ Unexpected error: {e}")
