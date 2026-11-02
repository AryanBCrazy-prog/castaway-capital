import http.server
import socketserver
import os
import webbrowser
from pathlib import Path

PORT = 8080
PROJECT_DIR = Path(__file__).parent
os.chdir(PROJECT_DIR)

class CustomHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # Prevent browser caching for instant JSON state updates
        self.send_header('Cache-Control', 'no-cache, no-store, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()

print("============================================================")
print("  CASTAWAY CAPITAL 3D BROWSER APPLICATION LAUNCHER")
print(f"  Live Board URL: http://localhost:{PORT}")
print("============================================================")

with socketserver.TCPServer(("", PORT), CustomHandler) as httpd:
    print(f"Opening browser at http://localhost:{PORT} ...")
    webbrowser.open(f"http://localhost:{PORT}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")
