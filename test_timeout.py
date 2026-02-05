#!/usr/bin/env python3
"""
Test script to verify timeout functionality for:
1. Slow requests (clients that don't send data)
2. CGI scripts with infinite loops
"""

import socket
import time

TARGET_HOST = "localhost"
TARGET_PORT = 1033

def test_slow_request():
    """Test that slow requests timeout after REQUEST_TIMEOUT seconds"""
    print("\n=== Testing Slow Request Timeout ===")
    print(f"Opening connection to {TARGET_HOST}:{TARGET_PORT}...")
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((TARGET_HOST, TARGET_PORT))
    
    print("Connected. Sending partial request and waiting...")
    # Send only partial request
    sock.send(b"GET / HTTP/1.1\r\n")
    
    print("Waiting for timeout (should be ~30 seconds)...")
    start_time = time.time()
    
    try:
        # Wait for server to close connection or send timeout response
        data = sock.recv(4096)
        elapsed = time.time() - start_time
        
        if data:
            response = data.decode('utf-8', errors='ignore')
            print(f"Received response after {elapsed:.1f} seconds:")
            print(response[:200])
            
            if "408" in response or "Timeout" in response:
                print("✓ Server correctly sent timeout response")
            else:
                print("✗ Server sent unexpected response")
        else:
            print(f"✗ Connection closed without response after {elapsed:.1f} seconds")
    except socket.timeout:
        print("✗ Socket timeout (server didn't respond)")
    except Exception as e:
        elapsed = time.time() - start_time
        print(f"Connection closed after {elapsed:.1f} seconds: {e}")
    finally:
        sock.close()

def test_cgi_timeout():
    """Test that CGI scripts timeout after CGI_TIMEOUT seconds"""
    print("\n=== Testing CGI Timeout ===")
    print(f"Requesting slow CGI script from {TARGET_HOST}:{TARGET_PORT}...")
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(10)  # 10 second socket timeout
    
    try:
        sock.connect((TARGET_HOST, TARGET_PORT))
        
        # Request the slow Python CGI script
        request = (
            "GET /dir/slow_script.py HTTP/1.1\r\n"
            f"Host: {TARGET_HOST}:{TARGET_PORT}\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        
        print("Sending request for slow_script.py (sleeps 10s, timeout at 5s)...")
        sock.send(request.encode())
        
        start_time = time.time()
        response = b""
        
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            response += chunk
        
        elapsed = time.time() - start_time
        
        print(f"Received response after {elapsed:.1f} seconds:")
        print(response.decode('utf-8', errors='ignore')[:500])
        
        if elapsed < 7:
            print(f"✓ CGI script timed out correctly (took {elapsed:.1f}s, expected ~5s)")
        else:
            print(f"✗ CGI script took too long ({elapsed:.1f}s)")
            
    except socket.timeout:
        print("✗ Socket timeout waiting for response")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        sock.close()

def main():
    print("=" * 60)
    print("Timeout Testing Script")
    print("=" * 60)
    print("\nMake sure Webserv is running on", f"{TARGET_HOST}:{TARGET_PORT}")
    print("Press Ctrl+C to cancel at any time")
    
    try:
        # Test CGI timeout (shorter, runs first)
        test_cgi_timeout()
        
        # Test slow request timeout (takes 30 seconds)
        response = input("\nDo you want to test slow request timeout (takes ~30s)? [y/N]: ")
        if response.lower() == 'y':
            test_slow_request()
        else:
            print("Skipping slow request test.")
        
        print("\n" + "=" * 60)
        print("Testing complete!")
        print("=" * 60)
        
    except KeyboardInterrupt:
        print("\n\nTest interrupted by user")
    except Exception as e:
        print(f"\nError: {e}")

if __name__ == "__main__":
    main()
