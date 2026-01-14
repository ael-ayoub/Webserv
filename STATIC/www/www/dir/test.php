<?php
header("Content-Type: text/html; charset=utf-8");

$poems = [
    [
        "title" => "Digital Whispers",
        "lines" => [
            "In circuits deep, where data flows,",
            "A server hums, and no one knows,",
            "The silent dance of ones and zeros,",
            "Creating worlds for digital heroes."
        ]
    ],
    [
        "title" => "The Request",
        "lines" => [
            "A packet flies through endless night,",
            "Seeking servers, pure and bright,",
            "TCP handshakes, soft and swift,",
            "Through fiber cables, data drift."
        ]
    ],
    [
        "title" => "Code & Coffee",
        "lines" => [
            "Morning light on keyboard keys,",
            "Debugging through uncertainties,",
            "Coffee cooling, spirits high,",
            "Building dreams that never die."
        ]
    ],
    [
        "title" => "Midnight Deploy",
        "lines" => [
            "In darkness, code begins to flow,",
            "To production servers, soft and slow,",
            "A prayer sent up with every commit,",
            "Please work this time, just a little bit."
        ]
    ]
];

$selectedPoem = $poems[array_rand($poems)];
$timestamp = date("l, F j, Y \a\\t g:i A");


$quotes = [
    "Simplicity is the ultimate sophistication. — Leonardo da Vinci",
    "Code is like humor. When you have to explain it, it's bad. — Cory House",
    "First, solve the problem. Then, write the code. — John Johnson",
    "The best error message is the one that never shows up. — Thomas Fuchs",
    "Talk is cheap. Show me the code. — Linus Torvalds"
];
$quote = $quotes[array_rand($quotes)];
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP Poetry Generator</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Georgia', serif;
            background: #fafafa;
            color: #333;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 40px 20px;
            line-height: 1.8;
        }
        
        .container {
            max-width: 600px;
            width: 100%;
        }
        
        .poem-card {
            background: white;
            padding: 60px 50px;
            border-radius: 2px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
            margin-bottom: 30px;
        }
        
        .poem-title {
            font-size: 28px;
            font-weight: 400;
            color: #222;
            margin-bottom: 40px;
            text-align: center;
            letter-spacing: 0.5px;
        }
        
        .poem-line {
            font-size: 18px;
            color: #444;
            margin-bottom: 18px;
            font-style: italic;
            text-align: center;
        }
        
        .poem-line:last-child {
            margin-bottom: 0;
        }
        
        .divider {
            width: 60px;
            height: 1px;
            background: #ddd;
            margin: 40px auto;
        }
        
        .quote {
            background: white;
            padding: 30px 40px;
            border-left: 3px solid #333;
            border-radius: 2px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
            margin-bottom: 30px;
        }
        
        .quote-text {
            font-size: 16px;
            color: #555;
            line-height: 1.6;
        }
        
        .meta {
            text-align: center;
            color: #999;
            font-size: 12px;
            letter-spacing: 1px;
            text-transform: uppercase;
            margin-top: 20px;
        }
        
        .refresh-btn {
            display: block;
            margin: 0 auto;
            padding: 12px 30px;
            background: #333;
            color: white;
            border: none;
            border-radius: 2px;
            font-size: 12px;
            letter-spacing: 1.5px;
            text-transform: uppercase;
            cursor: pointer;
            transition: background 0.3s ease;
            text-decoration: none;
        }
        
        .refresh-btn:hover {
            background: #555;
        }
        
        @media (max-width: 600px) {
            .poem-card {
                padding: 40px 30px;
            }
            
            .poem-title {
                font-size: 22px;
            }
            
            .poem-line {
                font-size: 16px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="poem-card">
            <h1 class="poem-title"><?php echo htmlspecialchars($selectedPoem['title']); ?></h1>
            
            <?php foreach ($selectedPoem['lines'] as $line): ?>
                <p class="poem-line"><?php echo htmlspecialchars($line); ?></p>
            <?php endforeach; ?>
            
            <div class="divider"></div>
            
            <p style="text-align: center; color: #999; font-size: 11px; font-style: normal; letter-spacing: 1px;">
                BY SERAPH
            </p>
        </div>
        
        <div class="quote">
            <p class="quote-text"><?php echo htmlspecialchars($quote); ?></p>
        </div>
        
        <p class="meta"><?php echo $timestamp; ?></p>
        
        <button class="refresh-btn" onclick="window.location.reload();" style="margin-top: 30px;">Generate New</button>
    </div>
</body>
</html>
