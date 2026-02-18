#!/usr/bin/ruby
require 'cgi'

# Data Gathering
hostname = `hostname`.strip rescue "c3r9p2"
uptime   = `uptime -p`.delete_prefix("up ").strip rescue "N/A"
load_avg = File.read('/proc/loadavg').split[0..2] rescue ["0.00", "0.00", "0.00"]
mem_raw  = `free -m | grep Mem`.split 
mem_pct  = ((mem_raw[2].to_f / mem_raw[1].to_f) * 100).round(1) rescue 0
disk_pct = `df / --output=pcent | tail -1`.strip.delete('%').to_i rescue 0
cpu_temp = `cat /sys/class/thermal/thermal_zone0/temp`.to_f / 1000 rescue 0

# Ayu Mirage Color Palette Logic
def get_ayu_color(val, threshold = 80)
  val.to_f > threshold ? "#f28779" : "#95e6cb"
end

# puts "Content-type: text/html\r\n\r\n"
puts <<~HTML
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="10"> <title>#{hostname} // Dashboard</title>
    <style>
        :root {
            --bg: #1f2430;
            --card: #242936;
            --text: #cbccc6;
            --accent: #ffcc66;
            --dim: #707a8c;
            --green: #95e6cb;
            --orange: #ffa759;
            --red: #f28779;
        }
        body { 
            background-color: var(--bg);
            color: var(--text);
            font-family: 'Inter', -apple-system, sans-serif;
            margin: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
        }
        .container { width: 85%; max-width: 1100px; }
        
        .header { margin-bottom: 40px; border-left: 4px solid var(--accent); padding-left: 20px; }
        .header h1 { font-size: 2.8rem; margin: 0; color: var(--accent); letter-spacing: -1px; }
        .header p { color: var(--dim); font-family: monospace; margin-top: 5px; text-transform: uppercase; }

        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(240px, 1fr)); gap: 20px; }
        .card { 
            background: var(--card);
            padding: 25px;
            border-radius: 4px; /* Ayu uses sharper corners */
            border-bottom: 2px solid transparent;
            transition: all 0.2s ease;
        }
        .card:hover { border-bottom-color: var(--accent); background: #2b3240; }
        
        .label { font-size: 0.75rem; color: var(--dim); font-weight: 700; text-transform: uppercase; letter-spacing: 1px; }
        .value { font-size: 2rem; font-weight: 300; display: block; margin: 10px 0; font-family: 'JetBrains Mono', monospace; }
        
        .meter { height: 4px; width: 100%; background: #191e2a; border-radius: 2px; overflow: hidden; }
        .fill { height: 100%; transition: width 0.5s ease; }

        .footer { margin-top: 50px; color: var(--dim); font-size: 0.8rem; font-family: monospace; text-align: center; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>#{hostname}</h1>
            <p>System Status // Uptime: #{uptime}</p>
        </div>

        <div class="grid">
            <div class="card">
                <span class="label">Load Average (1m)</span>
                <span class="value" style="color: var(--orange)">#{load_avg[0]}</span>
                <div class="meter"><div class="fill" style="width: #{[load_avg[0].to_f*50, 100].min}%; background: var(--orange)"></div></div>
            </div>

            <div class="card">
                <span class="label">Memory Usage</span>
                <span class="value" style="color: #{get_ayu_color(mem_pct)}">#{mem_pct}%</span>
                <div class="meter"><div class="fill" style="width: #{mem_pct}%; background: #{get_ayu_color(mem_pct)}"></div></div>
                <div style="font-size: 0.7rem; color: var(--dim); margin-top: 8px;">#{mem_raw[2]}MB / #{mem_raw[1]}MB</div>
            </div>

            <div class="card">
                <span class="label">Disk Usage (/)</span>
                <span class="value" style="color: #{get_ayu_color(disk_pct)}">#{disk_pct}%</span>
                <div class="meter"><div class="fill" style="width: #{disk_pct}%; background: #{get_ayu_color(disk_pct)}"></div></div>
            </div>

            <div class="card">
                <span class="label">CPU Temp</span>
                <span class="value" style="color: #{get_ayu_color(cpu_temp, 65)}">#{cpu_temp.round(1)}&deg;C</span>
                <div class="meter"><div class="fill" style="width: #{cpu_temp}%; background: #{get_ayu_color(cpu_temp, 65)}"></div></div>
            </div>
        </div>

        <div class="footer">
            [ WEBSERV_CGI_TEST ] — LAST UPDATED: #{Time.now.strftime('%H:%M:%S')}
        </div>
    </div>
</body>
</html>
HTML