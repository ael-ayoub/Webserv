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
    print("<html lang='en'><head><meta charset='utf-8'>")
    print("<meta name='viewport' content='width=device-width, initial-scale=1'>")
    print("<title>UPLOADS // MANAGER</title>")
    print("<link rel='stylesheet' href='/style.css'>")

    # Minimal table styles that use the shared CSS variables
    print("<style>")
    print("""
.upload-wrap { max-width: 960px; margin: 0 auto; padding: 2rem; }

table {
  width: 100%;
  border-collapse: collapse;
  border: var(--border-thick);
  border-radius: var(--radius);
  overflow: hidden;
  box-shadow: var(--shadow);
  font-size: 0.88rem;
}

thead { background: var(--bg-panel); }

th {
  text-align: left;
  padding: 0.85rem 1.25rem;
  border-bottom: var(--border-thick);
  font-size: 0.72rem;
  text-transform: uppercase;
  letter-spacing: 1px;
  color: var(--text-secondary);
  font-weight: 600;
}

td {
  padding: 0.85rem 1.25rem;
  border-bottom: var(--border-thin);
  color: var(--text-primary);
}

tbody tr:last-child td { border-bottom: none; }

tbody tr:hover { background: var(--bg-panel); }

td a {
  color: var(--accent-alt);
  text-decoration: none;
  font-weight: 600;
}

td a:hover { text-decoration: underline; }

.btn-open {
  padding: 0.4rem 0.9rem;
  background: var(--accent);
  color: #fff;
  border: none;
  border-radius: var(--radius);
  font-family: 'JetBrains Mono', monospace;
  font-weight: 600;
  font-size: 0.75rem;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  cursor: pointer;
  margin-right: 6px;
  transition: background 0.15s;
}
.btn-open:hover { background: #e0a03a; }

.btn-del {
  padding: 0.4rem 0.9rem;
  background: transparent;
  color: var(--accent-red);
  border: 1px solid var(--accent-red);
  border-radius: var(--radius);
  font-family: 'JetBrains Mono', monospace;
  font-weight: 600;
  font-size: 0.75rem;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  cursor: pointer;
  transition: background 0.15s, color 0.15s;
}
.btn-del:hover { background: var(--accent-red); color: #fff; }

.empty-note {
  color: var(--text-muted);
  font-size: 0.88rem;
  margin-top: 1rem;
}
    """)
    print("</style></head><body>")

    # print("<header>")
    # print("  <div class='title-block'><h1 class='brutalist-title'>UPLOADS</h1></div>")
    # print("  <nav>")
    # print("    <a href='/index.html'>HOME</a>")
    # print("    <a href='/about.html'>ABOUT</a>")
    # print("    <a href='/upload.html'>UPLOAD</a>")
    # print("    <a href='/uploads_manager.html'>UPLOADS</a>")
    # print("    <a href='/session.html'>SESSION</a>")
    # print("    <a href='/cgi_test.html'>CGI TEST</a>")
    # print("  </nav>")
    # print("</header>")

    # print("<main><div class='upload-wrap'>")
    # print("<h2 class='section-title'>UPLOAD MANAGER</h2>")
    # print("<p style='color:var(--text-secondary);font-size:0.88rem;margin-bottom:1.5rem;'>")
    # print("Open files or remove them via DELETE.</p>")

    if not files:
        print("<p class='empty-note'>&#9656; NO UPLOADED FILES FOUND.</p>")
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
            print("<button class='btn-open' onclick=\"window.open('/upload/%s','_blank')\">OPEN</button>" % url_name)
            print("<button class='btn-del' onclick=\"delFile('%s','%s')\">DELETE</button>" % (url_name, safe))
            print("</td>")
            print("</tr>")
        print("</tbody></table>")

    print("</div></main>")

    # print("<footer><p>&copy; WEBSERV C++98</p></footer>")

    print("<script src='/theme.js'></script>")
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