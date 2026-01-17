import threading
import http.client
import time

# CONFIGURATION
TARGET_HOST = "localhost"
TARGET_PORT = 1033
TARGET_PATH = "/"
CONCURRENT_THREADS = 50
TOTAL_REQUESTS = 1000

def send_request():
    try:
        conn = http.client.HTTPConnection(TARGET_HOST, TARGET_PORT)
        conn.request("GET", TARGET_PATH)
        response = conn.getresponse()
        # Read response to ensure request completes
        response.read()
        print(f"Status: {response.status}")
        conn.close()
    except Exception as e:
        print(f"Failed: {e}")

threads = []
print(f"Starting {TOTAL_REQUESTS} requests with {CONCURRENT_THREADS} concurrent threads...")

start_time = time.time()

for _ in range(TOTAL_REQUESTS):
    # Simple throttle to maintain concurrency limit
    while threading.active_count() > CONCURRENT_THREADS:
        time.sleep(0.01)
    
    t = threading.Thread(target=send_request)
    t.start()
    threads.append(t)

# Wait for all to finish
for t in threads:
    t.join()

print(f"\nFinished in {time.time() - start_time:.2f} seconds")