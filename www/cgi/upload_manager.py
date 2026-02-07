#!/usr/bin/env python3

import os
from urllib.parse import quote


def html_escape(s: str) -> str:
    return (s.replace("&", "&amp;")
             .replace("<", "&lt;")
             .replace(">", "&gt;")
             .replace('"', "&quot;"))


def main():
    # List/manage the *upload* directory, but keep this CGI script outside it.
    web_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    upload_dir = os.path.join(web_root, "upload")

    try:
        entries = os.listdir(upload_dir)
    except Exception as e:
        print("Content-Type: text/html")
        print()
        print("<h1>Failed to list uploads</h1>")
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
    print("<title>Uploads Manager</title>")
    print("<style>")
    print("body{font-family:monospace;padding:16px}")
    print("table{width:100%;border-collapse:collapse}")
    print("th,td{border:2px solid #000;padding:8px;text-align:left}")
    print("button{font-family:monospace;border:2px solid #000;background:#fff;padding:6px 10px;cursor:pointer}")
    print("button:hover{background:#000;color:#fff}")
    print(".danger{border-color:#ff0000}")
    print(".danger:hover{background:#ff0000;color:#fff}")
    print("</style></head><body>")

    print("<h1>Uploaded Files</h1>")
    print("<p>Open files or delete them using DELETE.</p>")

    if not files:
        print("<p><strong>No uploaded files found.</strong></p>")
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
    print("  if(!confirm('Delete ' + label + '?')) return;")
    print("  fetch('/upload/' + urlName, {method: 'DELETE'})")
    print("    .then(r => { if(!r.ok && r.status !== 204) throw new Error('HTTP ' + r.status); })")
    print("    .then(() => location.reload())")
    print("    .catch(e => alert('Delete failed: ' + e));")
    print("}")
    print("</script>")

    print("</body></html>")


if __name__ == '__main__':
    main()
