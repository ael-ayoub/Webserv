#!/bin/bash

TIME=$(date +"%H:%M:%S")
DATE=$(date +"%A, %B %d, %Y")
TIMEZONE=$(date +"%Z")

echo ""

cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="1"> <title>Ayu System Clock</title>
    <style>
        :root {
            --bg: #1f2430;
            --accent: #ffcc66;
            --text: #cbccc6;
            --dim: #707a8c;
            --blue: #5ccfe6;
        }
        body {
            background-color: var(--bg);
            color: var(--text);
            font-family: 'JetBrains Mono', 'Fira Code', monospace;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            overflow: hidden;
        }
        .clock-container {
            text-align: center;
            border: 2px solid var(--dim);
            padding: 60px 100px;
            border-radius: 4px;
            position: relative;
        }
        .label {
            position: absolute;
            top: -12px;
            left: 20px;
            background: var(--bg);
            padding: 0 10px;
            color: var(--accent);
            font-size: 0.8rem;
            letter-spacing: 2px;
        }
        .time {
            font-size: 8rem;
            font-weight: bold;
            color: var(--accent);
            margin: 0;
            line-height: 1;
        }
        .date {
            font-size: 1.5rem;
            color: var(--blue);
            margin-top: 20px;
            letter-spacing: 1px;
        }
        .footer {
            margin-top: 30px;
            color: var(--dim);
            font-size: 0.9rem;
        }
    </style>
</head>
<body>
    <div class="clock-container">
        <span class="label">SYSTEM_TIME_SYNC</span>
        <div class="time">$TIME</div>
        <div class="date">$DATE</div>
        <div class="footer">LOCATION: 1337.MA | ZONE: $TIMEZONE</div>
    </div>
</body>
</html>
EOF