#!/usr/bin/env python3
import random
from datetime import datetime

# A collection of quotes to cycle through
quotes = [
    {"text": "The only way to do great work is to love what you do.", "author": "Steve Jobs"},
    {"text": "Simplicity is the ultimate sophistication.", "author": "Leonardo da Vinci"},
    {"text": "Move fast and break things. Unless you are breaking things, you are not moving fast enough.", "author": "Mark Zuckerberg"},
    {"text": "Code is like humor. When you have to explain it, it’s bad.", "author": "Cory House"},
    {"text": "Perfection is achieved not when there is nothing more to add, but when there is nothing left to take away.", "author": "Antoine de Saint-Exupéry"},
    {"text": "Stay hungry, stay foolish.", "author": "Whole Earth Catalog"},
    {"text": "Design is not just what it looks like and feels like. Design is how it works.", "author": "Steve Jobs"}
]

# Select a random quote
selected_quote = random.choice(quotes)
current_time = datetime.now().strftime("%A, %B %d")

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ayu Zen - Daily Inspiration</title>
    <style>
        :root {{
            --bg: #0d1017;
            --card-bg: #11141d;
            --accent: #ffcc66; /* Ayu Mirage Orange */
            --fg: #cccac2;
            --dim: #5c6773;
            --sky: #5ccfe6;
        }}

        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        body {{
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            background-color: var(--bg);
            color: var(--fg);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            overflow: hidden;
        }}

        /* Subtle background glow */
        body::before {{
            content: '';
            position: absolute;
            width: 300px;
            height: 300px;
            background: var(--accent);
            filter: blur(150px);
            opacity: 0.05;
            top: 10%;
            left: 10%;
            z-index: -1;
        }}

        .quote-container {{
            max-width: 800px;
            padding: 60px;
            text-align: left;
            position: relative;
        }}

        .date-label {{
            font-family: 'JetBrains Mono', monospace;
            color: var(--sky);
            font-size: 14px;
            letter-spacing: 2px;
            text-transform: uppercase;
            margin-bottom: 20px;
            display: block;
        }}

        .quote-text {{
            font-size: 48px;
            font-weight: 700;
            line-height: 1.2;
            color: var(--fg);
            margin-bottom: 30px;
            position: relative;
        }}

        /* The decorative quote mark */
        .quote-text::before {{
            content: '“';
            position: absolute;
            left: -50px;
            top: -10px;
            font-size: 100px;
            color: var(--accent);
            opacity: 0.3;
            font-family: serif;
        }}

        .quote-author {{
            font-size: 18px;
            color: var(--dim);
            font-style: italic;
            display: flex;
            align-items: center;
        }}

        .quote-author::before {{
            content: '';
            width: 40px;
            height: 1px;
            background: var(--accent);
            display: inline-block;
            margin-right: 15px;
        }}

        .refresh-hint {{
            position: absolute;
            bottom: 40px;
            font-family: 'JetBrains Mono', monospace;
            font-size: 12px;
            color: var(--dim);
            text-transform: lowercase;
            letter-spacing: 1px;
        }}

        @media (max-width: 768px) {{
            .quote-text {{ font-size: 32px; }}
            .quote-container {{ padding: 30px; }}
            .quote-text::before {{ display: none; }}
        }}
    </style>
</head>
<body>
    <div class="quote-container">
        <span class="date-label">{current_time}</span>
        <div class="quote-text">
            {selected_quote['text']}
        </div>
        <div class="quote-author">
            {selected_quote['author']}
        </div>
    </div>

    <div class="refresh-hint">
        [ F5 to generate new wisdom ]
    </div>
</body>
</html>""")