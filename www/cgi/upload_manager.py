#!/usr/bin/env python3

import os
from urllib.parse import quote


def html_escape(s: str) -> str:
    return (s.replace("&", "&amp;")
             .replace("<", "&lt;")
             .replace(">", "&gt;")
             .replace('"', "&quot;"))


def main():
    web_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    upload_dir = os.path.join(web_root, "upload")

    try:
        entries = os.listdir(upload_dir)
    except Exception as e:
        print("Content-Type: text/html")
        print()
        print("<h1>FAILED TO LIST UPLOADS</h1>")
        print("<pre>%s</pre>" % html_escape(str(e)))
        return

    files = []
    for name in entries:
        if name in (".", ".."):
            continue
        full = os.path.join(upload_dir, name)
        if os.path.isfile(full):
            files.append(name)

    files.sort(key=lambda s: s.lower())

    print("Content-Type: text/html")
    print()

    print("<!doctype html>")
    print("<html><head><meta charset='utf-8'>")
    print("<meta name='viewport' content='width=device-width, initial-scale=1'>")
    print("<title>UPLOADS // MANAGER</title>")

    # ===== BRUTALIST AYU DARK =====
    print("<style>")
    print("""
:root {
  --bg: #0A0E14;
  --panel: #11151C;
  --border: #FFB454;
  --text: #E6E1CF;
  --accent: #59C2FF;
  --danger: #F07178;
  --muted: #626A73;
}

* {
  box-sizing: border-box;
}

body {
  margin: 0;
  padding: 32px;
  background: var(--bg);
  color: var(--text);
  font-family: monospace;
  font-size: 15px;
  line-height: 1.5;
}

h1 {
  margin: 0 0 24px 0;
  font-size: 28px;
  text-transform: uppercase;
  letter-spacing: 2px;
  border-bottom: 4px solid var(--border);
  padding-bottom: 12px;
}

p {
  color: var(--muted);
  margin-bottom: 24px;
}

table {
  width: 100%;
  border-collapse: collapse;
  border: 4px solid var(--border);
  background: var(--panel);
}

thead {
  background: #0D1117;
}

th {
  text-align: left;
  padding: 14px;
  border: 2px solid var(--border);
  text-transform: uppercase;
  font-size: 13px;
  letter-spacing: 1px;
}

td {
  padding: 14px;
  border: 2px solid var(--border);
}

tr:hover {
  background: #151A22;
}

a {
  color: var(--accent);
  text-decoration: none;
  font-weight: bold;
}

a:hover {
  text-decoration: underline;
}

button {
  font-family: monospace;
  font-weight: bold;
  padding: 8px 14px;
  margin-right: 8px;
  border: 3px solid var(--border);
  background: transparent;
  color: var(--text);
  cursor: pointer;
  text-transform: uppercase;
}

button:hover {
  background: var(--border);
  color: #000;
}

button.danger {
  border-color: var(--danger);
  color: var(--danger);
}

button.danger:hover {
  background: var(--danger);
  color: #000;
}

strong {
  color: var(--border);
}
    """)
    print("</style></head><body>")

    print("<h1>Uploads Manager</h1>")
    print("<p>Open files or delete them using DELETE.</p>")

    if not files:
        print("<p><strong>NO UPLOADED FILES FOUND.</strong></p>")
    else:
        print("<table>")
        print("<thead><tr><th>File</th><th>Actions</th></tr></thead>")
        print("<tbody>")
        for name in files:
            url_name = quote(name)
            safe = html_escape(name)
            print("<tr>")
            print("<td><a href='/upload/%s' target='_blank'>%s</a></td>" % (url_name, safe))
            print("<td>")
            print("<button onclick=\"window.open('/upload/%s','_blank')\">OPEN</button>" % url_name)
            print(" ")
            print("<button class='danger' onclick=\"delFile('%s','%s')\">DELETE</button>" % (url_name, safe))
            print("</td>")
            print("</tr>")
        print("</tbody></table>")

    print("<script>")
    print("function delFile(urlName, label){")
    print("  if(!confirm('DELETE ' + label + ' ?')) return;")
    print("  fetch('/upload/' + urlName, {method: 'DELETE'})")
    print("    .then(r => { if(!r.ok && r.status !== 204) throw new Error('HTTP ' + r.status); })")
    print("    .then(() => location.reload())")
    print("    .catch(e => alert('DELETE FAILED: ' + e));")
    print("}")
    print("</script>")

    print("</body></html>")


if __name__ == '__main__':
    main()