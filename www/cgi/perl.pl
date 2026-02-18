#!/usr/bin/perl
use strict;
use warnings;

# Get some system stats for the header
my $hostname = `hostname` || "Ubuntu-Node";
my $server_software = $ENV{'SERVER_SOFTWARE'} || "Unknown WebServ";

# print "Content-type: text/html\r\n\r\n";
print <<HTML;
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Perl CGI Debugger</title>
    <style>
        :root {
            --bg: #1f2430ff;
            --card: #353c4eff;
            --text: #cbccc6;
            --accent: #ffcc66;
            --tag: #5ccfe6;
            --val: #95e6cb;
            --border: #707a8c33;
        }
        body { 
            background: var(--bg); color: var(--text); 
            font-family: 'JetBrains Mono', 'Fira Code', monospace; 
            padding: 40px; margin: 0;
        }
        .header { border-bottom: 1px solid var(--border); padding-bottom: 20px; margin-bottom: 30px; }
        h1 { color: var(--accent); margin: 0; font-size: 1.8rem; }
        .grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(450px, 1fr)); gap: 10px; }
        .env-card { 
            background: var(--card); padding: 12px 18px; 
            border-radius: 4px; display: flex; align-items: center;
            border-left: 2px solid var(--tag);
        }
        .key { color: var(--tag); font-weight: bold; width: 180px; flex-shrink: 0; font-size: 0.85rem; }
        .value { color: var(--val); word-break: break-all; font-size: 0.85rem; }
        .label { color: #707a8c; font-size: 0.7rem; margin-bottom: 20px; display: block; }
    </style>
</head>
<body>
    <div class="header">
        <h1>PERL_ENV_DEBUGGER</h1>
        <div style="margin-top: 5px; color: var(--border);">Host: $hostname | Server: $server_software</div>
    </div>

    <span class="label">DUMPING %ENV HASH...</span>
    <div class="grid">
HTML

# Sort keys alphabetically and print them
foreach my $key (sort keys %ENV) {
    my $value = $ENV{$key} // "[NOT SET]";
    print qq(
        <div class="env-card">
            <span class="key">$key</span>
            <span class="value">$value</span>
        </div>
    );
}

print <<HTML;
    </div>
    <div style="margin-top: 40px; text-align: center; color: var(--border); font-size: 0.7rem;">
        CGI Specification 1.1 compliant test script
    </div>
</body>
</html>
HTML